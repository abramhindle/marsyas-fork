#!/usr/bin/env python
import sys
import numpy
import math
from marsyas import * 
import marsyas
import marsyas_util
import random

mars = MarSystemManager()
buffsize=512
series = ["Series/output", ["RealvecGrainSource/real_src", "AudioSink/dest"]]
print "Make Network"
this_net = marsyas_util.create(series)
print "Set InSamples"
this_net.updControl("mrs_natural/inSamples", buffsize)
print "Set Rate"
this_net.updControl("mrs_real/israte", 44100.0)
print "Make grains"
grain1 = marsyas.realvec(buffsize)
grain2 = marsyas.realvec(buffsize)
grain3 = marsyas.realvec(buffsize)
grain4 = marsyas.realvec(3*buffsize)
for i in range(0, buffsize):
    grain1[i] = math.sin(i*440/buffsize)
    grain2[i] = random.uniform(-1,1)
    grain3[i] = math.sin(math.tan(math.sin(math.tan(i/40))))

for i in range(0, buffsize*3):
    grain4[i] = math.sin(math.tan(i/100))



data_uri = "RealvecGrainSource/real_src/mrs_realvec/data"

grains = [[1,grain1],[2,grain2],[3,grain3],[4,grain4]]
#grains = [[4,grain4]]
n = len(grains)
for graint in grains:
    grain_id = graint[0]
    grain = graint[1]
    print ("Loading grain %d" % grain_id)
    print "Set Index"
    myindex = this_net.getControl("RealvecGrainSource/real_src/mrs_natural/index")
    myindex.setValue_natural( grain_id)
    #this_net.updControl("RealvecGrainSource/real_src/mrs_natural/index", grain_id)
    control = this_net.getControl(data_uri)
    print "Set Grain"
    control.setValue_realvec(grain)
    print "Commit"
    this_net.updControl("RealvecGrainSource/real_src/mrs_bool/commit",marsyas.MarControlPtr.from_bool(True))


output_net_begin_control = this_net.getControl("RealvecGrainSource/real_src/mrs_realvec/schedule")

this_net.updControl("AudioSink/dest/mrs_bool/initAudio",marsyas.MarControlPtr.from_bool(True))

print "Starting Loop"
t = 0
i = 0
c = 1
while 1:
    size = random.randint(1,1000)
    if (i % 100 == 0):
        c = c + 1
    nn = 3
    schedule = marsyas.realvec(nn * size)
    for j in range(0,size):
        schedule[j*nn + 0] = random.randint(0,44100) # in how many samples should be play it
        schedule[j*nn + 1] = random.randint(1,n) # which instrument
        schedule[j*nn + 2] = 0.1
    # this sets a schedule
    output_net_begin_control = this_net.getControl("RealvecGrainSource/real_src/mrs_realvec/schedule")
    output_net_begin_control.setValue_realvec(schedule)
    # this commits the schedule
    this_net.updControl("RealvecGrainSource/real_src/mrs_bool/schedcommit",marsyas.MarControlPtr.from_bool(True))
    this_net.tick()
    t = t + buffsize
    i = i + 1
