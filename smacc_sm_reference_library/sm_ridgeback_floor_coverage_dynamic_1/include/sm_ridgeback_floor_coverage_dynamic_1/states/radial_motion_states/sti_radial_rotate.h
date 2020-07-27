namespace sm_ridgeback_floor_coverage_dynamic_1
{
namespace radial_motion_states
{
// STATE DECLARATION
struct StiRadialRotate : smacc::SmaccState<StiRadialRotate, SS>
{
  using SmaccState::SmaccState;

// TRANSITION TABLE
  typedef mpl::list<
  
  Transition<EvActionSucceeded<ClMoveBaseZ, OrNavigation>, StiRadialEndPoint, SUCCESS>,
  Transition<EvActionAborted<ClMoveBaseZ, OrNavigation>, StiRadialLoopStart, ABORT>
  
  >reactions;

// STATE FUNCTIONS
  static void staticConfigure()
  {
    configure_orthogonal<OrNavigation, CbAbsoluteRotate>();
    configure_orthogonal<OrLED, CbLEDOff>();
  }

  void runtimeConfigure()
  {
    auto cbAbsRotate = this->getOrthogonal<OrNavigation>()
                           ->getClientBehavior<CbAbsoluteRotate>();

    auto &superstate = this->context<SS>();
    cbAbsRotate->absoluteGoalAngleDegree = superstate.iteration_count * SS::ray_angle_increment_degree();
  }
};
} // namespace radial_motion_states
} // namespace sm_ridgeback_floor_coverage_dynamic_1