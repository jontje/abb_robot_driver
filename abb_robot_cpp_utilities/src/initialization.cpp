/***********************************************************************************************************************
 *
 * Copyright (c) 2020, ABB Schweiz AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that
 * the following conditions are met:
 *
 *    * Redistributions of source code must retain the
 *      above copyright notice, this list of conditions
 *      and the following disclaimer.
 *    * Redistributions in binary form must reproduce the
 *      above copyright notice, this list of conditions
 *      and the following disclaimer in the documentation
 *      and/or other materials provided with the
 *      distribution.
 *    * Neither the name of ABB nor the names of its
 *      contributors may be used to endorse or promote
 *      products derived from this software without
 *      specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***********************************************************************************************************************
 */

#include <stdexcept>

#include <ros/ros.h>

#include "abb_robot_cpp_utilities/initialization.h"

namespace
{
/**
 * \brief Name for ROS logging in the 'init' context.
 */
constexpr char ROS_LOG_INIT[]{"init"};

/**
 * \brief Max number of attempts when trying to connect to a robot controller via RWS.
 */
constexpr unsigned int RWS_MAX_CONNECTION_ATTEMPTS{5};

/**
 * \brief Error message for failed connection attempts when trying to connect to a robot controller via RWS.
 */
constexpr char RWS_CONNECTION_ERROR_MESSAGE[]{"Failed to establish RWS connection to the robot controller"};

/**
 * \brief Time [s] to wait before trying to reconnect to a robot controller via RWS.
 */
constexpr double RWS_RECONNECTION_WAIT_TIME{1.0};
}

namespace abb
{
namespace robot
{
namespace utilities
{

/***********************************************************************************************************************
 * Utility function definitions
 */

RobotControllerDescription establishRWSConnection(RWSManager& rws_manager,
                                                  const std::string& robot_controller_id,
                                                  const bool no_connection_timeout)
{
  ros::Duration reconnection_wait_time{RWS_RECONNECTION_WAIT_TIME};

  unsigned int attempt{0};

  while(ros::ok() && (no_connection_timeout || attempt++ < RWS_MAX_CONNECTION_ATTEMPTS))
  {
    try
    {
      return rws_manager.collectAndParseSystemData(robot_controller_id);
    }
    catch(const std::runtime_error& exception)
    {
      if(!no_connection_timeout)
      {
        ROS_WARN_STREAM_NAMED(ROS_LOG_INIT, RWS_CONNECTION_ERROR_MESSAGE << " (attempt " << attempt << "/" <<
                                            RWS_MAX_CONNECTION_ATTEMPTS << "), reason: '" << exception.what() << "'");
      }
      else
      {
        ROS_WARN_STREAM_NAMED(ROS_LOG_INIT, RWS_CONNECTION_ERROR_MESSAGE << " (waiting indefinitely), reason: '" <<
                                            exception.what() << "'");
      }
      reconnection_wait_time.sleep();
    }
  }

  throw std::runtime_error{RWS_CONNECTION_ERROR_MESSAGE};
}

}
}
}
