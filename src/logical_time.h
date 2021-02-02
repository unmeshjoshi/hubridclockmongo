#pragma once

#include "timestamp.h"

namespace mongo {

class BSONObj;
class BSONObjBuilder;

/**
 *  The LogicalTime class holds the cluster time of the cluster. It provides conversions to
 *  a Timestamp to allow integration with opLog.
 */
class LogicalTime {
public:
    LogicalTime() = default;
    explicit LogicalTime(Timestamp ts);

    /**
     * Parses the 'operationTime' field of the specified object and extracts a LogicalTime from it.
     * If 'operationTime' is missing or of the wrong type, throws.
     */
    static LogicalTime fromOperationTime(Timestamp &obj);

    /**
     * Appends "operationTime" field to the specified builder as a Timestamp type.
     */
    Timestamp appendAsOperationTime() const;

    Timestamp asTimestamp() const {
        return Timestamp(_time);
    }

    /**
     * Increases the _time by ticks.
     */
    void addTicks(uint64_t ticks);

    /**
     * Const version, returns the LogicalTime with increased _time by ticks.
     */
    LogicalTime addTicks(uint64_t ticks) const;

    std::string toString() const;

    /**
     * Returns the LogicalTime as an array of unsigned chars in little endian order for use with the
     * crypto::hmacSHA1 function.
     */
    std::array<unsigned char, sizeof(uint64_t)> toUnsignedArray() const;

    /**
     *  serialize into BSON object.
     */
    BSONObj toBSON() const;

    /**
     * An uninitialized value of LogicalTime. Default constructed.
     */
    static const LogicalTime kUninitialized;

private:
    uint64_t _time{0};
};

inline bool operator==(const LogicalTime& l, const LogicalTime& r) {
    return l.asTimestamp() == r.asTimestamp();
}

inline bool operator!=(const LogicalTime& l, const LogicalTime& r) {
    return !(l == r);
}

inline bool operator<(const LogicalTime& l, const LogicalTime& r) {
    return l.asTimestamp() < r.asTimestamp();
}

inline bool operator<=(const LogicalTime& l, const LogicalTime& r) {
    return (l < r || l == r);
}

inline bool operator>(const LogicalTime& l, const LogicalTime& r) {
    return (r < l);
}

inline bool operator>=(const LogicalTime& l, const LogicalTime& r) {
    return (l > r || l == r);
}

inline std::ostream& operator<<(std::ostream& s, const LogicalTime& v) {
    return (s << v.toString());
}

}  // namespace mongo
