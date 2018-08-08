/*
   alienflightf3v1.cpp 

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

#include <f3board.h>
#include <SpektrumDSM.h>
#include <receiver.hpp>
#include <hackflight.hpp>
#include <mixers/quadx.hpp>

constexpr uint8_t CHANNEL_MAP[6] = {0, 1, 2, 3, 6, 4};

SpektrumDSM2048 * rx;

// Support for reading DSMX signals over UART2
static serialPort_t * serial2;
static uint8_t dsmAvailable;
static uint8_t dsmValue;

uint8_t dsmSerialAvailable(void)
{
    return dsmAvailable;
}

uint8_t dsmSerialRead(void)
{
    dsmAvailable--;
    return dsmValue;
}

static void serial_event_2(uint16_t value)
{
    dsmValue = (uint8_t)value;
    dsmAvailable = 1;

    rx->handleSerialEvent(micros());
}

class DSMX_Receiver : public hf::Receiver {

    public:

        DSMX_Receiver(const uint8_t channelMap[6], float trimRoll=.01, float trimPitch=0, float trimYaw=0) : 
            Receiver(channelMap, trimRoll, trimPitch, trimYaw) { }

    protected:

        virtual void begin(void) override 
        {
        }

        virtual bool gotNewFrame(void) override 
        {
            return true;
        }

        virtual void readRawvals(void)override 
        {
            if (rx->gotNewFrame()) {

                rx->getChannelValuesNormalized(rawvals, CHANNELS);
            }
        }
};

class AlienflightF3V1 : public F3Board {

    void writeMotor(uint8_t index, float value)
    {
        (void)index; // XXX
        (void)value;
    }

}; // class AlienflightF3V1

static hf::Hackflight h;

hf::MixerQuadX mixer;

void setup() {

    // Open connection to UART2
    serial2 = uartOpen(USART2, serial_event_2, 115200, MODE_RX, SERIAL_NOT_INVERTED);

    // Create an object for talking to the DSMX receiver.
    // We have to allocate such objects dynamicall to ensure that their constructor are called.
    rx = new SpektrumDSM2048();

    hf::Stabilizer * stabilizer = new hf::Stabilizer(
            0.20f,      // Level P
            0.225f,     // Gyro cyclic P
            0.001875f,  // Gyro cyclic I
            0.375f,     // Gyro cyclic D
            1.0625f,    // Gyro yaw P
            0.005625f); // Gyro yaw I

    DSMX_Receiver * rc = new DSMX_Receiver(
            CHANNEL_MAP,
            .005f,  // roll trim
            .01f,  // pitch trim
            0.f);   // yaw trim

    // Initialize Hackflight firmware
    h.init(new AlienflightF3V1(), rc, &mixer, stabilizer);
}

void loop() {

    h.update();

    hf::Debug::printf("%d\n", millis());
}

extern "C" {

#include "serial_usb_vcp.h"

    serialPort_t * serial0_open(void)
    {
        return usbVcpOpen();
    }

}
