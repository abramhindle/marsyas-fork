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
	last_index = 0;
	addctrl("mrs_bool/done", false);
	setctrlState("mrs_bool/done", true);
	addctrl("mrs_realvec/data", realvec(), ctrl_data_);
	// reals as index? gross!
	addctrl("mrs_natural/index", last_index, ctrl_index_);
	addctrl("mrs_bool/commit", false, ctrl_commit_); // commit a slice
	// alternating values
	// sample_to_play, index, amp, window
	addctrl("mrs_realvec/schedule", realvec(), ctrl_schedule_);
	addctrl("mrs_bool/schedcommit", false, ctrl_schedcommit_); // commit a slice
	setctrlState("mrs_natural/index", true);
	setctrlState("mrs_realvec/data", true);
	setctrlState("mrs_real/israte", true);
}


void
RealvecGrainSource::myUpdate(MarControlPtr sender)
{
	(void)sender;
	MRSDIAG("RealvecGrainSource.cpp - RealvecGrainSource:myUpdate");
	
	inSamples_ = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
	inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
	israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
	
	const bool& commit = ctrl_commit_->to<bool> ();
	if (commit) {
		const int& index = ctrl_index_->to<int> ();
		const realvec& data = ctrl_data_->to<realvec> ();
		addGrain(index, data);
	}
	const bool& commit = ctrl_commit_->to<bool> ();
	if (schedcommit) {
		const realvec& data = ctrl_schedule_->to<realvec> ();
		schedule( schedule );
	}

	setctrl("mrs_natural/onObservations", data.getRows());
	setctrl("mrs_natural/onSamples", inSamples_);
	setctrl("mrs_real/osrate", israte_);
	setctrl("mrs_bool/commit", false);
	setctrl("mrs_bool/schedcommit", false);
	
	count_ = 0;
	
	if( getctrl("mrs_bool/done")->isTrue()){
		setctrl("mrs_bool/done", false);
	}
}

void 
RealvecGrainSource::myProcess(realvec& in, realvec& out)
{ 
	mrs_natural o,t;
	(void) in; 
	//checkFlow(in,out);
	const realvec& data = ctrl_data_->to<realvec> ();
	
	for (o=0; o < onObservations_; o++)
	{
		for (t=0; t < onSamples_; t++)
		{
			out(o,t) = data(o,count_ + t);
		}
	}
	count_ += onSamples_;

	//out.dump();
}
