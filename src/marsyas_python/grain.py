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
for i in range(0, buffsize):
    grain1[i] = math.sin(i*440/buffsize)
    grain2[i] = random.uniform(-1,1)
    grain3[i] = math.sin(math.tan(math.sin(math.tan(i/40))))

data_uri = "RealvecGrainSource/real_src/mrs_realvec/data"

grains = [[1,grain1],[2,grain2],[3,grain3]]

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
while 1:
    n = 3
    size = 3
    schedule = marsyas.realvec(n * size)
    schedule[0] = t + 0 #when
    schedule[1] = 1
    schedule[2] = 1 #amp
    schedule[3] = t + 128 #when
    schedule[4] = 2
    schedule[5] = 1 #amp
    schedule[6] = t + 256 #when
    schedule[7] = 2
    schedule[8] = 1 #amp
    output_net_begin_control.setValue_realvec(schedule)
    this_net.updControl("RealvecGrainSource/real_src/mrs_bool/schedcommit",marsyas.MarControlPtr.from_bool(True))
    this_net.tick()
    t = t + buffsize
