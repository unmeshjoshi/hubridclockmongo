#define MONGO_LOGV2_DEFAULT_COMPONENT ::mongo::logv2::LogComponent::kDefault

#include "vector_clock_mongod.h"

namespace mongo {


VectorClockMongoD::VectorClockMongoD() {};

VectorClockMongoD::~VectorClockMongoD() {};



LogicalTime VectorClockMongoD::_tick(Component component, uint64_t nTicks) {
    if (component == Component::ClusterTime) {
        // Although conceptually ClusterTime can only be ticked when a mongod is able to take writes
        // (ie. primary, or standalone), this is handled at a higher layer.
        //
        // ClusterTime is ticked when replacing zero-valued Timestamps with the current time, which
        // is usually but not necessarily associated with writes.
        //
        // ClusterTime is ticked after winning an election, while persisting the stepUp to the
        // oplog, which is slightly before the repl state is changed to primary.
        //
        // As such, ticking ClusterTime is not restricted here based on repl state.

        return _advanceComponentTimeByTicks(component, nTicks);
    }

    // tick is not permitted in other circumstances.
    std::abort();
}

void VectorClockMongoD::_tickTo(Component component, LogicalTime newTime) {
    if (component == Component::ClusterTime) {
        // The ClusterTime is allowed to tickTo in certain very limited and trusted cases (eg.
        // initializing based on oplog timestamps), so we have to allow it here.
        _advanceComponentTimeTo(component, std::move(newTime));
        return;
    }

    if (component == Component::ConfigTime) {
        _advanceComponentTimeTo(component, std::move(newTime));
        return;
    }

    if (component == Component::TopologyTime) {
        _advanceComponentTimeTo(component, std::move(newTime));
        return;
    }

    // tickTo is not permitted in other circumstances.
    std::abort();
}

 // namespace
}  // namespace mongo
