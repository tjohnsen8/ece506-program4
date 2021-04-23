/************************************************************
			Course		: 	CSC/ECE506
			Source 		:	mesi.cc
			Owner		:	Ed Gehringer
			Email Id	:	efg@ncsu.edu
*************************************************************/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
using namespace std;
#include "cache.h"

#include "main.h"
#include "mesi.h"
//
void MESI::PrRd(ulong addr, int processor_number) {
	// YOUR CODE HERE
	// The below comments are for reference and might not be
	// sufficient to match the debug runs.
	//
  //
	// Update the Per-cache global counter to maintain LRU
	// order among cache ways, updated on every cache access
  // ^ we think this is current_cycle, if LRU or replacement is wrong, one area to look
  // Increment the global read counter.
  // Check whether the line is cached in the processor cache.
  current_cycle++;
  reads++;
  cache_line* line = find_line(addr);

	// If not cached, allocate a fresh line and update the state. (M,E,S,I)
  if (line == NULL || line->get_state() == I) {
    // update counters
    // Increment the directory operation counter like signalrds,
    // Do not forget to update miss/hit counter
    read_misses++;
    response_replyds++;

    cache_line *newline = allocate_line(addr);
    dir_entry* dir_line = directory->find_dir_line(newline->get_tag());
    if (dir_line == NULL) {
      // cache line is not in any cache - set state to E
      newline->set_state(E);
      // create in directory - find_empty_line will always return a line (exits if full)
      // Check whether the directory entry is updated. If not updated,
      // create a fresh entry in the directory, update the sharer vector or list.
      dir_line = directory->find_empty_line(0);
      dir_line->set_dir_tag(newline->get_tag());
      // Update the directory state (U, EM, S_).
      dir_line->set_dir_state(EM);
    }
    else {
      // directory found the line, so it exists in other caches
      newline->set_state(S);
      // Update the directory state (U, EM, S_).
      dir_line->set_dir_state(S_);
    }
    // update the sharer vector or list.
    dir_line->add_sharer_entry(processor_number);
  }
  else {
    // If the line is cached in the processor cache, do not forget
    // to update the LRU
    update_LRU(line);
  }

  signal_rds++;


	// response_replies etc... Invoke the relevant directory
	// signal functions like signalRd or signalRdX etc...

}
//
void MESI::PrWr(ulong addr, int processor_number) {
	// YOUR CODE HERE
	// Refer comments for PrRd
}
//
cache_line * MESI::allocate_line(ulong addr) {
    ulong tag;
    cache_state state;

    cache_line *victim = find_line_to_replace(addr);
    assert(victim != 0);
    state = victim->get_state();
    if (state == M)
    {
      write_back(addr);
    }

  	ulong victim_tag = victim->get_tag();
  	dir_entry* dir_line = directory->find_dir_line(victim_tag);
  	if(dir_line!=NULL)	{
  		dir_line->remove_sharer_entry(cache_num);
  		int present = 0;
  		present = dir_line->is_cached(num_processors);
  		if(!present)
  			dir_line->state = U;
  	}

    tag = tag_field(addr);
    victim->set_tag(tag);
    victim->set_state(I);
    return victim;
}
//
void MESI::signalRd(ulong addr, int processor_number){
	// YOUR CODE HERE
	// The below comments are for reference and might not be
	// sufficient to match the debug runs.
	//
	// Check the directory state and update the cache2cache counter
    
    cache_line* line = find_line(addr);
    dir_entry* dir_line = directory->find_dir_line(line->tag);
    if(dir_line != NULL){
        cache2cache++;
        dir_state ds = dir_line->get_state();
        if(ds == EM){
            flushes++;
            write_backs++;
            // Update the directory state
            line->set_state(S);
            dir_line->set_dir_state(S);
            // Update the vector/list
            dir_line->add_sharer_entry(processor_number);
            // Send Intervention or Invalidation
            sendInt(addr, processor_number);
        }
    }
}

void MESI::signalRdX(ulong addr, int processor_number){
	// YOUR CODE HERE
	// Change cache state to M
    //Change Directory state to EM
    //Invalidate Sharers
    cache_line* line = find_line(addr);
    dir_entry* dir_line = directory->find_dir_line(line->tag);
    if(dir_line != NULL){
        dir_line->set_dir_state(EM);
        line->set_state(M);
        invalidations++;
        //I am not sure if this is what he meant when he said invalidate sharers.
        sendInv(addr, processor_number);
    }
}

void MESI::signalUpgr(ulong addr, int processor_number){
	// YOUR CODE HERE
	// Refer to signalUpgr description in the handout
    cache_line* line = find_line(addr);
    dir_entry* dir_line = directory->find_dir_line(line->tag);
    if(dir_line != NULL){
        dir_line->set_dir_state(U);
        line->set_state(I);
        invalidations++;
        cache2cache++;
    }
}

void MESI::Int(ulong addr) {
	// YOUR CODE HERE
	// The below comments are for reference and might not be
	// sufficient to match the debug runs.
	//
	// Update the relevant counter, if the cache copy is dirty,
	// same needs to be written back to main memory. This is
	// achieved by simply updating the writeback counter
}

void MESI::Inv(ulong addr) {
	// YOUR CODE HERE
	// Refer Inv description in the handout
}
