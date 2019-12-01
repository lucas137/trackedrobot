//===========================================================================//
//  Nathan Lucas
//  2018
//===========================================================================//

#include "robot/robot.hpp"

#include "core/define.hpp"        // common macros, moc::fixation_radius
#include "core/sum_distance.hpp"  // moc::SumDistance
#include "core/time.hpp"          // moc::steady_clock_ms
#include "ui/color.hpp"           // moc::color

#include <utl/math.hpp>           // utl::math::pi

#include <algorithm>    // std::copy, std::nth_element
#include <cmath>        // std::abs, std::sqrt
#include <mutex>        // std::lock_guard, std::unique_lock
#include <exception>    // std::exception, std::runtime_error
#include <iostream>     // std::cout
#include <string>       // std::to_string
#include <sstream>      // std::ostringstream

#ifdef ROTATION_FILTER_LOWPASS
# include <iomanip>     // std::setprecision
#endif

//-----------------------------------------------------------

#if 0
#define DRAW_ATTENTION_RADIUS
#endif
#if 0
#define DRAW_ROBOT_CLICK_REGION
#endif

#if 0
#define OUTPUT_ROBOT_CONFIDENCE
#endif
#if 0
#define OUTPUT_ROBOT_DEBUG
#endif

namespace {   //-------------------------------------------------------------

constexpr int       background_radius = 30;
constexpr int       click_radius      = 50;   // Click threshold in pixels
constexpr int       click_radius_sq   = (click_radius * click_radius);
constexpr int       countdown_radius  = 24;   // Confidence countdown radius
constexpr int       hover_thick       = 1;    // Hover-over line thickness
constexpr unsigned  timeout_ms        = 1000; // Watchdog timer period

//-----------------------------------------------------------

#ifdef ROTATION_FILTER_LOWPASS
// Rotation low-pass filter value in radians
//  degrees:    45    40    36    33.3  30    25    22.5  20
//  divisor:    4.0   4.5   5.0   5.4   6.0   7.2   8.0   9.0
constexpr double  rotation_lowpass_rad = utl::math::pi / 5.4;

// Displacement in pixels for resetting rotation low-pass filter
constexpr int     rotation_lowpass_px = 100;

bool
opposite_signs(double x, double y)
{
  return (x < 0) ? (y >= 0) : (y < 0);
}

//// Compute the mean.
//template <std::size_t Size>
//double
//mean(std::array<double, Size> const& vals, double nominal)
//{
//  double sum = 0;
//  for (auto const& v : vals)
//  {
//    // Subtract or add to accumulate value according to its sign
//    if (opposite_signs(v, nominal)) { sum -= v; } else { sum += v; }
//  }
//  return sum / 5;
//}

inline bool
large_angle_change(double a, double b, double threshold)
{
  double delta = (opposite_signs(a, b) ? std::abs(a + b)
                                       : std::abs(a - b));
  if (delta > threshold)
  {
    std::cout << std::setprecision(1) << std::fixed << "delta = "
              << utl::math::rad_to_deg<double>(delta) << " deg : ";
    return true;
  }
  return false;
}
#endif

#ifdef ROTATION_FILTER_MEDIAN
// Compute the median of an odd number of values.
template <std::size_t Size>
double
median_odd(std::array<double, Size> const& vals)
{
  auto v = vals;
  std::nth_element(v.begin(), v.begin() + (Size / 2), v.end());
  return v[Size / 2];
}
#endif

} // anonymous --------------------------------------------------------------

namespace moc {
//---------------------------------------------------------------------------

Robot::Robot(unsigned id, Configuration const& cfg)
: alert_cue_(cfg.robot.alert)
, breadcrumbs_(cfg, id)
, center_(pt)
, click_region_(pt, click_radius)
, confidence_(cfg.robot.confidence)
, id_(id)
, log_(id, cfg)
, motion_(id, cfg.robot.motor.high, excl)
, motor_speeds_(cfg.robot.motor)
, path_(pt, rot, cfg.robot.min_path + 1, excl)
, sensor_(cfg.robot.sensor_deg, cfg.robot.sensor_px, pt, rot, obs)
{
  // Start recording duration of the current state
  log_.state_duration.deactivated.start();

  // Start pose update watchdog timer
  start_timer();
}

/*
  We chose to stop the timeout timer and swallow any exceptions in
  the destructor.  This ensures proper unwinding without relying on
  the user to call a separate member function to stop the timer.
  Although a separate function would allow us to pass exceptions for
  the caller to handle, any errors would be contained to this object
  and there isn't much the caller can do about them anyway.
*/
Robot::~Robot()
{
  //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  // Create scope within which to acquire lock on mutex_
  {
    std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
    timer_ = false;                             // Shut down timer loop
  } // Release lock upon leaving scope
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  // Synchronize threads
  // Note: We avoiding locking mutex_ because the lambda in start_timer()
  // needs a lock on mutex_ in order for timeout_thread_ to finish.
  try
  {
    if (timeout_thread_.joinable())
    {
      timeout_thread_.join();       // Wait for thread to finish
    }
  }
  catch (std::exception& e)     // Catch and report standard exceptions
  {
    std::cout <<'\n'<< __FILE__<<':'<<__LINE__    // source and line number
              <<'\n'<< "ERROR: exception:" <<'\n'<< e.what() <<'\n'<<'\n';
  }
  catch (...)                   // Catch all other exceptions
  {
    std::cout <<'\n'<< __FILE__<<':'<<__LINE__    // source and line number
              <<'\n'<< "ERROR: unknown exception" <<'\n'<<'\n';
  }
}


void
Robot::connect_tcp(tcp_ptr tcp)
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  tcp_ = tcp;
  if (tcp_ != nullptr)
  {
    if (tcp_->is_open())
    {
      do_pause(false);  // Resume motion if paused due to no TCP communication
    }
  }
}

