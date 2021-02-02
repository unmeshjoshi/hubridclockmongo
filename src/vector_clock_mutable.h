#pragma once

#include "vector_clock.h"

namespace mongo {

/**
 * A vector clock service that additionally permits being advanced authoritatively ("ticking").
 *
 * Only linked in contexts where ticking is allowed, ie. mongod, embedded, mongod-based unittests.
 */
class VectorClockMutable : public VectorClock {
public:
    /**
     * Returns the next time value for the component, and provides a guarantee that any future call
     * to tick() will return a value at least 'nTicks' ticks in the future from the current time.
     */
    LogicalTime tickClusterTime(uint64_t nTicks) {
        return _tick(Component::ClusterTime, nTicks);
    }
    LogicalTime tickConfigTime(uint64_t nTicks) {
        return _tick(Component::ConfigTime, nTicks);
    }
    LogicalTime tickTopologyTime(uint64_t nTicks) {
        return _tick(Component::TopologyTime, nTicks);
    }

    /**
     * Authoritatively ticks the current time of the specified component to newTime.
     *
     * For ClusterTime, this should only be used for initializing from a trusted source, eg. from an
     * oplog timestamp.
     */
    void tickClusterTimeTo(LogicalTime newTime) {
        _tickTo(Component::ClusterTime, newTime);
    }
    void tickConfigTimeTo(LogicalTime newTime) {
        _tickTo(Component::ConfigTime, newTime);
    }
    void tickTopologyTimeTo(LogicalTime newTime) {
        _tickTo(Component::TopologyTime, newTime);
    }

protected:
    VectorClockMutable();
    virtual ~VectorClockMutable();

    /**
     * Called by sub-classes in order to actually tick a Component time, once they have determined
     * that doing so is permissible.
     *
     * Returns as per tick(), ie. returns the next time value, and guarantees that future calls will
     * return at least nTicks later.
     */
    LogicalTime _advanceComponentTimeByTicks(Component component, uint64_t nTicks);

    /**
     * Called by sublclasses in order to actually tickTo a Component time, once they have determined
     * that doing so is permissible.
     */
    void _advanceComponentTimeTo(Component component, LogicalTime&& newTime);

    /**
     * Returns the next time value for the component, and provides a guarantee that any future call
     * to tick() will return a value at least 'nTicks' ticks in the future from the current time.
     */
    virtual LogicalTime _tick(Component component, uint64_t nTicks) = 0;

    /**
     * Authoritatively ticks the current time of the Component to newTime.
     *
     * For ClusterTime, this should only be used for initializing from a trusted source, eg. from an
     * oplog timestamp.
     */
    virtual void _tickTo(Component component, LogicalTime newTime) = 0;
};

}  // namespace mongo
