/*
 * YDBMVCCManager.cpp
 *
 *  Created on: 27-Dec-2021
 *      Author: unmesh
 */

#include "YDBMVCCManager.h"
#include <iostream>

YDBMVCCManager::YDBMVCCManager() {
	// TODO Auto-generated constructor stub

}

YDBMVCCManager::~YDBMVCCManager() {
	// TODO Auto-generated destructor stub
}


int YDBMVCCManager::DoGetSafeTime(int min_allowed,
                                      std::unique_lock<std::mutex>* lock) {


  int result;
  auto predicate = [this, &result,min_allowed] {
    if (queue_.empty()) {
      result = Now();
    } else {
      result = queue_.front();
    }

    std::cout << result << std::endl << min_allowed<< std::endl;
    return result >= min_allowed;
  };

  // In the case of an empty queue, the safe hybrid time to read at is only limited by hybrid time
  // ht_lease, which is by definition higher than min_allowed, so we would not get blocked.
   std::cout << "Waiting for safe time to be" << min_allowed << std::endl;
   cond_.wait(*lock, predicate);
   max_safe_time_returned_without_lease_ = result;
   std::cout << "returning result " << result << std::endl;
  return result;
}
