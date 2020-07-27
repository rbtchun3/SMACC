namespace sm_ridgeback_floor_coverage_dynamic_1
{
namespace s_pattern_states
{
// STATE DECLARATION
struct StiSPatternRotate1 : smacc::SmaccState<StiSPatternRotate1, SS>
{
    using SmaccState::SmaccState;

    // TRANSITION TABLE
    typedef mpl::list<

        Transition<EvActionSucceeded<ClMoveBaseZ, OrNavigation>, StiSPatternForward1>,
        Transition<EvActionAborted<ClMoveBaseZ, OrNavigation>, StiSPatternLoopStart>

        >
        reactions;

    // STATE FUNCTIONS
    static void staticConfigure()
    {
        configure_orthogonal<OrNavigation, CbAbsoluteRotate>();
        configure_orthogonal<OrLED, CbLEDOff>();
    }

    void runtimeConfigure()
    {
        auto &superstate = this->context<SS>();
        ROS_INFO("[StiSPatternRotate] SpatternRotate rotate: SS current iteration: %d/%d", superstate.iteration_count, SS::total_iterations());

        double offset;
        // if (superstate.iteration_count == 1)
        // {
            offset = 0;
        // }
        // else
        // {
        //     offset = 13.5;
        // }

        auto absoluteRotateBehavior = this->getOrthogonal<OrNavigation>()
                                          ->getClientBehavior<CbAbsoluteRotate>();

        if (superstate.direction() == TDirection::RIGHT)
        {
            // - offset because we are looking to the north and we have to turn clockwise
            absoluteRotateBehavior->absoluteGoalAngleDegree = superstate.initialStateAngle - offset;
        }
        else
        {
            // - offset because we are looking to the south and we have to turn counter-clockwise
            absoluteRotateBehavior->absoluteGoalAngleDegree = superstate.initialStateAngle + offset;
        }
    }
};
} // namespace s_pattern_states
} // namespace sm_ridgeback_floor_coverage_dynamic_1