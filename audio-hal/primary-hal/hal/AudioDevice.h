/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ANDROID_HARDWARE_AHAL_ADEVICE_H_
#define ANDROID_HARDWARE_AHAL_ADEVICE_H_

#include <stdlib.h>
#include <unistd.h>
#include <mutex>
#include <vector>
#include <set>
#include <string>

#include <cutils/properties.h>
#include <hardware/audio.h>
#include <system/audio.h>

#include <expat.h>

#include "AudioStream.h"
#include "AudioVoice.h"
#include "PalDefs.h"

#ifdef SEC_AUDIO_COMMON
// DEVICE
#include "SecAudioDevice.h"
// FACTORY
#include "AudioFactory.h"
// OFFLOAD
#include "AudioEffect.h"
#endif

#define COMPRESS_VOIP_IO_BUF_SIZE_NB 320
#define COMPRESS_VOIP_IO_BUF_SIZE_WB 640
#define COMPRESS_VOIP_IO_BUF_SIZE_SWB 1280
#define COMPRESS_VOIP_IO_BUF_SIZE_FB 1920
#define MAX_PERF_LOCK_OPTS 20

#ifdef SEC_AUDIO_HIDL
#define SEC_AUDIO_MAX_STREAM_CNT 20
#endif

/* HDR Audio use case parameters */
#define AUDIO_PARAMETER_KEY_HDR "hdr_record_on"
#define AUDIO_PARAMETER_KEY_WNR "wnr_on"
#define AUDIO_PARAMETER_KEY_ANS "ans_on"
#define AUDIO_PARAMETER_KEY_ORIENTATION "orientation"
#define AUDIO_PARAMETER_KEY_INVERTED "inverted"
#define AUDIO_PARAMETER_KEY_FACING "facing"
#define AUDIO_PARAMETER_KEY_HDR_CHANNELS "hdr_audio_channel_count"
#define AUDIO_PARAMETER_KEY_HDR_SAMPLERATE "hdr_audio_sampling_rate"

#define AUDIO_MAKE_STRING_FROM_ENUM(X)   { #X, X }
#define PAL_MAX_INPUT_DEVICES (PAL_DEVICE_IN_MAX - (PAL_DEVICE_IN_MIN + 1))
#define MIC_INFO_MAP_INDEX(X) (X - (PAL_DEVICE_IN_MIN + 1))
#define XML_READ_BUFFER_SIZE 1024
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

typedef enum {
    TAG_MICROPHONE_CHARACTERISTIC,
    TAG_SND_DEVICES,
    TAG_INPUT_SND_DEVICE,
    TAG_INPUT_SND_DEVICE_TO_MIC_MAPPING,
    TAG_SND_DEV,
    TAG_MIC_INFO
} mic_xml_tags_t;

typedef struct xml_userdata_t {
    char data_buf[XML_READ_BUFFER_SIZE];
    size_t offs;
    mic_xml_tags_t tag;
} xml_userdata_t;

typedef struct microphone_characteristics_t
{
    uint32_t declared_mic_count;
    struct audio_microphone_characteristic_t microphone[AUDIO_MICROPHONE_MAX_COUNT];
} microphone_characteristics_t;

typedef struct mic_info_t {
    char device_id[AUDIO_MICROPHONE_ID_MAX_LEN];
    uint32_t channel_count;
    audio_microphone_channel_mapping_t channel_mapping[AUDIO_CHANNEL_COUNT_MAX];
} mic_info_t;

typedef struct snd_device_to_mic_map_t {
    mic_info_t microphones[AUDIO_MICROPHONE_MAX_COUNT];
    uint32_t mic_count;
} snd_device_to_mic_map_t;

class AudioPatch{
    public:
        enum PatchType{
            PATCH_NONE = -1,
            PATCH_PLAYBACK,
            PATCH_CAPTURE,
            PATCH_DEVICE_LOOPBACK
        };
        AudioPatch() = default;
        AudioPatch(PatchType patch_type,
                   const std::vector<struct audio_port_config>& sources,
                   const std::vector<struct audio_port_config>& sinks);
   protected:
        enum PatchType type = PATCH_NONE;
        audio_patch_handle_t handle = AUDIO_PATCH_HANDLE_NONE;
        std::vector<struct audio_port_config> sources, sinks;
        static audio_patch_handle_t generate_patch_handle_l();
        friend class AudioDevice;
};

