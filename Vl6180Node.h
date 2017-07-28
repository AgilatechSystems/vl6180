/**
 * \file Vl6180Node.h
 *
 *  Created by Scott Erholm on 07/27/2017.
 *  Copyright (c) 2017 Agilatech. All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __Vl6180Node__
#define __Vl6180Node__

#include <node.h>
#include <node_object_wrap.h>
#include <uv.h>
#include <iostream>
#include <cmath>
#include <string>
#include <thread>
#include "Vl6180Drv.h"

namespace vl6180 {
    
class Vl6180Node : public node::ObjectWrap {
 
public:
    static void Init(v8::Local<v8::Object> exports);
    
    static void getDeviceName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDeviceType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDeviceVersion(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getDeviceNumValues (const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getTypeAtIndex (const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getNameAtIndex (const v8::FunctionCallbackInfo<v8::Value>& args);
    static void isDeviceActive (const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getValueAtIndexSync (const v8::FunctionCallbackInfo<v8::Value>& args);
    static void getValueAtIndex (const v8::FunctionCallbackInfo<v8::Value>& args);
    
private:
    
    explicit Vl6180Node(std::string devfile = "/dev/i2c-1", uint32_t addr = 0x29) {}
    
    ~Vl6180Node() {}
    
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    static void WorkAsync(uv_work_t *req);
    static void WorkAsyncComplete(uv_work_t *req,int status);
    
    static v8::Persistent<v8::Function> constructor;
    
    static Vl6180Drv *driver;
    
    struct Work {
        uv_work_t  request;
        v8::Persistent<v8::Function> callback;
        
        int valueIndex;
        std::string value;
    };

    
};

    
} // namespace


#endif /* defined(__Vl6180Node__) */
