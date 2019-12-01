//===========================================================================//
/// @file
/// @brief    Robot navigation, sensor, and control.
/// @author   Nathan Lucas
/// @date     2018
//===========================================================================//
#ifndef MOC_ROBOT_HPP
#define MOC_ROBOT_HPP

#include "robot/confidence.hpp"   // moc::Confidence
#include "robot/motion.hpp"       // moc::Motion
#include "robot/robot_id.hpp"     // moc::RobotID
#include "robot/robot_log.hpp"    // moc::RobotLog
#include "robot/robot_state.hpp"  // moc::RobotState
#include "robot/sensor.hpp"       // moc::Sensor
#include "nav/breadcrumbs.hpp"    // moc::Breadcrumbs
#include "nav/obstacles.hpp"      // moc::Obstacles
#include "nav/path.hpp"           // moc::Path
#include "config/config.hpp"      // moc::Configuration
#include "core/target.hpp"        // moc::Targets
#include "ui/alert_cue.hpp"       // moc::AnimatedCue, moc::FlashingCue
#include "ui/mouse.hpp"           // moc::MouseClickDrag

#include <utl/asio/tcp/connection.hpp>    // utl::io::tcp

#include <array>      // std::array
#include <cstdint>    // std::uint_fast32_t
#include <mutex>      // std::mutex
#include <ostream>    // std::ostream
#include <string>     // std::string
#include <thread>     // std::thread
#include <vector>     // std::vector

//-----------------------------------------------------------
// Build features and debugging

#if 0
# define DRAW_ANIMATED_ALERT
#elif 0
# define DRAW_ARROW_ALERT
#else
# define DRAW_FLASHING_ALERT
#endif

#if 0
// Define for testing and debugging without TCP connections to robots
#define DEBUG_ROBOT_TCP_OVERRIDE
#endif

#if 0
// Left click moving robot to pause motion
#define ROBOT_LEFT_CLICK_PAUSE
#endif

#if 0
#define ROTATION_FILTER_LOWPASS
#endif

#if 0
#define ROTATION_FILTER_MEDIAN
#endif
//-----------------------------------------------------------

namespace moc {
/// @addtogroup moc_robot
/// @{
//---------------------------------------------------------------------------

/// @brief  %Robot navigation, sensor, task data, and control input.
///
/// - TCP communication with a physical (or virtualized) robot.
/// - Processes user input and localization data from external sources.
/// - Encapsulates navigation and task data.
/// - Draws graphics on externally supplied video frames.
class Robot
{
public:
  //-----------------------------------------------------------

  /// TCP connection pointer.
  using tcp_ptr = utl::io::tcp::connection_ptr;

  //-----------------------------------------------------------

  /// @brief  Construct robot.
  /// @param  [in]  id    Unique ID number (must be `> 0`).
  /// @param  [in]  cfg   Configuration settings.
  /// @param  [in]  pt    Starting position.
  /// @param  [in]  rot   Starting rotation in radians.
  /// @param  [in]  obs   Solid obstacles that
  ///                     block sensor line-of-sight.
  /// @param  [in]  excl  Exclusion zones that provide a buffer around
  ///                     the obstacles.  Navigation waypoints are not
  ///                     permitted with these zones.
  ///
  Robot(unsigned id, Configuration const& cfg, cv::Point const& pt,
        double rot, Obstacles const& obs, Obstacles const& excl);

  /// Destructor
  ~Robot();

  void      connect_tcp(tcp_ptr tcp);     ///< Sets TCP connection.
  bool      connected() const;            ///< Returns TCP connection status.
  //unsigned  id() const;                 ///< Returns robot ID.
  void      targets(Targets const& ts);   ///< Sets target points.

  /// Returns the ID of the detected target, with `1` being
  /// the first target, or `0` if no target is detected.
  ///
  /// @note Sensor can only detect one target at a time.
  std::size_t
  target_detected() const;

  //-----------------------------------------------------------
  /// @name Path and Motion
  /// @{

  /// Returns a const reference to robot position breadcrumbs.
  Breadcrumbs const&
  breadcrumbs() const;

  /// Returns a reference to robot position breadcrumbs.
  Breadcrumbs&
  breadcrumbs();

  /// `true` if a collision occurred or is imminent.
  void
  collision(bool val);

  /// Returns `true` if robot is in motion.
  bool
  is_moving() const;

  /// Returns navigation path vertices.
  moc::Vertices
  path_vertices() const;