class AudioDevice {
public:
    ~AudioDevice();
    static std::shared_ptr<AudioDevice> GetInstance();
    static std::shared_ptr<AudioDevice> GetInstance(audio_hw_device_t* device);
    int Init(hw_device_t **device, const hw_module_t *module);
    std::shared_ptr<StreamOutPrimary> CreateStreamOut(
            audio_io_handle_t handle,
            const std::set<audio_devices_t> &devices,
            audio_output_flags_t flags,
            struct audio_config *config,
            audio_stream_out **stream_out,
            const char *address);
    void CloseStreamOut(std::shared_ptr<StreamOutPrimary> stream);
    int CreateAudioPatch(audio_patch_handle_t* handle,
                         const std::vector<struct audio_port_config>& sources,
                         const std::vector<struct audio_port_config>& sinks);
    int ReleaseAudioPatch(audio_patch_handle_t handle);
    int SetGEFParam(void *data, int length);
    int GetGEFParam(void *data, int *length);
    std::shared_ptr<StreamOutPrimary> OutGetStream(audio_io_handle_t handle);
    std::shared_ptr<StreamOutPrimary> OutGetStream(audio_stream_t* audio_stream);
    std::shared_ptr<StreamInPrimary> CreateStreamIn(
            audio_io_handle_t handle,
            const std::set<audio_devices_t> &devices,
            audio_input_flags_t flags,
            struct audio_config *config,
            const char *address,
            audio_stream_in **stream_in,
            audio_source_t source);
    void CloseStreamIn(std::shared_ptr<StreamInPrimary> stream);
    std::shared_ptr<StreamInPrimary> InGetStream(audio_io_handle_t handle);
    std::shared_ptr<StreamInPrimary> InGetStream(audio_stream_t* stream_in);
    std::shared_ptr<AudioVoice> voice_;
#ifdef SEC_AUDIO_COMMON
    // DEVICE
    std::shared_ptr<SecAudioDevice> sec_device_;
    // FACTORY
    std::shared_ptr<AudioFactory> factory_;
    // EFFECT
    std::shared_ptr<AudioEffect> effect_;
#endif
#ifdef SEC_AUDIO_HIDL
	audio_stream_t *sec_out_streams[SEC_AUDIO_MAX_STREAM_CNT];	// Opened output streams
	audio_stream_t *sec_in_streams[SEC_AUDIO_MAX_STREAM_CNT];  // Opened input streams
#endif
    int SetMicMute(bool state);
    bool mute_;
    int GetMicMute(bool *state);
    int SetParameters(const char *kvpairs);
    char* GetParameters(const char *keys);
    int SetMode(const audio_mode_t mode);
    int SetVoiceVolume(float volume);
    void SetChargingMode(bool is_charging);
    void FillAndroidDeviceMap();
    int GetPalDeviceIds(
            const std::set<audio_devices_t>& hal_device_id,
            pal_device_id_t* pal_device_id);
    int usb_card_id_;
    int usb_dev_num_;
    int dp_controller;
    int dp_stream;
    int num_va_sessions_ = 0;
    pal_speaker_rotation_type current_rotation;
    static card_status_t sndCardState;
    std::mutex adev_init_mutex;
    uint32_t adev_init_ref_count = 0;
    hw_device_t *GetAudioDeviceCommon();
    int perf_lock_handle;
    int perf_lock_opts[MAX_PERF_LOCK_OPTS];
    int perf_lock_opts_size;
    bool hdr_record_enabled = false;
    bool wnr_enabled = false;
    bool ans_enabled = false;
    bool orientation_landscape = true;
    bool inverted = false;
    int  facing = 0; /*0 - none, 1 - back, 2 - front/selfie*/
    int  hdr_channel_count = 0;
    int  hdr_sample_rate = 0;
    int cameraOrientation = CAMERA_DEFAULT;
    bool usb_input_dev_enabled = false;
    static bool mic_characteristics_available;
    static microphone_characteristics_t microphones;
    static snd_device_to_mic_map_t microphone_maps[PAL_MAX_INPUT_DEVICES];
    static bool find_enum_by_string(const struct audio_string_to_enum * table, const char * name,
                                    int32_t len, unsigned int *value);
    static bool set_microphone_characteristic(struct audio_microphone_characteristic_t mic);
    static int32_t get_microphones(struct audio_microphone_characteristic_t *mic_array, size_t *mic_count);
    static void process_microphone_characteristics(const XML_Char **attr);
    static bool is_input_pal_dev_id(int deviceId);
    static void process_snd_dev(const XML_Char **attr);
    static bool set_microphone_map(pal_device_id_t in_snd_device, const mic_info_t *info);
    static bool is_built_in_input_dev(pal_device_id_t deviceId);
    static void process_mic_info(const XML_Char **attr);
    int32_t get_active_microphones(uint32_t channels, pal_device_id_t id,
                                          struct audio_microphone_characteristic_t *mic_array,
                                          uint32_t *mic_count);
    static void xml_start_tag(void *userdata, const XML_Char *tag_name,
                             const XML_Char **attr);
    static void xml_end_tag(void *userdata, const XML_Char *tag_name);
    static void xml_char_data_handler(void *userdata, const XML_Char *s, int len);
    static int parse_xml();
#ifdef SEC_AUDIO_CALL
    audio_io_handle_t primary_out_io_handle = AUDIO_IO_HANDLE_NONE;
#endif
#ifdef SEC_AUDIO_BLUETOOTH
    bool bt_sco_on = false;
#endif
#ifdef SEC_AUDIO_SUPPORT_USB_OFFLOAD
    bool USBConnected(void);
#endif
#ifdef SEC_AUDIO_COMMON
    void SetForceRouteOutStream(const std::set<audio_devices_t>& new_devices);
    void SetForceRouteInStream(const std::set<audio_devices_t>& new_devices);
    std::shared_ptr<StreamInPrimary> GetActiveInStream();
    std::shared_ptr<StreamOutPrimary> OutGetStream(pal_stream_type_t pal_stream_type);
#endif
#ifdef SEC_AUDIO_HIDL
    audio_hw_device_t* GetAudioDeviceInstance();
#endif
protected:
    AudioDevice() {}
    std::shared_ptr<AudioVoice> VoiceInit();
#ifdef SEC_AUDIO_COMMON
    // DEVICE
    std::shared_ptr<SecAudioDevice> SecDeviceInit();
    // FACTORY
    std::shared_ptr<AudioFactory> FactoryInit();
    // OFFLOAD
    std::shared_ptr<AudioEffect> EffectInit();
#endif
#ifdef SEC_AUDIO_HIDL
    void SecInitAudioStreams(audio_hw_device_t* dev);
#endif
    static std::shared_ptr<AudioDevice> adev_;
    static std::shared_ptr<audio_hw_device_t> device_;
    std::vector<std::shared_ptr<StreamOutPrimary>> stream_out_list_;
    std::vector<std::shared_ptr<StreamInPrimary>> stream_in_list_;
    std::mutex out_list_mutex;
    std::mutex in_list_mutex;
    std::mutex patch_map_mutex;
    btsco_lc3_cfg_t btsco_lc3_cfg;
    void *offload_effects_lib_;
#ifdef SEC_AUDIO_OFFLOAD
    offload_effects_update_output fnp_offload_effect_update_output_ = nullptr;
#else
    offload_effects_start_output fnp_offload_effect_start_output_ = nullptr;
    offload_effects_stop_output fnp_offload_effect_stop_output_ = nullptr;
    visualizer_hal_start_output fnp_visualizer_start_output_ = nullptr;
    visualizer_hal_stop_output fnp_visualizer_stop_output_ = nullptr;
#endif
    bool is_charging_;
    void *visualizer_lib_;
    std::map<audio_devices_t, pal_device_id_t> android_device_map_;
    std::map<audio_patch_handle_t, AudioPatch*> patch_map_;
    int add_input_headset_if_usb_out_headset(int *device_count,  pal_device_id_t** pal_device_ids);
};

#endif //ANDROID_HARDWARE_AHAL_ADEVICE_H_
