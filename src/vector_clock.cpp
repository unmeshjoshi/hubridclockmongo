#include "vector_clock.h"
#include <mutex>
#include <memory>

namespace mongo {

class VectorClock::PlainComponentFormat : public VectorClock::ComponentFormat {
public:
    using ComponentFormat::ComponentFormat;
    virtual ~PlainComponentFormat() = default;
};

template <class ActualFormat>
class VectorClock::OnlyOutOnNewFCVComponentFormat : public ActualFormat {
public:
    using ActualFormat::ActualFormat;
    virtual ~OnlyOutOnNewFCVComponentFormat() = default;

};

class VectorClock::SignedComponentFormat : public VectorClock::ComponentFormat {
public:
    using ComponentFormat::ComponentFormat;
    virtual ~SignedComponentFormat() = default;


private:
    static constexpr char kClusterTimeFieldName[] = "clusterTime";
    static constexpr char kSignatureFieldName[] = "signature";
    static constexpr char kSignatureHashFieldName[] = "hash";
    static constexpr char kSignatureKeyIdFieldName[] = "keyId";
};


const VectorClock::ComponentArray<std::unique_ptr<VectorClock::ComponentFormat>>
    VectorClock::_gossipFormatters {
        std::make_unique<VectorClock::SignedComponentFormat>(VectorClock::kClusterTimeFieldName),
        std::make_unique<VectorClock::OnlyOutOnNewFCVComponentFormat<VectorClock::PlainComponentFormat>>(VectorClock::kConfigTimeFieldName),
        std::make_unique<VectorClock::OnlyOutOnNewFCVComponentFormat<VectorClock::PlainComponentFormat>>(VectorClock::kTopologyTimeFieldName)
	};


VectorClock::VectorTime VectorClock::getTime() const {
//    stdx::lock_guard<Latch> lock(_mutex);
	return VectorTime(_vectorTime);
}

bool VectorClock::_lessThanOrEqualToMaxPossibleTime(LogicalTime time,
		uint64_t nTicks) {
	return time.asTimestamp().getSecs() <= kMaxValue
			&& time.asTimestamp().getInc() <= (kMaxValue - nTicks);
}
//
//void VectorClock::_ensurePassesRateLimiter(ServiceContext* service,
//                                           const LogicalTimeArray& newTime) {
//    const unsigned wallClockSecs =
//        durationCount<Seconds>(service->getFastClockSource()->now().toDurationSinceEpoch());
//    auto maxAcceptableDriftSecs = static_cast<const unsigned>(gMaxAcceptableLogicalClockDriftSecs);
//
//    for (auto newIt = newTime.begin(); newIt != newTime.end(); ++newIt) {
//        auto newTimeSecs = newIt->asTimestamp().getSecs();
//        auto name = _componentName(Component(newIt - newTime.begin()));
//
//        // Both values are unsigned, so compare them first to avoid wrap-around.
//        uassert(ErrorCodes::ClusterTimeFailsRateLimiter,
//                str::stream() << "New " << name << ", " << newTimeSecs
//                              << ", is too far from this node's wall clock time, " << wallClockSecs
//                              << ".",
//                ((newTimeSecs <= wallClockSecs) ||
//                 (newTimeSecs - wallClockSecs) <= maxAcceptableDriftSecs));
//
//        uassert(40484,
//                str::stream() << name << " cannot be advanced beyond its maximum value",
//                _lessThanOrEqualToMaxPossibleTime(*newIt, 0));
//    }
//}

void VectorClock::_advanceTime(LogicalTimeArray &&newTime) {
//    _ensurePassesRateLimiter(_service, newTime);

//    stdx::lock_guard<Latch> lock(_mutex);

	auto it = _vectorTime.begin();
	auto newIt = newTime.begin();
	for (; it != _vectorTime.end() && newIt != newTime.end(); ++it, ++newIt) {
		if (*newIt > *it) {
			*it = std::move(*newIt);
		}
	}
}

std::string VectorClock::_componentName(Component component) {
	return _gossipFormatters[component]->_fieldName;
}

bool VectorClock::isEnabled() const {
//    stdx::lock_guard<Latch> lock(_mutex);
	return _isEnabled;
}

void VectorClock::_disable() {
//    stdx::lock_guard<Latch> lock(_mutex);
	_isEnabled = false;
}

void VectorClock::resetVectorClock_forTest() {
//    stdx::lock_guard<Latch> lock(_mutex);
	auto it = _vectorTime.begin();
	for (; it != _vectorTime.end(); ++it) {
		*it = LogicalTime();
	}
	_isEnabled = true;
}

void VectorClock::_advanceTime_forTest(Component component,
		LogicalTime newTime) {
	LogicalTimeArray newTimeArray;
	newTimeArray[component] = newTime;
	_advanceTime(std::move(newTimeArray));
}

VectorClock::VectorClock(){};

VectorClock::~VectorClock(){};

}  // namespace mongo
