/**
 * \file Vl6180Drv.cpp
 *
 *  Created by Scott Erholm on 07/27/2017.
 *  Copyright (c) 2017 Agilatech. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "Vl6180Drv.h"

const std::string Device::name = "VL6180";
const std::string Device::type = "sensor";

const int Device::numValues = Vl6180Drv::NUM_VALUES;

const std::string Device::valueNames[numValues] = {"range", "lux"};
const std::string Device::valueTypes[numValues] = {"integer", "float"};

Vl6180Drv::Vl6180Drv(std::string devfile, uint32_t addr):i2cbus::I2CDevice(devfile,addr) {
    
    if (initialize()) {
        this->active = true;
    }
    else {
        std::cerr << name << " did not initialize. " << name << " is inactive" << std::endl;
    }
    
}

std::string Vl6180Drv::getValueAtIndex(int index) {
    
    if (!this->active) {
        return "none";
    }
    
    if (index < numValues) {
        return (this->*readFunction[index])();
    }
    else {
        return "none";
    }
}

bool Vl6180Drv::initialize() {
    
    if (read8(VL6180_IDENTIFICATION_MODEL_ID) != 0xB4) {
        return false;
    }
    
    loadSettings();
    
    write8(VL6180_SYSTEM_FRESH_OUT_OF_RESET, 0x00);

    return true;
}

std::string Vl6180Drv::readValue0() {
    
    if (!this->active) {
        return "none";
    }
    
    unsigned char range_status;
    unsigned char status;
    
    // wait for device to be ready for range measurement
    while (! (read8(VL6180_RESULT_RANGE_STATUS) & 0x01));
    
    // Start a range measurement
    write8(VL6180_SYSRANGE_START, 0x01);
    
    // check the status
    status = read8(VL6180_RESULT_INTERRUPT_STATUS_GPIO);
    range_status = status & 0x07;
    
    // wait for new measurement ready status
    while (range_status != 0x04) {
        status = read8(VL6180_RESULT_INTERRUPT_STATUS_GPIO);
        range_status = status & 0x07;
    }
    
    // read range in mm
    uint8_t range = read8(VL6180_RESULT_RANGE_VAL);
    
    // clear interrupt
    write8(VL6180_SYSTEM_INTERRUPT_CLEAR, 0x07);
    
    return DataManip::dataToString(range);
}

std::string Vl6180Drv::readValue1() {
    
    if (!this->active) {
        return "none";
    }
    
    uint8_t reg;
    uint8_t gain = VL6180_ALS_GAIN_5; // start at 5x gain
    
    reg = read8(VL6180_SYSTEM_INTERRUPT_CONFIG_GPIO);
    reg &= ~0x38;
    reg |= (0x4 << 3); // IRQ on ALS ready
    write8(VL6180_SYSTEM_INTERRUPT_CONFIG_GPIO, reg);
    
    // analog gain
    if (gain > VL6180_ALS_GAIN_40) {
        gain = VL6180_ALS_GAIN_40;
    }

    write8(VL6180_SYSALS_ANALOGUE_GAIN, 0x40 | gain);
    
    // start ALS
    write8(VL6180_SYSALS_START, 0x1);
    
    // Poll until "New Sample Ready threshold event" is set
    while (4 != ((read8(VL6180_RESULT_INTERRUPT_STATUS_GPIO) >> 3) & 0x7));
    
    // read lux
    float lux = read16(VL6180_RESULT_ALS_VAL);
    
    // clear interrupt
    write8(VL6180_SYSTEM_INTERRUPT_CLEAR, 0x07);
    
    lux *= 0.32; // calibrated count/lux
    switch(gain) {
        case VL6180_ALS_GAIN_1:
            break;
        case VL6180_ALS_GAIN_1_25:
            lux /= 1.25;
            break;
        case VL6180_ALS_GAIN_1_67:
            lux /= 1.76;
            break;
        case VL6180_ALS_GAIN_2_5:
            lux /= 2.5;
            break;
        case VL6180_ALS_GAIN_5:
            lux /= 5;
            break;
        case VL6180_ALS_GAIN_10:
            lux /= 10;
            break;
        case VL6180_ALS_GAIN_20: 
            lux /= 20;
            break;
        case VL6180_ALS_GAIN_40: 
            lux /= 20;
            break;
    }
    lux *= 100;
    lux /= 100; // integration time in ms
    
    return DataManip::dataToString(lux, 1);
}

void Vl6180Drv::loadSettings(void) {
    
    // private settings from page 24 of app note
    write8(0x0207, 0x01);
    write8(0x0208, 0x01);
    write8(0x0096, 0x00);
    write8(0x0097, 0xfd);
    write8(0x00e3, 0x00);
    write8(0x00e4, 0x04);
    write8(0x00e5, 0x02);
    write8(0x00e6, 0x01);
    write8(0x00e7, 0x03);
    write8(0x00f5, 0x02);
    write8(0x00d9, 0x05);
    write8(0x00db, 0xce);
    write8(0x00dc, 0x03);
    write8(0x00dd, 0xf8);
    write8(0x009f, 0x00);
    write8(0x00a3, 0x3c);
    write8(0x00b7, 0x00);
    write8(0x00bb, 0x3c);
    write8(0x00b2, 0x09);
    write8(0x00ca, 0x09);
    write8(0x0198, 0x01);
    write8(0x01b0, 0x17);
    write8(0x01ad, 0x00);
    write8(0x00ff, 0x05);
    write8(0x0100, 0x05);
    write8(0x0199, 0x05);
    write8(0x01a6, 0x1b);
    write8(0x01ac, 0x3e);
    write8(0x01a7, 0x1f);
    write8(0x0030, 0x00);
    
    // Recommended : Public registers - From the ST data sheet
    
    // Enables polling for 'New Sample ready' when measurement completes
    write8(VL6180_SYSTEM_MODE_GPIO1, 0x10);
    
    // Set the averaging sample period (compromise between lower noise and
    // increased execution time)
    write8(VL6180_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30);
    
    // Sets the light and dark gain (upper nibble). Dark gain should not be changed
    write8(VL6180_SYSALS_ANALOGUE_GAIN, 0x46);
    
    // sets the # of range measurements after which auto calibration of
    // system is performed
    write8(VL6180_SYSRANGE_VHV_REPEAT_RATE, 0xFF);
    
    // Set ALS integration time to 100ms
    write8(VL6180_SYSALS_INTEGRATION_PERIOD, 0x64);
    
    // perform a single temperature calibration of the ranging sensor
    write8(VL6180_SYSRANGE_VHV_RECALIBRATE, 0x01);
    
    // Optional: Public registers - See data sheet for more detail
    
    // Set default ranging inter-measurement period to 100ms
    write8(VL6180_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09);
    
    // Set default ALS inter-measurement period to 500ms
    write8(VL6180_SYSALS_INTERMEASUREMENT_PERIOD, 0x31);
    
    // Configures interrupt on 'New Sample Ready threshold event'
    write8(VL6180_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24);
    
    write8(VL6180_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32);
    write8(VL6180_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);
    write8(VL6180_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x7B );
    
    write8(VL6180_FIRMWARE_RESULT_SCALER,0x01);
}

uint8_t Vl6180Drv::readRangeStatus(void) {
    return (read8(VL6180_RESULT_RANGE_STATUS) >> 4);
}

uint16_t Vl6180Drv::read16(uint8_t reg) {
    uint16_t h; uint16_t l;
    
    l = read8(reg);
    h = read8(reg+1);
    
    return ((short)h<<8)|(short)l;
}

// seems that for this device, we need to split the 16-bit register in two,
// so the usual writeRegister does not work, hence this method
void Vl6180Drv::write8(uint16_t reg, unsigned char data) {
    unsigned char data_write[3];
    data_write[0] = (reg >> 8) & 0xFF;; // MSB of register address
    data_write[1] = reg & 0xFF; // LSB of register address
    data_write[2] = data & 0xFF;
    
    ::write(this->file, data_write, 3);
}

// seems that for this device, we need to split the 16-bit register in two,
// so the usual readRegister does not work, hence this method
unsigned char Vl6180Drv::read8(uint16_t reg) {
    unsigned char data_write[2];
    unsigned char data_read[1];
    data_write[0] = (reg >> 8) & 0xFF; // MSB of register address
    data_write[1] = reg & 0xFF; // LSB of register address
    
    ::write(this->file, data_write, 2);
    ::read(this->file, data_read, 1);
    
    return data_read[0];
}




