/*
 * Copyright (C) 2018 The Android Open Source Project
 * Copyright (C) 2022 STMicroelectronics
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <android/hardware/sensors/2.1/ISensors.h>
#include <ISTMSensorsCallbackData.h>
#include <STMSensor.h>

namespace android {
namespace hardware {
namespace sensors {
namespace stm {
namespace multihal {

bool convertFromSTMSensorType(const ::stm::core::SensorType& type,
                              V2_1::SensorType& sensorType);

bool convertFromSTMSensor(const ::stm::core::STMSensor& src,
                          V2_1::SensorInfo *dst);

void convertFromSTMSensorData(const ::stm::core::ISTMSensorsCallbackData& sensorData,
                              V2_1::Event& event);

}  // namespace multihal
}  // namespace stm
}  // namespace sensors
}  // namespace hardware
}  // namespace android
