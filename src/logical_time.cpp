#include "logical_time.h"

namespace mongo {

const LogicalTime LogicalTime::kUninitialized = LogicalTime();

LogicalTime::LogicalTime(Timestamp ts) :
		_time(ts.asULL()) {
}

LogicalTime LogicalTime::fromOperationTime(Timestamp &timestamp) {
	return LogicalTime(timestamp);
}

Timestamp LogicalTime::appendAsOperationTime() const {
	return asTimestamp();
}

void LogicalTime::addTicks(uint64_t ticks) {
	_time += ticks;
}

LogicalTime LogicalTime::addTicks(uint64_t ticks) const {
	return LogicalTime(Timestamp(_time + ticks));
}

std::string LogicalTime::toString() const {
	return std::to_string(_time);
}

}
