#pragma once
namespace sm_moveit_4
{
namespace place_states
{
// STATE DECLARATION
struct StPlaceRetreat : smacc::SmaccState<StPlaceRetreat, SS>
{
    using SmaccState::SmaccState;

    // TRANSITION TABLE
    typedef mpl::list<

        Transition<MoveGroupMotionExecutionSucceded<ClMoveGroup, OrArm>, StNavigationPosture, SUCCESS>,
        Transition<MoveGroupMotionExecutionFailed<ClMoveGroup, OrArm>, StPlaceRetreat, ABORT> /*retry on failure*/
        >
        reactions;

    // STATE FUNCTIONS
    static void staticConfigure()
    {
        configure_orthogonal<OrArm, CbMoveCartesianRelative>();
    }

    void runtimeConfigure()
    {
        auto moveCartesianRelative = this->getOrthogonal<OrArm>()
                                         ->getClientBehavior<CbMoveCartesianRelative>();

        moveCartesianRelative->offset_.z = 0.15;
    }

    void onExit()
    {
        ClPerceptionSystem *perceptionSystem;
        this->requiresClient(perceptionSystem);

        perceptionSystem->setSafeArmMotionToAvoidCubeCollisions();
    }
};
} // namespace place_states
} // namespace sm_moveit_4