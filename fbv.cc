/************************************************************
			Course		: 	CSC/ECE506
			Source 		:	fbv.cc
			Owner		:	Ed Gehringer
			Email Id	:	efg@ncsu.edu
*************************************************************/
#include "fbv.h"

void FBV::add_sharer_entry(int proc_num) {
  if (proc_num < 0 || proc_num >= 16)
    return;

	bit[proc_num] = true;
}

void FBV::remove_sharer_entry(int proc_num){
	if (proc_num < 0 || proc_num >= 16)
    return;

	// Reset the bit vector entry
  bit[proc_num] = false;
}

int FBV::is_cached(int num_proc){
	// Check bit vector for any set bit.
	// If set, return 1, else send 0
  for (int i=0; i<16; i++) {
    if (bit[i])
      return 1;
  }
  return 0;
}

void FBV::sendInt_to_sharer(ulong addr, int num_proc, int proc_num){
	// Invoke the sendInt function defined in main
	// for all the processors except for proc_num
	// Make sure that you check the FBV to see if the
	// bit is set
  for (int i=0; i<16; i++) {
    if (i != proc_num && bit[i]) {
      sendInt(addr, i);
    }
  }
}

void FBV::sendInv_to_sharer(ulong addr, int num_proc, int proc_num){
	// Invoke the sendInv function defined in main
	// for all the processors except for proc_num
	// Make sure that you check the FBV to see if the
	// bit is set
  for (int i=0; i<16; i++) {
    if (i != proc_num && bit[i]) {
      sendInv(addr, i);
    }
  }
}