bool
Robot::connected() const
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  if (tcp_ == nullptr) { return false; }
  return (tcp_->is_open());
}

//unsigned
//Robot::id() const
//{
//  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
//  return id_.num();
//}

void
Robot::targets(Targets const& ts)
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  sensor_.targets(ts);
}

std::size_t
Robot::target_detected() const
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  return sensor_.target_detected();
}


// Path and Motion ------------------------------------------

Breadcrumbs const&
Robot::breadcrumbs() const
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  return breadcrumbs_;
}



void
Robot::collision(bool val)
{
  std::lock_guard<std::mutex> lock(mutex_);           // Acquire scoped lock
  if (state_ == RobotState::deactivated) { return; }  // Ignore input
  using S = RobotState;
  if (val && (state_ == S::path_motion)) { state_change(S::collision); } else
  if (!val && (state_ == S::collision)) { state_change(S::path_motion); }
  else
  {
    return; // Do nothing
  }
  drive();
}

bool
Robot::is_moving() const
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  return !motion_.stopped();
}

Vertices
Robot::path_vertices() const
{
  std::lock_guard<std::mutex> lock(mutex_);   // Acquire scoped lock
  return path_.vertices();
}

//void
//Robot::pause(bool val)
//{
//  std::lock_guard<std::mutex> lock(mutex_);           // Acquire scoped lock
//  if (state_ == RobotState::deactivated) { return; }  // Ignore input
//  do_pause(val);
//}




// Input/Output ---------------------------------------------

void
Robot::handle_key(int code)
{
  std::lock_guard<std::mutex> lock(mutex_);           // Acquire scoped lock
  if (state_ == RobotState::deactivated) { return; }  // Ignore input

  // Check for number key matching robot ID
  if (code == (0x30 + static_cast<int>(id_.num())))
  {
    do_pause(state_ == RobotState::path_motion);    // Pause if in motion
  }
  // Alert cue override
  else if ((code & 0x7F) == 'a')
  {
    alert_cue_.toggle_active();
    std::cout << "alert cue " << (alert_cue_.is_active() ? "on":"off") <<'\n';
  }
  //else if ((code & 0x7F) == '+') {}  // 0x2B
  //else if ((code & 0x7F) == '-') {}  // 0x2D
  else
  {
    return;   // No relevant key input
  }
  log_.event.write(state_, "key_press", center_.x, center_.y, code);
}

void
Robot::handle_mouse(int event, int x, int y, int flags)
{
  // Note: Double-clicks trigger three events for the respective button:
  // BUTTONUP, BUTTONDBLCLK, and another BUTTONUP. If we don't care how
  // fast the sequential clicks occur, we can look for two consecutive
  // BUTTONUP events near each other instead of a single BUTTONDBLCLK.

  std::lock_guard<std::mutex> lock(mutex_);       // Acquire scoped lock
  switch (event)                                  // Process mouse event
  {
    case cv::EVENT_LBUTTONDOWN: mouse_left_press(x, y, flags);      break;
    case cv::EVENT_LBUTTONUP:   mouse_left_release(x, y, flags);    break;
    case cv::EVENT_RBUTTONUP:   mouse_right_release(x, y, flags);   break;
    case cv::EVENT_MOUSEMOVE:   mouse_move(x, y, flags);            break;
    default:  break;
  }
}

moc::RobotState
Robot::state() const
{
  std::lock_guard<std::mutex> lock(mutex_);       // Acquire scoped lock
  return state_;
}


//---------------------------------------------------------------------------
// private

