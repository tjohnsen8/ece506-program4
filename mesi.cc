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
    newline->set_state(I);
    signalRd(addr, processor_number);
  }
  else {
    // If the line is cached in the processor cache, do not forget
    // to update the LRU
    update_LRU(line);
  }


	// response_replies etc... Invoke the relevant directory
	// signal functions like signalRd or signalRdX etc...

}
//
void MESI::PrWr(ulong addr, int processor_number) {

  // Update the Per-cache global counter to maintain LRU
  // order among cache ways, updated on every cache access
  // ^ we think this is current_cycle, if LRU or replacement is wrong, one area to look
  // Increment the global write counter.
  // Check whether the line is cached in the processor cache.
  current_cycle++;
  writes++;
  cache_line *line = find_line(addr);

  // see if line is in cache
  if (line == NULL || line->get_state() == I) {
    // is not found, increment miss counter
    write_misses++;
    // now allocate new line
    cache_line *newLine = allocate_line(addr);
    newLine->set_state(M);

    // send ReadX request
    signalRdX(addr, processor_number);

  } else if (line->get_state() == E) {
    // cache hit!
    line->set_state(M);
    update_LRU(line);
  } else if (line->get_state() == M) {
    // cache hit!
    update_LRU(line);
  } else if (line->get_state() == S) {
    // cache hit!
    // action needed:
    // send upgrade request
    signalUpgr(addr, processor_number);
    line->set_state(M);
  }

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

  signal_rds++;

  // Check the directory state and update the cache2cache counter
  cache_line* line = find_line(addr);
  dir_entry* dir_line = directory->find_dir_line(line->get_tag());
  if (dir_line == NULL) {
    memory_transactions++;
    // cache line is not in any cache - set state to E
    line->set_state(E);
    // create in directory - find_empty_line will always return a line (exits if full)
    // Check whether the directory entry is updated. If not updated,
    // create a fresh entry in the directory, update the sharer vector or list.
    dir_line = directory->find_empty_line(0);
    dir_line->set_dir_tag(line->get_tag());
    // Update the directory state (U, EM, S_).
    dir_line->set_dir_state(EM);
  } else {
    // directory found the line, so it exists in other caches
    cache2cache++;
    dir_state ds = dir_line->get_state();
    if(ds == EM){
        // update the directory state
        dir_line->set_dir_state(S_);
        // Send Intervention
        interventions++;
        dir_line->sendInt_to_sharer(addr, 0, processor_number);
    }
    // Update the cache line state
    line->set_state(S);
  }
  // update the sharer vector or list.
  dir_line->add_sharer_entry(processor_number);
}

void MESI::signalRdX(ulong addr, int processor_number){
	  // YOUR CODE HERE
    signal_rdxs++;
	  // Change cache state to M
    //Change Directory state to EM
    //Invalidate Sharers
    cache_line* line = find_line(addr);
    dir_entry* dir_line = directory->find_dir_line(line->get_tag());
    if(dir_line != NULL){
        dir_line->set_dir_state(EM);
        line->set_state(M);
        invalidations++;
        dir_line->sendInv_to_sharer(addr, 0, processor_number);
    }
}

void MESI::signalUpgr(ulong addr, int processor_number){
	// YOUR CODE HERE
	// Refer to signalUpgr description in the handout
    signal_upgrs++;
    cache_line* line = find_line(addr);
    dir_entry* dir_line = directory->find_dir_line(line->get_tag());
    if(dir_line != NULL) {
        dir_line->set_dir_state(EM);
        line->set_state(M);
        invalidations++;
        dir_line->sendInv_to_sharer(addr, 0, processor_number);
        cache2cache++;
    }
}

// Int is called by the directory to a cache that the directory has tracked as a
//  owner of the cache line. This means that the directory thinks the processor
//  that this function is running on has a copy of the cached line in the E or M
//  state.
void MESI::Int(ulong addr) {
	// YOUR CODE HERE
	// The below comments are for reference and might not be
	// sufficient to match the debug runs.
	//
	// Update the relevant counter, if the cache copy is dirty,
	// same needs to be written back to main memory. This is
	// achieved by simply updating the writeback counter
  cache_line* line = find_line(addr);

  if (line == NULL || line->get_state() == I || line->get_state() == S)
    return;

  // in the E or M state
  flushes++;

  if (line->get_state() == M)
    write_back(0);

  // always go to shared state
  line->set_state(S);

}

// Inv is called by the directory to a cache that the directory has tracked as a
//  sharer of the cache line. This means that the directory thinks the processor
//  that this function is running on has a copy of the cached line in the E, S, M
//  state. If the cache is in I state, no Inv *should* be sent
void MESI::Inv(ulong addr) {
  // find the line in cache
  cache_line* line = find_line(addr);

  // if there's nothing in cache, or if the cache is already invalid
  //  then just leave this function
  // TODO: if line is I, should an InvAck be sent?
  if (line == NULL || line->get_state() == I)
    return;

  // for states S, E, M, ack the invalidation
  inv_acks++;

  if (line->get_state() == E) {
    // flush required
    flushes++;
  } else if (line->get_state() == M) {
    // flush and write back required
    flushes++;
    write_back(0);
  }

  // always set to state I
  line->set_state(I);
}
