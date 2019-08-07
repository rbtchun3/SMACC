#pragma once

#include <keyboard_state_machine.h>
#include <orthogonals/input_device_orthogonal.h>
#include <substates_behaviors/input_device/keyboard_substate.h>

//--------------------------------------------
struct KeyboardState: smacc::SmaccState<KeyboardState, KeyboardStateMachine>
{
  typedef sc::transition<smacc::KeyPressEvent<'k'>, KeyboardState> reactions; 

public:
  using SmaccState::SmaccState;

  void onInitialize()
  {
    this->configure<InputDeviceOrthogonal>(new smacc::Keyboard());
  }

  void onEntry()
  {
    ROS_INFO("pressed key 'k'");
  }

  void onExit()
  {
  }
};
