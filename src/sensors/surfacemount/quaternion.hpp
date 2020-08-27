/*
   Support for treating quaternion as a sensor
   
   Supports IMUs like EM7180 SENtral Sensor Fusion solution, where 
   quaternion is computed in hardware, and simulation platforms like
   UE4, where quaternion is provided by physics engine. For other IMUs 
   and simulators, you can use quaternion-filter classes in filters.hpp.

   Copyright (c) 2018 Simon D. Levy

   This file is part of Hackflight.

   Hackflight is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Hackflight is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with Hackflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <math.h>

#include "sensors/surfacemount.hpp"

namespace hf {

    class Quaternion : public SurfaceMountSensor {

        friend class Hackflight;

        private:

            float _w = 0;
            float _x = 0;
            float _y = 0;
            float _z = 0;

        protected:

            Quaternion(void)
            {
                _w = 0;
                _x = 0;
                _y = 0;
                _z = 0;
            }

            virtual void modifyState(state_t & state, float time) override
            {
                (void)time;

                imu->computeEulerAngles(_w, _x, _y, _z, state.rotation);

                // Convert heading from [-pi,+pi] to [0,2*pi]
                if (state.rotation[2] < 0) {
                    state.rotation[2] += 2*M_PI;
                }
            }

            virtual bool ready(float time) override
            {
                return imu->getQuaternion(_w, _x, _y, _z, time);
            }


            // We make this public so we can use it in different sketches
            static void computeEulerAngles(float qw, float qx, float qy, float qz, float euler[3])
            {
	    }

    };  // class Quaternion

} // namespace hf
