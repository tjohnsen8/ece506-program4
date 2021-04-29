/************************************************************
			Course		: 	CSC/ECE506
			Source 		:	ssci.cc
			Owner		:	Ed Gehringer
			Email Id	:	efg@ncsu.edu
*************************************************************/
#include "ssci.h"

typedef std::list<int>::iterator ssci_it;

void SSCI::add_sharer_entry(int proc_no){
	cache_list.push_back(proc_no);
}

void SSCI::remove_sharer_entry(int proc_num){
	// Remove the entry from the linked list
  cache_list.remove_if([&proc_num] (int proc) {
    if (proc == proc_num)
      return true;
    else
      return false;
  });
}

int SSCI::is_cached(int proc_num) {
  return (cache_list.size() > 0 ? 1 : 0);
}

void SSCI::sendInv_to_sharer(ulong addr, int num_proc, int proc_num){
	// YOUR CODE HERE
	//
	// Invoke the sendInv function defined in the main function
  // Remove sharer AFTER sending the Invalidate
  ssci_it it = cache_list.begin();
  while ( it != cache_list.end()) {
    if (*it != proc_num)
    {
      sendInv(addr, *it);
      remove_sharer_entry(*it++);
    }
    else
      it++;
  }
}

void SSCI::sendInt_to_sharer(ulong addr, int num_proc, int proc_num){
	// YOUR CODE HERE
	//
	// Invoke the sendInv function defined in the main function
	// for all the entries in the list except for proc_num.
  for (ssci_it it = cache_list.begin(); it != cache_list.end(); it++) {
    if (*it != proc_num)
      sendInt(addr, *it);
  }
}