void
Robot::state_change(moc::RobotState const& rs)
{
  // Ignore if requested state is already the current state
  if (rs == state_) { return; }

  // Current steady clock time point in milliseconds
  auto now_ms = moc::steady_clock_ms();

  log_.state_duration.stop(state_, now_ms);   // Stop old state duration
  state_ = rs;                                // Set new robot state
  switch (state_)                             // Additional processing
  {
    case moc::RobotState::deactivated:
      alert_cue_.is_active(false);
    case moc::RobotState::drag_motion:
      reset_path();
      break;
    case moc::RobotState::path_input:
      reset_path();
      path_.input_mode(true);
      break;
    case moc::RobotState::wait_idle:
      if (sensor_.target_detected())
      {
        state_ = moc::RobotState::wait_target;
      }
    case moc::RobotState::wait_target:
      reset_path();
      break;
    case moc::RobotState::collision:
    case moc::RobotState::path_motion:
    case moc::RobotState::path_pause:
    default:
      break;
  }
  log_.state_duration.start(state_, now_ms);  // Start new state duration
  log_.event.write(state_, "state_change", center_.x, center_.y);
 #ifdef OUTPUT_ROBOT_DEBUG
  std::cout << id_.str() << ": mode = " << state_ <<'\n';
 #endif
}

void
Robot::do_pause(bool val)
{
  using S = RobotState;
  if (val && (state_ == S::path_motion)) { state_change(S::path_pause); } else
  if (!val && (state_ == S::path_pause)) { state_change(S::path_motion); }
  else { return; }  // Do nothing
  drive();
}

void
Robot::drive()
{
  if (!is_tracked_)
  {
    motion_.stop();   // Stop motion if we don't know robot location
  }
  else
  {
    switch (state_)
    {
      case RobotState::drag_motion:
        // Mouse drag motion parameters computed within pose()
        break;
      case RobotState::path_motion:
        motion_.path_motion(path_.vertices());
        break;
      case RobotState::collision:
      case RobotState::deactivated:
      case RobotState::path_input:
      case RobotState::path_pause:
      case RobotState::wait_idle:
      case RobotState::wait_target:
      default:
        motion_.stop();
        break;
    }
  }
 #ifdef OUTPUT_ROBOT_DEBUG
  std::cout << id_.str() << ": \"" << motion_.left_motor() << ","
                                   << motion_.right_motor() << "\"\n";
 #endif
 #ifndef DEBUG_ROBOT_TCP_OVERRIDE
  if ((tcp_ == nullptr) || (!tcp_->is_open()))
  {
    // No TCP connection
    if (state_ == RobotState::path_motion)
    {
      state_change(RobotState::path_pause);
    }
    return;
  }
  // Send motor command.
  // Do not use a public method (e.g., motor()), because it will attempt to
  // acquire a lock on mutex_, which should already be locked at this point.
  tcp_->write(std::to_string(motion_.left_motor()) + "," +
              std::to_string(motion_.right_motor()) + "\n");
 #endif
}

void
Robot::shutdown()
{
  log_.event.write(state_, "shutdown", center_.x, center_.y);
  if ((tcp_ != nullptr) && tcp_->is_open())
  {
    motion_.stop();
    tcp_->write(std::to_string(motion_.left_motor()) + "," +
                std::to_string(motion_.right_motor()) + "\n");
    state_change(RobotState::deactivated);
    tcp_->write("shutdown\n");
  }
  else
  {
    log_.event.write(state_, "error", center_.x, center_.y);
    std::cout << "ERROR: " << id_.str() << " not connected" << '\n';
  }
  std::cout << id_.str() << ": shutdown" <<'\n';
}

/*
  The robot pose timer trackers whether the robot is receiving periodic pose
  updates.  start_timer spawns a new thread for the timer loop.  The loop
  resets the robot object's is_tracked_ flag before the thread goes to sleep
  for the specified duration, timeout_ms, in milliseconds.  The flag is reset
  when the robot receives updated pose via member function pose().  Lack of
  updated pose after timeout_ms indicates robot tracking is lost.  The timer
  loop responds to lost tracking by issuing a command to stop robot motion.
*/
void
Robot::start_timer()
{
  timeout_thread_ = std::thread(
      // Lambda to periodically check for timeout
      [this]
      {
        // Local loop flag
        bool loop = true;
        //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        // Acquire lock on mutex that can be locked and unlocked
        std::unique_lock<std::mutex> lock(mutex_);
        while (loop)
        {
          // Check for timeout condition, which indicates robot pose
          // is not being updated (i.e., robot tracking is lost)
          if (!is_tracked_)
          {
            drive();    // Stop robot
            log_.event.write(state_, "tracking_lost", center_.x, center_.y);
            std::cout << (id_.str() +": tracking lost\n");
          }
          else if (state_ != RobotState::deactivated)
          {
              breadcrumbs_.push_back(center_);
          }
          // Reset robot pose tracking flag
          is_tracked_ = false;
          // Release lock on mutex while thread is asleep
          lock.unlock();
          //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

          // Wait for robot pose to be updated and allow time to process input
          std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

          //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
          // Re-acquire lock on mutex before accessing member variables
          lock.lock();
          loop = timer_;
        }
      });
}

//---------------------------------------------------------------------------
} // moc
//===========================================================================//
