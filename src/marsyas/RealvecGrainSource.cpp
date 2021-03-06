/*
** Copyright (C) 2013 Abram Hindle <abram.hindle@softwareprocess.es>
** originally RealvecGrainSource.cpp
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "RealvecGrainSource.h"
#include "common_source.h"

/*
	TODOS:
		- [ ] Add Vectors

*/

using namespace std;
using namespace Marsyas;

RealvecGrainSource::RealvecGrainSource(mrs_string name):MarSystem("RealvecGrainSource",name)
{
	count_= 0;
	addControls();
        
}

RealvecGrainSource::RealvecGrainSource(const RealvecGrainSource& a):MarSystem(a)
{
	count_ = 0;
	ctrl_data_ = getctrl("mrs_realvec/data");
        ctrl_index_ = getctrl("mrs_natural/index");
        ctrl_commit_ = getctrl("mrs_bool/commit");
        ctrl_schedule_ = getctrl("mrs_realvec/schedule");
        ctrl_schedcommit_ = getctrl("msr_bool/schedcommit");
        addControls();
}


RealvecGrainSource::~RealvecGrainSource()
{
}


MarSystem* 
RealvecGrainSource::clone() const 
{
	return new RealvecGrainSource(*this);
}

void 
RealvecGrainSource::addControls()
{
	int last_index = 0;
	addctrl("mrs_bool/done", false);
	setctrlState("mrs_bool/done", true);
	addctrl("mrs_realvec/data", realvec(), ctrl_data_);
	// reals as index? gross!
	addctrl("mrs_natural/index", last_index, ctrl_index_);
	addctrl("mrs_bool/commit", false, ctrl_commit_); // commit a slice
	// alternating values
	// sample_to_play, index, amp
	addctrl("mrs_realvec/schedule", realvec(), ctrl_schedule_);
	addctrl("mrs_bool/schedcommit", false, ctrl_schedcommit_); // commit a slice        
	setctrlState("mrs_natural/index", true);
	setctrlState("mrs_realvec/data", true);
	setctrlState("mrs_real/israte", true);
	setctrlState("mrs_bool/commit", true);
	setctrlState("mrs_realvec/schedule", true);
	setctrlState("mrs_bool/schedcommit", true);

}


void
RealvecGrainSource::myUpdate(MarControlPtr sender)
{
	(void)sender;
	
	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
	
	// This is lame, basically we check if we want to commit a grain
	const bool& commit = ctrl_commit_->to<bool> ();
	if (commit) 
	{
		const int index = ctrl_index_->to<mrs_natural> ();
		const realvec& data = ctrl_data_->to<realvec> ();
		addGrain(index, data);
	}
	// This is lame, basically we check if we want to commit a schedule
	const bool& schedcommit = ctrl_schedcommit_->to<bool> ();
	if (schedcommit) 
	{

		const realvec& schedule = ctrl_schedule_->to<realvec> ();
		scheduleGrain( schedule );
	}

	setctrl("mrs_natural/onObservations", 1);//data.getRows());
	setctrl("mrs_natural/onSamples", inSamples_);
	setctrl("mrs_real/osrate", israte_);
	setctrl("mrs_bool/commit", false);
	setctrl("mrs_bool/schedcommit", false);
	
	if( getctrl("mrs_bool/done")->isTrue()){
		setctrl("mrs_bool/done", false);
	}
}

void 
RealvecGrainSource::myProcess(realvec& in, realvec& out)
{ 
	mrs_natural o,t;
	(void) in; 
	newplaylist.clear();
	int lastCount = count_ + onSamples_;
	for (t=0; t < onSamples_; t++) 
	{
		out(0,t) = 0.0;//data(o,count_ + t);
	}
	while( !schedule.empty() && schedule.top().when < lastCount) 
	{
		SchedTuple st = schedule.top();
		schedule.pop();
		myPlay(st, out, onSamples_);
	}
	for (unsigned int i = 0; i < playlist.size(); i++) 
	{
		SchedTuple st = playlist[i];
		myPlay(st, out, onSamples_);
	}
	// we're done with the playlist
	// now copy our newplaylist to playlist
	playlist.clear();
	playlist.swap(newplaylist);
	count_ = lastCount;
}
void RealvecGrainSource::myPlay(SchedTuple & st, realvec& out, int onSamples_ ) 
{
	int start = st.when - count_;
	int lastCount = count_ + onSamples_;
	mrs_real amp = st.amp;
	int offset = 0;
	if (grains.count(st.index) > 0) 
	{
		realvec &grain = grains[st.index];
		int cols = grain.getCols();
		if (start < 0) 
		{
			// already playing?
			offset = -1 * start;
			start = 0;
		} else if (start > 0) 
		{
			offset = -start;
                }
                int msamp = min(onSamples_, cols - offset);
                for (t = start; t < msamp; t++) 
		{
			// TODO: Windowing
			out(0,t) = out(0,t) + amp * grain(0,t+offset);
                }
                int samples_played = lastCount - st.when;
                if (cols > samples_played) 
		{
			// if not done playing add to playlist
			newplaylist.push_back( st );
                }
	} else {
	  // warn the grain didn't exist!
          MRSERR("Grain didn't exist!");
	}
}
void RealvecGrainSource::addGrain(const int index, const realvec& data )
{
	// grains is a map
	grains[index] = data;
}
void RealvecGrainSource::scheduleGrain( const realvec& data )
{
	// sample_to_play, index, amp
	// maybe use observations..
	int cols = data.getCols();
	for (int i = 0 ; i < cols; i += 3) 
	{
		int when = ((int)data(0, i)) + count_;
		int index = (int)data(0, i+1);
		mrs_real amp = data(0, i+2);
		//worry - do we need new/alloc?
		SchedTuple s(when, index, amp);
		schedule.push(s);
	}
}
