/*
 * Copyright (C) 2018 The Android Open Source Project
 * Copyright (C) 2019-2020 STMicroelectronics
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

#include <ctime>

#include <IUtils.h>

using stm::core::IUtils;

class Utils : public IUtils {
public:
    Utils(void) = default;
    ~Utils(void) = default;

    Utils(const Utils &) = delete;
    Utils& operator= (const Utils &) = delete;

    int64_t getTime(void) const override;
};

int64_t Utils::getTime(void) const
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts)) {
      return 0;
    }

    return (ts.tv_sec * 1000000000) + ts.tv_nsec;
}

IUtils& IUtils::getInstance(void)
{
    static Utils instance;

    return instance;
}
