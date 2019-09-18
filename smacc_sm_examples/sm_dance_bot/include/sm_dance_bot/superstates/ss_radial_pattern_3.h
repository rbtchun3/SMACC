
namespace SS3
{
//forward declaration for initial ssr
class SsrRadialRotate;
class SsrRadialReturn;

struct SsRadialPattern3 : smacc::SmaccState<SsRadialPattern3, SmDanceBot, SsrRadialRotate>
{
public:
    using SmaccState::SmaccState;

    typedef mpl::list<
                      // Expected event
                      sc::transition<EvStateFinish<SsRadialPattern3>, StRotateDegrees4>,

                      // Keyboard events
                      sc::transition<smacc::KeyPressEvent<'n'>, StRotateDegrees4>,
                      sc::transition<KeyPressEvent<'p'>,StNavigateToWaypointsX>,
                       
                      // Error events
                      sc::transition<smacc::EvTopicMessageTimeout<LidarSensor>, StAcquireSensors>,
                      sc::transition<EvActionAborted<smacc::SmaccMoveBaseActionClient::Result>, StNavigateToWaypointsX>,

                      // Internal events
                      sc::custom_reaction<smacc::EvStateFinish<SsrRadialReturn>>
            > reactions;

    int iteration_count;
    int total_iterations;

    void onInitialize()
    {
        iteration_count = 0;
        total_iterations =2;

        this->configure<KeyboardOrthogonal>(new SbKeyboard());
    }

    sc::result react(const smacc::EvStateFinish<SsrRadialReturn> &ev)
    {
        if (++iteration_count == total_iterations) // 1 == two times
        {
            this->throwFinishEvent();
        }
    }
};

//forward declaration for the superstate
using SS = SsRadialPattern3;
#include <sm_dance_bot/superstate_routines/ssr_radial_end_point.h>
#include <sm_dance_bot/superstate_routines/ssr_radial_return.h>
#include <sm_dance_bot/superstate_routines/ssr_radial_rotate.h>
} // namespace SS3