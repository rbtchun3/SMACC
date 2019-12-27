#include <smacc/smacc.h>

namespace sm_atomic
{
using namespace ros_timer_client;

struct State1
    : smacc::SmaccState<State1, SmAtomic>
{
    typedef mpl::list<smacc::transition<EvTimer<CbTimerSingleCountdown, OrTimer>, State2>> reactions;

    using SmaccState::SmaccState;

    static void onDefinition()
    {
        static_configure<OrTimer, CbTimerRepeatCountdown>(3); // triggers each 3 client ticks
        static_configure<OrTimer, CbTimerSingleCountdown>(10); // triggers once at 10 client ticks
    }

    void onInitialize()
    {
        // get reference to the client
        ClRosTimer *client;
        this->requiresClient(client);

        // subscribe to the timer client callback
        client->onTimerTick(&State1::onTimerClientTickCallback, this);


        // getting reference to the repeat countdown behavior
        auto* cbrepeat = this->getStateMachine().getOrthogonal<OrTimer>()
                               ->getClientBehavior<CbTimerRepeatCountdown>();

        // subscribe to the repeat countdown behavior callback
        cbrepeat->onTimerTick(&State1::onRepeatBehaviorTickCallback, this);

        // getting reference to the single countdown behavior
        auto* cbsingle = this->getStateMachine().getOrthogonal<OrTimer>()
                               ->getClientBehavior<CbTimerSingleCountdown>();

        // subscribe to the single countdown behavior callback
        cbsingle->onTimerTick(&State1::onSingleBehaviorTickCallback, this);
    }

    void onTimerClientTickCallback()
    {
        ROS_INFO("timer client tick!");
    }

    void onRepeatBehaviorTickCallback()
    {
        ROS_INFO("repeat behavior tick!");
    }

    void onSingleBehaviorTickCallback()
    {
        ROS_INFO("single behavior tick!");
    }
};
} // namespace sm_atomic