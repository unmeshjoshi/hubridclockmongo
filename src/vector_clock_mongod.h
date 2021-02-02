/*
 * vector_clock_mongod.h
 *
 *  Created on: 02-Feb-2021
 *      Author: unmesh
 */

#ifndef SRC_VECTOR_CLOCK_MONGOD_H_
#define SRC_VECTOR_CLOCK_MONGOD_H_

#include "vector_clock_mutable.h"

namespace mongo {


class VectorClockMongoD : public VectorClockMutable {
    VectorClockMongoD(const VectorClockMongoD&) = delete;
    VectorClockMongoD& operator=(const VectorClockMongoD&) = delete;

public:

    VectorClockMongoD();
    virtual ~VectorClockMongoD();

private:
    // VectorClock methods implementation


    LogicalTime _tick(Component component, uint64_t nTicks) override;
    void _tickTo(Component component, LogicalTime newTime) override;

    // ReplicaSetAwareService methods implementation


    /**
     * Structure used as keys for the map of waiters for VectorClock durability.
     */
    struct ComparableVectorTime {
        bool operator<(const ComparableVectorTime& other) const {
            return vt.configTime() < other.vt.configTime() ||
                vt.topologyTime() < other.vt.topologyTime();
        }
        bool operator>(const ComparableVectorTime& other) const {
            return vt.configTime() > other.vt.configTime() ||
                vt.topologyTime() > other.vt.topologyTime();
        }
        bool operator==(const ComparableVectorTime& other) const {
            return vt.configTime() == other.vt.configTime() &&
                vt.topologyTime() == other.vt.topologyTime();
        }

        VectorTime vt;
    };

    // If set to true, means that another operation already scheduled the `_queue` draining loop, if
    // false it means that this operation must do it
    bool _loopScheduled{false};

};

}

#endif /* SRC_VECTOR_CLOCK_MONGOD_H_ */
