/*
** Copyright (C) 2013 Abram Hindle <abram.hindle@softwareprocess.es>
** originally RealvecSource.h
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

#ifndef MARSYAS_REALVECGRAINSOURCE_H
#define MARSYAS_REALVECGRAINSOURCE_H

#include "MarSystem.h"	
	
#include <vector>
#include <map>
#include <queue>

namespace Marsyas
{
/**
   Little Helper for Schedule
*/
class SchedTuple {
 public:
	int when; // at what sample?
	int current; // are we already playing? if so how far
	int index; // what sample are we playing?
	float amp; // how loud?
	SchedTuple(int when_, int index_, float amp_ = 0.0, int current_=0) 
	{
		when = when_;
		index = index_;
		current = current_;
		amp = amp_;
	}
	// this is slightly confusing
	// basically NEAREST or SOONEST are LARGEST
	bool operator<(const SchedTuple& rhs) 
	{
		return (when > rhs.when || (when == rhs.when && index > rhs.index));
	}
};
 inline bool operator< (const SchedTuple& lhs, const SchedTuple& rhs) 
 { 
	 return (lhs.when > rhs.when || (lhs.when == rhs.when && lhs.index > rhs.index));
 }
 /** 
     \class RealvecGrainSource
     \ingroup IO
     
     A RealvecGrainSource takes a realvec and uses it as a source for your
     MarSystem network.  It is similar to a SoundFileSource except that
     as taking a sound file as the input, it takes a realvec that you
     create as input.
     
     It is very useful for getting numbers from external sources into
     your Marsystems, for example when reading external files of data.
     
     It is also very useful when writing tests for your Marsystems.
     You can load up a realvec with data, and then run it through a
     Marsystem to test it.
     
     When you feed in a realvec, the rows turn into observations and
     the columns turn into samples.

 */

 
 class RealvecGrainSource: public MarSystem
 {
 private: 
	 
	 MarControlPtr ctrl_data_;
	 MarControlPtr ctrl_index_;
	 MarControlPtr ctrl_commit_;
	 MarControlPtr ctrl_schedule_;
	 MarControlPtr ctrl_schedcommit_;
	 std::map<int, realvec> grains;
	 std::priority_queue<SchedTuple> schedule;
	 std::vector<SchedTuple> playlist;
	 std::vector<SchedTuple> newplaylist;
	 
	 void addControls();
	 void myUpdate(MarControlPtr sender);
	 void myPlay(SchedTuple & st, realvec & out, int onSamples_);
	
	 mrs_natural count_;
	 
 public:
	 RealvecGrainSource(std::string name);
	 RealvecGrainSource(const RealvecGrainSource& a);
	 ~RealvecGrainSource();
	 MarSystem* clone() const;  
	 
	 void myProcess(realvec& in, realvec& out);
	 
	 void addGrain( const int index, const realvec& data );
	 void scheduleGrain(const realvec& schedule );
 };
 
 
}//namespace Marsyas

#endif