  ///// Pause/resume robot motion.
  //void
  //pause(bool val);

  /// Returns robot position and orientation.
  moc::Vertex
  pose() const;

  /// @brief Set robot position and orientation.
  ///
  /// If pose is not updated periodically, it is assumed that robot tracking
  /// is lost, and robot motion is paused until pose is updated again.
  ///
  /// @param  [in]  cxy   Center point in pixels.
  /// @param  [in]  rot   Yaw rotation in radians.
  //  @param  [in]  pts   Corner coordinates.
  void
  pose(cv::Point const& cxy, double rot);
  //, std::array<cv::Point, 4> const& pts);

  /// @}
  //-----------------------------------------------------------
  /// @name Input/Output
  /// @{

  /// Returns the current confidence value.
  std::uint_fast32_t
  confidence() const;

  /// Sets confidence value and corresponding confidence level.
  void
  confidence(std::uint_fast32_t val);

  /// Draws robot and path graphics.
  void
  draw(cv::Mat& image) const;

 #ifdef DRAW_ARROW_ALERT
  /// Sets eye gaze point.
  void
  gaze(cv::Point const& pt);
 #endif

  /// Checks for and process keyboard input.
  void
  handle_key(int code);

  /// Checks for and process mouse input.
  void
  handle_mouse(int event, int x, int y, int flags);

  bool  is_activated() const;     ///< Returns `true` if accepting input.
  void  is_activated(bool act);   ///< `true` to accept and process input.

  /// Returns accumulated state, target detection, alert, and path
  /// distance data serialized in comma separated value (CSV) format.
  std::string
  metrics_csv() const;

  /// Returns the current input/motion state.
  moc::RobotState
  state() const;

  /// @}

private:
  //-----------------------------------------------------------
  // Private member functions

  void reset_path();                    // Stop motion & clear path vertices
  void state_change(moc::RobotState const& rs);   // Update robot state
  void do_pause(bool val);              // Pause/resume robot motion
  void drive();                         // Plan motion & send motor command
  void shutdown();                      // Stop robot & send shutdown command

  // The robot pose timer trackers whether the robot is receiving
  // periodic pose updates.  Lack of updated pose after a duration
  // of 1000 milliseconds indicates robot pose tracking is lost.
  // The timer responds to lost tracking by pausing robot motion.
  void start_timer();

  //-----------------------------------------------------------
  // Type alias

 #ifdef DRAW_ANIMATED_ALERT
  using AlertCue = moc::AnimatedCue;    // Animated alert graphic
 #elif defined DRAW_ARROW_ALERT
  using AlertCue = moc::ArrowCue;       // Arrow cue graphic
 #else
  using AlertCue = moc::FlashingCue;    // Flashing alert graphic
 #endif

  //-----------------------------------------------------------
  // Private member variables

  mutable std::mutex        mutex_{};             // Mutual exclusion

  moc::RobotID              id_;                  // Unique identification
  bool                      is_tracked_{false};   // `true` if pose is tracked
  moc::RobotLog             log_;                 // Data logging
  moc::MouseClickDrag       left_click_{};        // Left mouse click and drag
  moc::Motion               motion_;              // Gross, fine motor control
  moc::MotorHighMedLow      motor_speeds_;        // High, medium, low speeds
  moc::Path                 path_;                // Navigation path
 #ifdef ROTATION_FILTER_MEDIAN
  // Rotation filtering
  static constexpr unsigned           rot_buff_size_{3};  // (!) Must be odd
  std::array<double, rot_buff_size_>  rot_buff_{};        // Moving window
  std::size_t                         rot_index_{0};      // Index into buffer
 #endif
  moc::Sensor         sensor_;                // Sensor computations/graphics
  moc::RobotState     state_{RobotState::deactivated};  // Input/motion state
  tcp_ptr             tcp_{nullptr};          // TCP connection
  std::thread         timeout_thread_{};      // Dedicated timer thread
  bool                timer_{true};           // `true` for timeout timer
};
//---------------------------------------------------------------------------

constexpr unsigned  robot_count = 4;      ///< Number of robots.

using Robots     = std::array<Robot, robot_count>;    ///< Array of robots.
using RobotFlags = std::array<bool, robot_count>;     ///< Boolean flags.

//---------------------------------------------------------------------------
/// @}
} // moc

#endif // MOC_ROBOT_HPP
//===========================================================================//
