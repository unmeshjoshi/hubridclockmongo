#pragma once

#include <array>

#include "logical_time.h"
#include <set>
#include <limits>
#include <memory>

namespace mongo {

/**
 * The VectorClock service provides a collection of cluster-wide logical clocks (including the
 * clusterTime), that are used to provide causal-consistency to various other services.
 */
class VectorClock {
protected:
    enum class Component : uint8_t {
        ClusterTime = 0,
        ConfigTime = 1,
        TopologyTime = 2,
        _kNumComponents = 3,
    };

    template <typename T>
    class ComponentArray
        : public std::array<T, static_cast<unsigned long>(Component::_kNumComponents)> {
    public:
        const T& operator[](Component component) const {
            return std::array<T, static_cast<unsigned long>(Component::_kNumComponents)>::
            operator[](static_cast<unsigned long>(component));
        }

        T& operator[](Component component) {
            return std::array<T, static_cast<unsigned long>(Component::_kNumComponents)>::
            operator[](static_cast<unsigned long>(component));
        }

    private:
        const T& operator[](unsigned long i) const;
        T& operator[](unsigned long i);
    };

    using LogicalTimeArray = ComponentArray<LogicalTime>;

    struct component_comparator {
        bool operator()(const Component& c0, const Component& c1) const {
            return static_cast<uint8_t>(c0) < static_cast<uint8_t>(c1);
        }
    };

    using ComponentSet = std::set<Component, component_comparator>;

public:
    class VectorTime {
    public:
        explicit VectorTime(LogicalTimeArray time) : _time(std::move(time)) {}
        VectorTime() = default;

        LogicalTime clusterTime() const& {
            return _time[Component::ClusterTime];
        }

        LogicalTime configTime() const& {
            return _time[Component::ConfigTime];
        }

        LogicalTime topologyTime() const& {
            return _time[Component::TopologyTime];
        }

//        LogicalTime clusterTime() const&& = delete;
//        LogicalTime configTime() const&& = delete;
//        LogicalTime topologyTime() const&& = delete;

        LogicalTime operator[](Component component) const {
            return _time[component];
        }

    private:
        friend class VectorClock;

        LogicalTimeArray _time;
    };

    static constexpr char kClusterTimeFieldName[] = "$clusterTime";
    static constexpr char kConfigTimeFieldName[] = "$configTime";
    static constexpr char kTopologyTimeFieldName[] = "$topologyTime";

    // Decorate ServiceContext with VectorClock* which points to the actual vector clock
    // implementation.

    /**
     * Returns an instantaneous snapshot of the current time of all components.
     */
    VectorTime getTime() const;

    /**
     * Returns true if the clock is enabled and can be used. Defaults to true.
     */
    bool isEnabled() const;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // The group of methods below is only used for unit-testing
    ///////////////////////////////////////////////////////////////////////////////////////////////

    void advanceClusterTime_forTest(LogicalTime newTime) {
        _advanceTime_forTest(Component::ClusterTime, newTime);
    }

    void advanceConfigTime_forTest(LogicalTime newTime) {
        _advanceTime_forTest(Component::ConfigTime, newTime);
    }

    void advanceTopologyTime_forTest(LogicalTime newTime) {
        _advanceTime_forTest(Component::TopologyTime, newTime);
    }

    void resetVectorClock_forTest();

protected:
    class ComponentFormat {
    public:
        ComponentFormat(std::string fieldName) : _fieldName(std::move(fieldName)) {}
        virtual ~ComponentFormat() = default;


        const std::string _fieldName;
    };

    VectorClock();
    virtual ~VectorClock();

    /**
     * The maximum permissible value for each part of a LogicalTime's Timestamp (ie. "secs" and
     * "inc").
     */
    static constexpr uint32_t kMaxValue = std::numeric_limits<int32_t>::max();

    /**
     * The "name" of the given component, for user-facing error messages. The name used is the
     * field name used when gossiping.
     */
    static std::string _componentName(Component component);

    /**
     * Disables the clock. A disabled clock won't process logical times and can't be re-enabled.
     */
    void _disable();


    /**
     * Used to ensure that gossiped or ticked times never overflow the maximum possible LogicalTime.
     */
    static bool _lessThanOrEqualToMaxPossibleTime(LogicalTime time, uint64_t nTicks);

    /**
     * As for _gossipInInternal, except from a client external to the cluster, eg. a driver or user
     * client. By default, just the ClusterTime is gossiped.
     */
    virtual ComponentSet _gossipInExternal() const {
        return ComponentSet{Component::ClusterTime};
    }


    /**
     * For each component in the LogicalTimeArray, sets the current time to newTime if the newTime >
     * current time and it passes the rate check.  If any component fails the rate check, then this
     * function uasserts on the first such component (without setting any current times).
     */
    void _advanceTime(LogicalTimeArray&& newTime);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // The group of methods below is only used for unit-testing
    ///////////////////////////////////////////////////////////////////////////////////////////////

    void _advanceTime_forTest(Component component, LogicalTime newTime);


    bool _isEnabled{true};

    LogicalTimeArray _vectorTime;

private:
    class PlainComponentFormat;
    class SignedComponentFormat;
    template <class ActualFormat>
    class OnlyOutOnNewFCVComponentFormat;
    static const ComponentArray<std::unique_ptr<ComponentFormat>> _gossipFormatters;

};

}  // namespace mongo
