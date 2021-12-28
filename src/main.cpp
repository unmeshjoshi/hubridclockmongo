#include "logical_time.h"
#include "timestamp.h"
#include <iostream>
#include <time.h>
#include <thread>
#include "vector_clock_mongod.h"
#include "YDBMVCCManager.h"
using namespace mongo;

long i = 200;
// A dummy function
void foo(YDBMVCCManager *manager)
{
	while(true) {
		manager->now = i;
		i++;
		std::cout << "setting timestamp to " << i << std::endl;
		std::this_thread::sleep_for(200ms);
	}
}

void waitForSafeTime(YDBMVCCManager *manager) {

	std::mutex mutex_;
	std::unique_lock<std::mutex> lock(mutex_);
	auto safeTime = manager->DoGetSafeTime(2, &lock);
	std::cout << safeTime << std::endl;
}


int main() {
//	unsigned long long int now = mongo::curTimeMillis64();
//	auto t1 = mongo::Timestamp(now, 0);
//	auto t2 = mongo::Timestamp(now, 1);
//	std::cout << now << std::endl;
//	std::cout << (t1.asLL()) << std::endl;
//	std::cout << (t2.asLL()) << std::endl;
//
//	Timestamp tX(1);
//	auto time = LogicalTime(t1);
//
//	auto v1 = new VectorClockMongoD();
//	v1->tickClusterTimeTo(time);
//	v1->tickClusterTime(1);
//	auto clusterTime = v1->getTime().clusterTime();
//	std::cout << (clusterTime.asTimestamp().asLL()) << std::endl;
//
//	v1->tickClusterTime(1);
//	auto clusterTime1 = v1->getTime().clusterTime();
//	std::cout << (clusterTime1.asTimestamp().asLL()) << std::endl;

	YDBMVCCManager *manager = new YDBMVCCManager();
	manager->now = 200;

		// Pass f and its parameters to thread
	// object constructor as
	std::cout << "created thread" << std::endl;
	std::thread t1(foo, manager);
	std::thread t2(waitForSafeTime, manager);

	t2.join();

	return 0;
}
