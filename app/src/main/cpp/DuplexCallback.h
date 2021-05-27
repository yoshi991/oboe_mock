/**
 * Copyright 2021 yoshi991
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DUPLEX_CALLBACK_H__
#define __DUPLEX_CALLBACK_H__

class DuplexInputCallback {
public:
    virtual ~DuplexInputCallback() = default;

    virtual void onInputReady(
        int16_t *buffer, 
        int32_t channelCount,
        int32_t numFrames
    ) {}
};

class DuplexOutputCallback {
public:
    virtual ~DuplexOutputCallback() = default;

    virtual void onOutputReady(
        int16_t *buffer, 
        int32_t channelCount,
        int32_t numFrames
    ) {}
};

#endif //__DUPLEX_CALLBACK_H__