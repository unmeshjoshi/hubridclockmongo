#include "logical_time.h"
#include "timestamp.h"
#include <iostream>
#include <time.h>
#include "vector_clock_mongod.h"

using namespace mongo;

int main()
{
	unsigned long long int now = mongo::curTimeMillis64();
	auto t1 = mongo::Timestamp(now, 0);
	auto t2 = mongo::Timestamp(now, 1);
	std::cout << (t1.asLL()) << std::endl;
	std::cout << (t2.asLL()) << std::endl;

	  Timestamp tX(1);
	    auto time = LogicalTime(t1);

	    auto v1 = new VectorClockMongoD();
	    v1->tickClusterTimeTo(time);
	    v1->tickClusterTime(1);
	    auto clusterTime = v1->getTime().clusterTime();
	    std::cout << (clusterTime.asTimestamp().asLL()) << std::endl;

	    v1->tickClusterTime(1);
	    auto clusterTime1 = v1->getTime().clusterTime();
	    std::cout << (clusterTime1.asTimestamp().asLL()) << std::endl;

	return 0;
}
