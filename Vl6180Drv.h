/**
 * \file Vl6180Drv.h
 *
 *  Created by Scott Erholm on 07/27/2017.
 *  Copyright (c) 2017 Agilatech. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __Vl6180Drv__
#define __Vl6180Drv__

#include <iostream>
#include <fstream>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "I2CDevice.h"
#include "Device.h"
#include "DataManip.h"

#define VL6180_DEFAULT_I2C_ADDR 0x29

#define VL6180_ALS_GAIN_1         0x06
#define VL6180_ALS_GAIN_1_25      0x05
#define VL6180_ALS_GAIN_1_67      0x04
#define VL6180_ALS_GAIN_2_5       0x03
#define VL6180_ALS_GAIN_5         0x02
#define VL6180_ALS_GAIN_10        0x01
#define VL6180_ALS_GAIN_20        0x00
#define VL6180_ALS_GAIN_40        0x07

#define VL6180_ERROR_NONE         0
#define VL6180_ERROR_SYSERR_1     1
#define VL6180_ERROR_SYSERR_5     5
#define VL6180_ERROR_ECEFAIL      6
#define VL6180_ERROR_NOCONVERGE   7
#define VL6180_ERROR_RANGEIGNORE  8
#define VL6180_ERROR_SNR          11
#define VL6180_ERROR_RAWUFLOW     12
#define VL6180_ERROR_RAWOFLOW     13
#define VL6180_ERROR_RANGEUFLOW   14
#define VL6180_ERROR_RANGEOFLOW   15



#define VL6180_IDENTIFICATION_MODEL_ID              0x0000
#define VL6180_IDENTIFICATION_MODEL_REV_MAJOR       0x0001
#define VL6180_IDENTIFICATION_MODEL_REV_MINOR       0x0002
#define VL6180_IDENTIFICATION_MODULE_REV_MAJOR      0x0003
#define VL6180_IDENTIFICATION_MODULE_REV_MINOR      0x0004
#define VL6180_IDENTIFICATION_DATE                  0x0006 //16bit value
#define VL6180_IDENTIFICATION_TIME                  0x0008 //16bit value

#define VL6180_SYSTEM_MODE_GPIO0                    0x0010
#define VL6180_SYSTEM_MODE_GPIO1                    0x0011
#define VL6180_SYSTEM_HISTORY_CTRL                  0x0012
#define VL6180_SYSTEM_INTERRUPT_CONFIG_GPIO         0x0014
#define VL6180_SYSTEM_INTERRUPT_CLEAR               0x0015
#define VL6180_SYSTEM_FRESH_OUT_OF_RESET            0x0016
#define VL6180_SYSTEM_GROUPED_PARAMETER_HOLD        0x0017

#define VL6180_SYSRANGE_START                       0x0018
#define VL6180_SYSRANGE_THRESH_HIGH                 0x0019
#define VL6180_SYSRANGE_THRESH_LOW                  0x001A
#define VL6180_SYSRANGE_INTERMEASUREMENT_PERIOD     0x001B
#define VL6180_SYSRANGE_MAX_CONVERGENCE_TIME        0x001C
#define VL6180_SYSRANGE_CROSSTALK_COMPENSATION_RATE 0x001E
#define VL6180_SYSRANGE_CROSSTALK_VALID_HEIGHT      0x0021
#define VL6180_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE  0x0022
#define VL6180_SYSRANGE_PART_TO_PART_RANGE_OFFSET   0x0024
#define VL6180_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT   0x0025
#define VL6180_SYSRANGE_RANGE_IGNORE_THRESHOLD      0x0026
#define VL6180_SYSRANGE_MAX_AMBIENT_LEVEL_MULT      0x002C
#define VL6180_SYSRANGE_RANGE_CHECK_ENABLES         0x002D
#define VL6180_SYSRANGE_VHV_RECALIBRATE             0x002E
#define VL6180_SYSRANGE_VHV_REPEAT_RATE             0x0031

#define VL6180_SYSALS_START                         0x0038
#define VL6180_SYSALS_THRESH_HIGH                   0x003A
#define VL6180_SYSALS_THRESH_LOW                    0x003C
#define VL6180_SYSALS_INTERMEASUREMENT_PERIOD       0x003E
#define VL6180_SYSALS_ANALOGUE_GAIN                 0x003F
#define VL6180_SYSALS_INTEGRATION_PERIOD            0x0040

#define VL6180_RESULT_RANGE_STATUS                  0x004D
#define VL6180_RESULT_ALS_STATUS                    0x004E
#define VL6180_RESULT_INTERRUPT_STATUS_GPIO         0x004F
#define VL6180_RESULT_ALS_VAL                       0x0050
#define VL6180_RESULT_HISTORY_BUFFER                0x0052
#define VL6180_RESULT_RANGE_VAL                     0x0062
#define VL6180_RESULT_RANGE_RAW                     0x0064
#define VL6180_RESULT_RANGE_RETURN_RATE             0x0066
#define VL6180_RESULT_RANGE_REFERENCE_RATE          0x0068
#define VL6180_RESULT_RANGE_RETURN_SIGNAL_COUNT     0x006C
#define VL6180_RESULT_RANGE_REFERENCE_SIGNAL_COUNT  0x0070
#define VL6180_RESULT_RANGE_RETURN_AMB_COUNT        0x0074
#define VL6180_RESULT_RANGE_REFERENCE_AMB_COUNT     0x0078
#define VL6180_RESULT_RANGE_RETURN_CONV_TIME        0x007C
#define VL6180_RESULT_RANGE_REFERENCE_CONV_TIME     0x0080

#define VL6180_READOUT_AVERAGING_SAMPLE_PERIOD      0x010A
#define VL6180_FIRMWARE_BOOTUP                      0x0119
#define VL6180_FIRMWARE_RESULT_SCALER               0x0120
#define VL6180_I2C_SLAVE_DEVICE_ADDRESS             0x0212
#define VL6180_INTERLEAVED_MODE_ENABLE              0x02A3


class Vl6180Drv : public i2cbus::I2CDevice, public Device {
    
public:
    Vl6180Drv(std::string devfile, uint32_t addr);
    virtual std::string getValueAtIndex(int index);
    
    static const int NUM_VALUES = 2;
    
protected:
    
    virtual bool initialize();
    virtual std::string readValue0();
    virtual std::string readValue1();
    
private:
    void loadSettings(void);
    uint8_t readRangeStatus(void);
    uint16_t read16(uint8_t reg);
    void write8(uint16_t reg, unsigned char data);
    unsigned char read8(uint16_t reg);
    
    // Create an array of read functions, so that multiple functions can be easily called
    typedef std::string(Vl6180Drv::*readValueType)();
    readValueType readFunction[NUM_VALUES] = { &Vl6180Drv::readValue0, &Vl6180Drv::readValue1 };
        
};

#endif /* defined(__Vl6180Drv__) */
