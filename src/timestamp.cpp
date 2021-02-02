#include "timestamp.h"


#include <cstdint>
#include <ctime>
#include <iostream>
#include <limits>
#include <sstream>

namespace mongo {

unsigned long long curTimeMillis64() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long)tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

const Timestamp Timestamp::kAllowUnstableCheckpointsSentinel = Timestamp(0, 1);

Timestamp Timestamp::max() {
    unsigned int t = static_cast<unsigned int>(std::numeric_limits<uint32_t>::max());
    unsigned int i = std::numeric_limits<uint32_t>::max();
    return Timestamp(t, i);
}

// std::string Timestamp::toStringPretty() const {
//     std::stringstream ss;
//     ss << time_t_to_String_short(secs) << ':' << i;
//     return ss.str();
// }

std::string Timestamp::toString() const {
    std::stringstream ss;
    ss << "Timestamp(" << secs << ", " << i << ")";
    return ss.str();
}

// BSONObj Timestamp::toBSON() const {
//     BSONObjBuilder bldr;
//     bldr.append("", *this);
//     return bldr.obj();
// }
}  // namespace mongo
