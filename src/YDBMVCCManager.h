/*
 * YDBMVCCManager.h
 *
 *  Created on: 27-Dec-2021
 *      Author: unmesh
 */

#ifndef SRC_YDBMVCCMANAGER_H_
#define SRC_YDBMVCCMANAGER_H_
#include "timestamp.h"
#include <condition_variable>
#include <mutex>
#include <deque>
#include <queue>
#include <vector>

using namespace mongo;
using namespace std;

class YDBMVCCManager {
public:
	YDBMVCCManager();
	int Now() {
		return now; //for testing
	}
	int max_safe_time_returned_without_lease_;
	std::deque<int> queue_;
	int DoGetSafeTime(int min_allowed, std::unique_lock<std::mutex>* lock);
	virtual ~YDBMVCCManager();
    mutable std::condition_variable cond_;
    int now;

};

#endif /* SRC_YDBMVCCMANAGER_H_ */
