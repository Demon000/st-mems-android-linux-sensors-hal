/*
 * Copyright (C) 2018 The Android Open Source Project
 * Copyright (C) 2015-2020 STMicroelectronics
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

#include <poll.h>

#include "SensorBase.h"
#include <IUtils.h>
#include <IConsole.h>

#if (CONFIG_ST_HAL_ADDITIONAL_INFO_ENABLED)
#include "SensorAdditionalInfo.h"
#endif /* CONFIG_ST_HAL_ADDITIONAL_INFO_ENABLED */

extern "C" {
    #include "utils.h"
}

namespace stm {
namespace core {

#define HW_SENSOR_BASE_DEFAULT_IIO_BUFFER_LEN    (2)
#define HW_SENSOR_BASE_IIO_SYSFS_PATH_MAX        (200)
#define HW_SENSOR_BASE_IIO_DEVICE_NAME_MAX       (30)
#define HW_SENSOR_BASE_MAX_CHANNELS              (8)

struct HWSensorBaseCommonData {
    char device_iio_sysfs_path[HW_SENSOR_BASE_IIO_SYSFS_PATH_MAX];
    char device_name[HW_SENSOR_BASE_IIO_DEVICE_NAME_MAX];
    unsigned int device_iio_dev_num;

    int num_channels;
    struct device_iio_info_channel channels[HW_SENSOR_BASE_MAX_CHANNELS];

    struct device_iio_scales sa;
} typedef HWSensorBaseCommonData;

#ifdef CONFIG_ST_HAL_HAS_SELFTEST_FUNCTIONS
struct selftest_data {
    unsigned int available;
    char mode[5][20];
};
#endif /* CONFIG_ST_HAL_HAS_SELFTEST_FUNCTIONS */

class HWSensorBase;
class HWSensorBaseWithPollrate;

/*
 * class HWSensorBase
 */
class HWSensorBase : public SensorBase {
private:
#if (CONFIG_ST_HAL_ADDITIONAL_INFO_ENABLED)
    void WriteSensorAdditionalInfoFrames(additional_info_event_t array_sensorAdditionaInfoDataFrames[], size_t frames_numb);
#endif /* CONFIG_ST_HAL_ADDITIONAL_INFO_ENABLED */
protected:
    ssize_t scan_size;
    struct pollfd pollfd_iio[2];
    FlushRequested flush_requested;
    HWSensorBaseCommonData common_data;
    ChangeODRTimestampStack odr_switch;
#ifdef CONFIG_ST_HAL_FACTORY_CALIBRATION
    bool factory_calibration_updated;
    float factory_offset[3];
    float factory_scale[3];
#endif /* CONFIG_ST_HAL_FACTORY_CALIBRATION */
#ifdef CONFIG_ST_HAL_HAS_SELFTEST_FUNCTIONS
    struct selftest_data selftest;
#endif /* CONFIG_ST_HAL_HAS_SELFTEST_FUNCTIONS */
    uint8_t *injection_data;
    bool has_event_channels;

    int64_t lastDecimatedPollrate = 0;

    int WriteBufferLenght(unsigned int buf_len);

    IConsole &console { IConsole::getInstance() };
    IUtils &utils { IUtils::getInstance() };

#if (CONFIG_ST_HAL_ADDITIONAL_INFO_ENABLED)
    bool supportsSensorAdditionalInfo;

    const additional_info_event_t defaultSensorPlacement_additional_info_event = {
        .type = AINFO_SENSOR_PLACEMENT,
        .serial = 0,
        .data_float = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 },
    };

    virtual size_t getSensorAdditionalInfoPayLoadFramesArray(additional_info_event_t **array_sensorAdditionalInfoPLFrames);
    void WriteSensorAdditionalInfoReport(additional_info_event_t array_sensorAdditionaInfoDataFrames[], size_t frames);
    void getSensorAdditionalInfoDataFramesArray(additional_info_event_t **array_sensorAdditionalInfoDataFrames, size_t *p_array_size);
#endif /* CONFIG_ST_HAL_ADDITIONAL_INFO_ENABLED */

public:
    HWSensorBase(HWSensorBaseCommonData *data,
                 const char *name,
                 int handle, const STMSensorType &sensor_type,
                 unsigned int hw_fifo_len,
                 float power_consumption);
    virtual ~HWSensorBase();

#ifdef CONFIG_ST_HAL_HAS_SELFTEST_FUNCTIONS
    virtual selftest_status ExecuteSelfTest();
    void GetSelfTestAvailable();
#endif /* CONFIG_ST_HAL_HAS_SELFTEST_FUNCTIONS */

    virtual int Enable(int handle, bool enable, bool lock_en_mute);
    virtual int SetDelay(int handle, int64_t period_ns, int64_t timeout, bool lock_en_mute);

    virtual int AddSensorDependency(SensorBase *p);
    virtual void RemoveSensorDependency(SensorBase *p);

    int ApplyFactoryCalibrationData(char *filename, time_t *last_modification);

    virtual void ProcessData(SensorBaseData *data);
    virtual void ProcessEvent(struct device_iio_events *event_data);
    virtual int FlushData(int handle, bool lock_en_mute);
    virtual void ProcessFlushData(int handle, int64_t timestamp);
    virtual void ThreadDataTask();
    virtual void ThreadEventsTask();

    virtual int InjectionMode(bool enable);
    virtual int InjectSensorData(const sensors_event_t *data);

    bool hasEventChannels() { return has_event_channels; }
    bool hasDataChannels() { return common_data.num_channels > 0; }
};

/*
 * class HWSensorBaseWithPollrate
 */
class HWSensorBaseWithPollrate : public HWSensorBase {
private:
    struct device_iio_sampling_freqs sampling_frequency_available;

public:
    HWSensorBaseWithPollrate(HWSensorBaseCommonData *data, const char *name,
                             struct device_iio_sampling_freqs *sfa, int handle,
                             const STMSensorType &sensor_type,
                             unsigned int hw_fifo_len, float power_consumption);
    virtual ~HWSensorBaseWithPollrate();

    virtual int SetDelay(int handle, int64_t period_ns, int64_t timeout,
                         bool lock_en_mute);
    virtual int FlushData(int handle, bool lock_en_mute);
    virtual void WriteDataToPipe(int64_t hw_pollrate);
};

} // namespace core
} // namespace stm