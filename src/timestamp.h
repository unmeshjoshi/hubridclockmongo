#pragma once
#include <string>
#include <tuple> 
#include <sys/time.h>

namespace mongo {

unsigned long long curTimeMillis64();


class BSONObj;

/**
 * Timestamp: A combination of a count of seconds since the POSIX epoch plus an ordinal value.
 */
class Timestamp {
public:
    // Timestamp to signal that the storage engine should take unstable checkpoints.
    static const Timestamp kAllowUnstableCheckpointsSentinel;

    // Maximum Timestamp value.
    static Timestamp max();

    // Returns the minimum timestamp. Used in the context of selecting and ordering storage engine
    // snapshots.
    static Timestamp min() {
        return Timestamp();
    }

    /**
     * Constructor that builds a Timestamp from a Date_t by using the high-order 4 bytes of "date"
     * for the "secs" field and the low-order 4 bytes for the "i" field.
     */
    // explicit Timestamp(Date_t date) : Timestamp(date.toULL()) {}

    /**
     * Constructor that builds a Timestamp from a 64-bit unsigned integer by using
     * the high-order 4 bytes of "v" for the "secs" field and the low-order 4 bytes for the "i"
     * field.
     */
    // explicit Timestamp(unsigned long long val) : Timestamp(val >> 32, val) {}

    // Timestamp(Seconds s, unsigned increment) : Timestamp(s.count(), increment) {}

    Timestamp(unsigned a, unsigned b) : i(b), secs(a) {}

    Timestamp() = default;

    unsigned getSecs() const {
        return secs;
    }

    unsigned getInc() const {
        return i;
    }

    unsigned long long asULL() const {
        unsigned long long result = secs;
        result <<= 32;
        result |= i;
        return result;
    }
    long long asLL() const {
        return static_cast<long long>(asULL());
    }

    bool isNull() const {
        return secs == 0;
    }

    // std::string toStringPretty() const;

    std::string toString() const;

    bool operator==(const Timestamp& r) const {
        return tie() == r.tie();
    }
    bool operator!=(const Timestamp& r) const {
        return tie() != r.tie();
    }
    bool operator<(const Timestamp& r) const {
        return tie() < r.tie();
    }
    bool operator<=(const Timestamp& r) const {
        return tie() <= r.tie();
    }
    bool operator>(const Timestamp& r) const {
        return tie() > r.tie();
    }
    bool operator>=(const Timestamp& r) const {
        return tie() >= r.tie();
    }

    Timestamp operator+(unsigned long long inc) const {
        return Timestamp(asULL() + inc);
    }

    Timestamp operator-(unsigned long long inc) const {
        return Timestamp(asULL() - inc);
    }
    
    explicit Timestamp(unsigned long long val) : Timestamp(val >> 32, val) {}   
private:
    std::tuple<unsigned, unsigned> tie() const {
        return std::tie(secs, i);
    }

   

    unsigned i = 0;
    unsigned secs = 0;
};

}  // namespace mongo
