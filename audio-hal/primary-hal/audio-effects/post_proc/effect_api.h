/*
 * Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.

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

#ifndef OFFLOAD_EFFECT_API_H_
#define OFFLOAD_EFFECT_API_H_
#include "PalApi.h"


#define EQ_PARAM_LATENCY 0x80000000
#define EQ_PARAM_CUSTOM_HEADER_SIZE 12 // 3*sizeof(uint32_t)
#define EQ_PARAM_NORMAL_HEADER_SIZE 8    // 2*sizeof(uint32_t)
#define EQ_ENABLE_CMD_SIZE 4
#define EQ_PRESET_CMD_SIZE 12	//pregain, preset, num band = 0

#define PARAM_ID_MODULE_ENABLE 0x8001026
#define PARAM_ID_EQ_CONFIG 0x800110c
#define PARAM_ID_PBE_PARAMS_CONFIG 0x8001150
#define PARAM_ID_BASS_BOOST_MODE 0x800112c
#define PARAM_ID_BASS_BOOST_STRENGTH 0x800112D
#define PARAM_ID_REVERB_MODE 0x80010fd
#define PARAM_ID_REVERB_PRESET 0x80010fe
#define PARAM_ID_REVERB_WET_MIX 0x80010ff
#define PARAM_ID_REVERB_GAIN_ADJUST 0x8001100
#define PARAM_ID_REVERB_ROOM_LEVEL 0x8001101
#define PARAM_ID_REVERB_ROOM_HF_LEVEL 0x8001102
#define PARAM_ID_REVERB_DECAY_TIME 0x8001103
#define PARAM_ID_REVERB_DECAY_HF_RATIO 0x8001104
#define PARAM_ID_REVERB_REFLECTIONS_LEVEL 0x8001105
#define PARAM_ID_REVERB_REFLECTIONS_DELAY 0x8001106
#define PARAM_ID_REVERB_LEVEL 0x8001107
#define PARAM_ID_REVERB_DELAY 0x8001108
#define PARAM_ID_REVERB_DIFFUSION 0x8001109
#define PARAM_ID_REVERB_DENSITY 0x800110a

#define PARAM_ID_VIRTUALIZER_STRENGTH 0x8001136
#define PARAM_ID_VIRTUALIZER_OUT_TYPE 0x8001137
#define PARAM_ID_VIRTUALIZER_GAIN_ADJUST 0x8001138

typedef enum {
    EQ_PARAM_DISABLE = 0x0,
    EQ_PARAM_ENABLE = 0x1,
}eq_switch_t;

#if __cplusplus
extern "C" {
#endif

#define OFFLOAD_SEND_PBE_ENABLE_FLAG      (1 << 0)
#define OFFLOAD_SEND_PBE_CONFIG           (OFFLOAD_SEND_PBE_ENABLE_FLAG << 1)
void offload_pbe_set_device(struct pbe_params *pbe,
                            uint32_t device);
void offload_pbe_set_enable_flag(struct pbe_params *pbe,
                                 bool enable);
int offload_pbe_get_enable_flag(struct pbe_params *pbe);

int offload_pbe_send_params_pal(pal_stream_handle_t *pal_handle,
                            struct pbe_params *pbe,
                            unsigned param_send_flags);
#define OFFLOAD_SEND_BASSBOOST_ENABLE_FLAG      (1 << 0)
#define OFFLOAD_SEND_BASSBOOST_STRENGTH         \
                                          (OFFLOAD_SEND_BASSBOOST_ENABLE_FLAG << 1)
#define OFFLOAD_SEND_BASSBOOST_MODE             \
                                          (OFFLOAD_SEND_BASSBOOST_STRENGTH << 1)
void offload_bassboost_set_device(struct bass_boost_params *bassboost,
                                  uint32_t device);
void offload_bassboost_set_enable_flag(struct bass_boost_params *bassboost,
                                       bool enable);
int offload_bassboost_get_enable_flag(struct bass_boost_params *bassboost);
void offload_bassboost_set_strength(struct bass_boost_params *bassboost,
                                    int strength);
void offload_bassboost_set_mode(struct bass_boost_params *bassboost,
                                int mode);
int offload_bassboost_send_params_pal(pal_stream_handle_t *pal_handle,
                                  struct bass_boost_params *bassboost,
                                  unsigned param_send_flags);

#define OFFLOAD_SEND_VIRTUALIZER_ENABLE_FLAG    (1 << 0)
#define OFFLOAD_SEND_VIRTUALIZER_STRENGTH       \
                                          (OFFLOAD_SEND_VIRTUALIZER_ENABLE_FLAG << 1)
#define OFFLOAD_SEND_VIRTUALIZER_OUT_TYPE       \
                                          (OFFLOAD_SEND_VIRTUALIZER_STRENGTH << 1)
#define OFFLOAD_SEND_VIRTUALIZER_GAIN_ADJUST    \
                                          (OFFLOAD_SEND_VIRTUALIZER_OUT_TYPE << 1)
void offload_virtualizer_set_device(struct virtualizer_params *virtualizer,
                                    uint32_t device);
void offload_virtualizer_set_enable_flag(struct virtualizer_params *virtualizer,
                                         bool enable);
int offload_virtualizer_get_enable_flag(struct virtualizer_params *virtualizer);
void offload_virtualizer_set_strength(struct virtualizer_params *virtualizer,
                                      int strength);
void offload_virtualizer_set_out_type(struct virtualizer_params *virtualizer,
                                      int out_type);
void offload_virtualizer_set_gain_adjust(struct virtualizer_params *virtualizer,
                                         int gain_adjust);
int offload_virtualizer_send_params_pal(pal_stream_handle_t *pal_stream_handle,
                                  struct virtualizer_params *virtualizer,
                                  unsigned param_send_flags);

#define OFFLOAD_SEND_EQ_ENABLE_FLAG             (1 << 0)
#define OFFLOAD_SEND_EQ_PRESET                  \
                                          (OFFLOAD_SEND_EQ_ENABLE_FLAG << 1)
#define OFFLOAD_SEND_EQ_BANDS_LEVEL             \
                                          (OFFLOAD_SEND_EQ_PRESET << 1)
void offload_eq_set_device(struct eq_params *eq, uint32_t device);
void offload_eq_set_enable_flag(struct eq_params *eq, bool enable);
int offload_eq_get_enable_flag(struct eq_params *eq);
void offload_eq_set_preset(struct eq_params *eq, int preset);
void offload_eq_set_bands_level(struct eq_params *eq, int num_bands,
                                const uint16_t *band_freq_list,
                                int *band_gain_list);
int offload_eq_send_params_pal(pal_stream_handle_t *pal_handle, struct eq_params *eq,
                           unsigned param_send_flags);

#define OFFLOAD_SEND_REVERB_ENABLE_FLAG         (1 << 0)
#define OFFLOAD_SEND_REVERB_MODE                \
                                          (OFFLOAD_SEND_REVERB_ENABLE_FLAG << 1)
#define OFFLOAD_SEND_REVERB_PRESET              \
                                          (OFFLOAD_SEND_REVERB_MODE << 1)
#define OFFLOAD_SEND_REVERB_WET_MIX             \
                                          (OFFLOAD_SEND_REVERB_PRESET << 1)
#define OFFLOAD_SEND_REVERB_GAIN_ADJUST	        \
                                          (OFFLOAD_SEND_REVERB_WET_MIX << 1)
#define OFFLOAD_SEND_REVERB_ROOM_LEVEL	        \
                                          (OFFLOAD_SEND_REVERB_GAIN_ADJUST << 1)
#define OFFLOAD_SEND_REVERB_ROOM_HF_LEVEL       \
                                          (OFFLOAD_SEND_REVERB_ROOM_LEVEL << 1)
#define OFFLOAD_SEND_REVERB_DECAY_TIME          \
                                          (OFFLOAD_SEND_REVERB_ROOM_HF_LEVEL << 1)
#define OFFLOAD_SEND_REVERB_DECAY_HF_RATIO      \
                                          (OFFLOAD_SEND_REVERB_DECAY_TIME << 1)
#define OFFLOAD_SEND_REVERB_REFLECTIONS_LEVEL   \
                                          (OFFLOAD_SEND_REVERB_DECAY_HF_RATIO << 1)
#define OFFLOAD_SEND_REVERB_REFLECTIONS_DELAY   \
                                          (OFFLOAD_SEND_REVERB_REFLECTIONS_LEVEL << 1)
#define OFFLOAD_SEND_REVERB_LEVEL               \
                                          (OFFLOAD_SEND_REVERB_REFLECTIONS_DELAY << 1)
#define OFFLOAD_SEND_REVERB_DELAY               \
                                          (OFFLOAD_SEND_REVERB_LEVEL << 1)
#define OFFLOAD_SEND_REVERB_DIFFUSION           \
                                          (OFFLOAD_SEND_REVERB_DELAY << 1)
#define OFFLOAD_SEND_REVERB_DENSITY             \
                                          (OFFLOAD_SEND_REVERB_DIFFUSION << 1)
void offload_reverb_set_device(struct reverb_params *reverb, uint32_t device);
void offload_reverb_set_enable_flag(struct reverb_params *reverb, bool enable);
int offload_reverb_get_enable_flag(struct reverb_params *reverb);
void offload_reverb_set_mode(struct reverb_params *reverb, int mode);
void offload_reverb_set_preset(struct reverb_params *reverb, int preset);
void offload_reverb_set_wet_mix(struct reverb_params *reverb, int wet_mix);
void offload_reverb_set_gain_adjust(struct reverb_params *reverb,
                                    int gain_adjust);
void offload_reverb_set_room_level(struct reverb_params *reverb,
                                   int room_level);
void offload_reverb_set_room_hf_level(struct reverb_params *reverb,
                                      int room_hf_level);
void offload_reverb_set_decay_time(struct reverb_params *reverb,
                                   int decay_time);
void offload_reverb_set_decay_hf_ratio(struct reverb_params *reverb,
                                       int decay_hf_ratio);
void offload_reverb_set_reflections_level(struct reverb_params *reverb,
                                          int reflections_level);
void offload_reverb_set_reflections_delay(struct reverb_params *reverb,
                                          int reflections_delay);
void offload_reverb_set_reverb_level(struct reverb_params *reverb,
                                     int reverb_level);
void offload_reverb_set_delay(struct reverb_params *reverb, int delay);
void offload_reverb_set_diffusion(struct reverb_params *reverb, int diffusion);
void offload_reverb_set_density(struct reverb_params *reverb, int density);
int offload_reverb_send_params_pal(pal_stream_handle_t *pal_stream_handle,
                               struct reverb_params *reverb,
                               unsigned param_send_flags);

#define OFFLOAD_SEND_SOFT_VOLUME_ENABLE_FLAG         (1 << 0)
#define OFFLOAD_SEND_SOFT_VOLUME_GAIN_2CH             \
                                          (OFFLOAD_SEND_SOFT_VOLUME_ENABLE_FLAG << 1)
#define OFFLOAD_SEND_SOFT_VOLUME_GAIN_MASTER          \
                                          (OFFLOAD_SEND_SOFT_VOLUME_GAIN_2CH << 1)
void offload_soft_volume_set_enable(struct soft_volume_params *vol,
                                    bool enable);
void offload_soft_volume_set_gain_master(struct soft_volume_params *vol,
                                         int gain);
void offload_soft_volume_set_gain_2ch(struct soft_volume_params *vol,
                                      int l_gain, int r_gain);

#define OFFLOAD_SEND_TRANSITION_SOFT_VOLUME_ENABLE_FLAG         (1 << 0)
#define OFFLOAD_SEND_TRANSITION_SOFT_VOLUME_GAIN_2CH             \
                                  (OFFLOAD_SEND_TRANSITION_SOFT_VOLUME_ENABLE_FLAG << 1)
#define OFFLOAD_SEND_TRANSITION_SOFT_VOLUME_GAIN_MASTER          \
                                  (OFFLOAD_SEND_TRANSITION_SOFT_VOLUME_GAIN_2CH << 1)
void offload_transition_soft_volume_set_enable(struct soft_volume_params *vol,
                                               bool enable);
void offload_transition_soft_volume_set_gain_master(struct soft_volume_params *vol,
                                                    int gain);
void offload_transition_soft_volume_set_gain_2ch(struct soft_volume_params *vol,
                                                 int l_gain, int r_gain);
#if __cplusplus
} //extern "C"
#endif

#endif /*OFFLOAD_EFFECT_API_H_*/
