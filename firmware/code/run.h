/**
 * Copyright 2022 Colin Lam, Ploopy Corporation
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPECIAL THANKS TO:
 * @kilograham (github.com/kilograham)
 * for his exceptional work on Pico Playground's usb-sound-card, on which
 * a large portion of this work is based.
 */

#ifndef RUN_H
#define RUN_H

#include "pico/usb_device.h"
#include "AudioClassCommon.h"

#include "ringbuf.h"
#include "i2s.h"
#include "fix16.h"

/*****************************************************************************
 * USB-related definitions begin here.
 ****************************************************************************/

// todo fix these

// actually windows doesn't seem to like this in the middle, so set top range to 0db
#define CENTER_VOLUME_INDEX 91

#define ENCODE_DB(x) ((uint16_t)(int16_t)((x)*256))

#define MIN_VOLUME ENCODE_DB(-100)
#define DEFAULT_VOLUME ENCODE_DB(0)
#define MAX_VOLUME ENCODE_DB(0)
#define VOLUME_RESOLUTION ENCODE_DB(0.5f)

typedef struct _audio_state_config {
    uint32_t freq;
    union {
        int16_t volume[2];
        int32_t _volume;
    };
    union {
        int16_t target_volume[2];
        int32_t _target_volume;
    };
    union {
        struct {
            //  Register 68
            uint8_t mute: 2;
            uint8_t phase: 1;
            uint8_t reserved1: 3;
            uint8_t oversampling: 1;
            uint8_t reserved2: 1;
            //  Register 69
            uint8_t zero_fn: 1;
            uint8_t zero_polarity: 1;
            uint8_t reserved3: 2;
            uint8_t de_emphasis: 1;
            uint8_t de_emphasis_frequency: 2;
            uint8_t rolloff: 1;
        };
        int8_t target_pcm3060_registers[2];
        int16_t _target_pcm3060_registers;
    };
    int16_t pcm3060_registers;
} audio_state_config;
extern audio_state_config audio_state;

typedef struct _audio_device_config {
    struct usb_configuration_descriptor descriptor;
    struct usb_interface_descriptor ac_interface;
    struct __packed {
        USB_Audio_StdDescriptor_Interface_AC_t core;
        USB_Audio_StdDescriptor_InputTerminal_t input_terminal;
        USB_Audio_StdDescriptor_FeatureUnit_t feature_unit;
        USB_Audio_StdDescriptor_OutputTerminal_t output_terminal;
    } ac_audio;
    struct usb_interface_descriptor as_zero_interface;
    struct usb_interface_descriptor as_op_interface;
    struct __packed {
        USB_Audio_StdDescriptor_Interface_AS_t streaming;
        struct __packed {
            USB_Audio_StdDescriptor_Format_t core;
            USB_Audio_SampleFreq_t freqs[2];
        } format;
    } as_audio;
    struct __packed {
        struct usb_endpoint_descriptor_long core;
        USB_Audio_StdDescriptor_StreamEndpoint_Spc_t audio;
    } ep1;
    struct usb_endpoint_descriptor_long ep2;

    struct usb_interface_descriptor configuration_interface;
    struct usb_endpoint_descriptor ep3;
    struct usb_endpoint_descriptor ep4;
} audio_device_config;

typedef struct _preprocessing_config {
    fix3_28_t preamp;
    int reverse_stereo;
} preprocessing_config;

extern preprocessing_config preprocessing;

static char *descriptor_strings[] = {
    "Ploopy Corporation",
    "Ploopy Headphones",
    "0000000000000001" // Dummy serial number, will be overwritten with the value read from the SPI flash chip. Must be 17bytes.
};

/*****************************************************************************
 * USB-related definitions end here.
 ****************************************************************************/
#define PICO_AUDIO_I2S_DATA_PIN 18 // DIN
#define PICO_AUDIO_I2S_CLOCK_PIN_BASE 16 // BCK / LRCK

#define PCM3060_DAC_SCK_PIN PICO_AUDIO_I2S_CLOCK_PIN_BASE
#define PCM3060_DAC_WS_PIN (PICO_AUDIO_I2S_CLOCK_PIN_BASE+1)
#define PCM3060_DAC_SD_PIN PICO_AUDIO_I2S_DATA_PIN


#define SYSTEM_FREQ 230400000
#define CODEC_FREQ 9216000
#define SAMPLING_FREQ (CODEC_FREQ / 192)

#define CORE0_READY 19813219
#define CORE1_READY 72965426

/*****************************************************************************
 * /seriousness
 ****************************************************************************/

void core1_entry(void);
void setup(void);

const char *_get_descriptor_string(uint);
static void _as_sync_packet(struct usb_endpoint *);
static bool do_get_current(struct usb_setup_packet *);
static bool do_get_minimum(struct usb_setup_packet *);
static bool do_get_maximum(struct usb_setup_packet *);
static bool do_get_resolution(struct usb_setup_packet *);
static void _audio_reconfigure(void);
static void audio_set_volume(int8_t, int16_t);
static void audio_cmd_packet(struct usb_endpoint *);
static bool as_set_alternate(struct usb_interface *, uint);
static bool do_set_current(struct usb_setup_packet *);
static bool ac_setup_request_handler(__unused struct usb_interface *, struct usb_setup_packet *);
bool _as_setup_request_handler(__unused struct usb_endpoint *, struct usb_setup_packet *);
void usb_sound_card_init(void);
extern void power_down_dac();
extern void power_up_dac();
#endif