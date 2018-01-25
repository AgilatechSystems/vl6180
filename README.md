## Node addon for hardware VL6180 (x) sensor

##### This addon should work on any Linux platform, and has been thoroughly tested on BBB

### Install

Notably, this module is not dependant on any other modules except node-gyp for compilation.

```
npm install @agilatech/vl6180
```
OR
```
git clone https://github.com/Agilatech/vl6180.git
node-gyp configure build
```


### Usage
##### Load the module and create an instance
```
const addon = require('@agilatech/vl6180');

// create an instance on the /dev/i2c-1 I2C file at address 0x29
const vl6180 = new addon.Vl6180('/dev/i2c-1', 0x29);
```
If either the bus or address args are omitted, it defaults to /dev/i2c-1 and 0x29 respectively.


##### Get basic device info
```
const name = vl6180.deviceName();  // returns string with name of device
const type = vl6180.deviceType();  // returns string with type of device
const version = vl6180.deviceVersion(); // returns this software version
const active = vl6180.deviceActive(); // true if active, false if inactive
const numVals =  vl6180.deviceNumValues(); // returns the number of paramters sensed
```


#### Get parameter info and values
Sensed parameter values are at separate indicies.  The reasoning here is to support a generic sensor platform.
```
// range is at index 0
const paramName0 = vl6180.nameAtIndex(0);
const paramType0 = vl6180.typeAtIndex(0);
const paramVal0  = vl6180.valueAtIndexSync(0);

// lux is at index 1
const paramName1 = vl6180.nameAtIndex(1);
const paramType1 = vl6180.typeAtIndex(1);
const paramVal1  = vl6180.valueAtIndexSync(1);
```
If the device is not active, or if any parameter is disabled, the return value will be "none".


#### Asynchronous value collection is also available
```
vl6180.valueAtIndex(0, function(err, val) {
    if (err) {
        console.log(err);
    }
    else {
        console.log(`Asynchronous call return: ${val}`);
    }
});
```


### Operation Notes
The VL6180 is a "Time of Flight" distance/proximity sensor.  It measures the time the IR emitted light takes to traverse the distance.  This unit measures from 0-100mm.  Note that beyond 100mm, the value returned is 255. The sensor also includes a lux light sensor.

In addition to the device info, this module returns name, type, and value at two indicies, 0 and 1.  Index 0 is for range, while index 1 is for lux.  Typical usage might look like this:

```
const range  = vl6180.valueAtIndexSync(0);
const lux = vl6180.valueAtIndexSync(1);
```



### Dependencies
* node-gyp


### Copyright
Copyright © 2018 Agilatech. All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

