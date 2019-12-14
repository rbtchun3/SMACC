#include <smacc/smacc.h>

#include <sensor_msgs/LaserScan.h>

//SUBSTATE BEHAVIORS
#include <ros_timer_client/sb_timer_substate.h>

#include <sm_dance_bot/substate_behaviors/navigation/sb_rotate.h>
#include <sm_dance_bot/substate_behaviors/navigation/sb_undo_path_backwards.h>
#include <sm_dance_bot/substate_behaviors/navigation/sb_navigate_global_position.h>
#include <sm_dance_bot/substate_behaviors/navigation/sb_navigate_forward.h>
#include <sm_dance_bot/substate_behaviors/navigation/sb_navigate_backward.h>

#include <sm_dance_bot/substate_behaviors/tool/sb_tool_start.h>
#include <sm_dance_bot/substate_behaviors/tool/sb_tool_stop.h>

#include <sm_dance_bot/substate_behaviors/lidar_sensor/sb_lidar_sensor.h>

#include <sm_dance_bot/substate_behaviors/temperature_sensor/sb_custom_condition_temperature_sensor.h>
#include <sm_dance_bot/substate_behaviors/publisher/sb_string_publisher.h>
#include <smacc_interface_components/substate_behaviors/sensor_substate.h>
#include <sm_dance_bot/substate_behaviors/service_client/service3_client.h>
#include <sm_dance_bot/substate_behaviors/service_client/service3_behavior.h>
#include <sm_dance_bot/substate_behaviors/updatable_publisher/sb_updatable_publisher.h>

//LOGIC UNITS
#include <all_events_go/lu_all_events_go.h>
#include <event_countdown/event_countdown.h>
#include <conditional/lu_conditional.h>

// ORTHOGONALS
#include <sm_dance_bot/orthogonals/navigation_orthogonal.h>
#include <sm_dance_bot/orthogonals/obstacle_perception_orthogonal.h>
#include <sm_dance_bot/orthogonals/tool_orthogonal.h>
#include <sm_dance_bot/orthogonals/sensor_orthogonal.h>
#include <sm_dance_bot/orthogonals/publisher_orthogonal.h>
#include <sm_dance_bot/orthogonals/service3_orthogonal.h>
#include <sm_dance_bot/orthogonals/timer_orthogonal.h>
#include <sm_dance_bot/orthogonals/updatable_publisher_orthogonal.h>

namespace sm_dance_bot
{
//STATE FORWARD DECLARATIONS
class StAcquireSensors;
class StEventCountDown;
class StRotateDegrees4;
class StNavigateForward1;
class StNavigateToWaypoint1;
class StNavigateToWaypointsX;
class StRotateDegrees2;
class StRotateDegrees1;
class StNavigateReverse2;
class StRotateDegrees3;
class StNavigateReverse1;
class StNavigateForward2;
class StRotateDegrees5;
class StNavigateReverse3;
class StRotateDegrees6;
class StNavigateReverse3;

//SUPERSTATE FORWARD DECLARATIONS

// MEGASTATE FORWARD DECLARATIONS
class MsDanceBotRunMode;
class MsDanceBotRecoveryMode;

namespace SS1
{
class SsRadialPattern1;
}

namespace SS2
{
class SsRadialPattern2;
}

namespace SS3
{
class SsRadialPattern3;
}

namespace SS4
{
class SsFPattern1;
}

namespace SS5
{
class SsSPattern1;
}

// custom smd_dance_bot event
struct EvGlobalError : sc::event<EvGlobalError>
{
};

} // namespace sm_dance_bot

using namespace sm_dance_bot;
using namespace ros_timer_client;
using namespace smacc;

namespace sm_dance_bot
{
/// \brief Advanced example of state machine with smacc that shows multiple techniques
///  for the development of state machines
struct SmDanceBot
    : public smacc::SmaccStateMachineBase<SmDanceBot, MsDanceBotRunMode>
{
    int counter_1;
    bool rt_ready_flag;

    typedef mpl::bool_<false> shallow_history;
    typedef mpl::bool_<false> deep_history;
    typedef mpl::bool_<false> inherited_deep_history;

    typedef smacc::SmaccStateMachineBase<SmDanceBot, MsDanceBotRunMode> base;

    using SmaccStateMachineBase::SmaccStateMachineBase;

    virtual void onInitialize() override
    {
        this->setGlobalSMData("counter_1", counter_1);
        this->setGlobalSMData("rt_ready_flag", rt_ready_flag);

        //this->createOrthogonal<MoveBaseActionClient, Robot1>();
        //this->createOrthogonal<MoveBaseActionClient, Robot2>();

        this->createOrthogonal<ObstaclePerceptionOrthogonal>();
        this->createOrthogonal<ToolOrthogonal>();
        this->createOrthogonal<SensorOrthogonal>();
        this->createOrthogonal<PublisherOrthogonal>();
        this->createOrthogonal<Service3Orthogonal>();
        this->createOrthogonal<TimerOrthogonal>();
        this->createOrthogonal<UpdatablePublisherOrthogonal>();
    }
};

} // namespace sm_dance_bot

//MEGASTATES
#include <sm_dance_bot/modestates/ms_dance_bot_run_mode.h>
#include <sm_dance_bot/modestates/ms_dance_bot_recovery_mode.h>

//SUPERSTATES
#include <sm_dance_bot/superstates/ss_radial_pattern_1.h>
#include <sm_dance_bot/superstates/ss_radial_pattern_2.h>
#include <sm_dance_bot/superstates/ss_radial_pattern_3.h>
#include <sm_dance_bot/superstates/ss_f_pattern_1.h>
#include <sm_dance_bot/superstates/ss_s_pattern_1.h>

//STATES
#include <sm_dance_bot/states/st_acquire_sensors.h>
#include <sm_dance_bot/states/st_event_count_down.h>

#include <sm_dance_bot/states/st_navigate_to_waypoints_x.h>

#include <sm_dance_bot/states/st_rotate_degrees_6.h>
#include <sm_dance_bot/states/st_rotate_degrees_5.h>
#include <sm_dance_bot/states/st_navigate_forward_2.h>
#include <sm_dance_bot/states/st_rotate_degrees_4.h>
#include <sm_dance_bot/states/st_navigate_forward_1.h>
#include <sm_dance_bot/states/st_navigate_to_waypoint_1.h>
#include <sm_dance_bot/states/st_rotate_degrees_2.h>
#include <sm_dance_bot/states/st_rotate_degrees_1.h>
#include <sm_dance_bot/states/st_navigate_reverse_2.h>
#include <sm_dance_bot/states/st_rotate_degrees_3.h>
#include <sm_dance_bot/states/st_navigate_reverse_1.h>
#include <sm_dance_bot/states/st_navigate_reverse_3.h>
