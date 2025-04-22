#ifndef __UAC_COMM_H__
#define __UAC_COMM_H__

#include "no_os_defs.h"
#include <stdbool.h>
#include "usb.h"

typedef struct audio_frame_buf {
    void*          addr0;
    int            num_samples;
} audio_frame_buf_t;


#ifdef __cplusplus
extern "C" {
#endif

/* A.8. Audio Class-Specific Request Codes */
#define UAC_RC_UNDEFINED		    0x00
#define UAC_SET_CUR					0x01
#define UAC_SET_RES					0x04
#define UAC_GET_CUR					0x81
#define UAC_GET_MIN					0x82
#define UAC_GET_MAX					0x83
#define UAC_GET_RES					0x84
#define UAC_GET_LEN					0x85
#define UAC_GET_INFO			    0x86
#define UAC_GET_DEF					0x87


/** bInterfaceProtocol values to denote the version of the standard used */
#define UAC_VERSION_1			0x00
#define UAC_VERSION_2			0x20

/** A.2 Audio Interface Subclass Codes */
#define USB_SUBCLASS_AUDIOCONTROL	0x01
#define USB_SUBCLASS_AUDIOSTREAMING	0x02
#define USB_SUBCLASS_MIDISTREAMING	0x03

/** A.5 Audio Class-Specific AC Interface Descriptor Subtypes */
#define UAC_HEADER			0x01
#define UAC_INPUT_TERMINAL		0x02
#define UAC_OUTPUT_TERMINAL		0x03
#define UAC_MIXER_UNIT			0x04
#define UAC_SELECTOR_UNIT		0x05
#define UAC_FEATURE_UNIT		0x06
#define UAC1_PROCESSING_UNIT		0x07
#define UAC1_EXTENSION_UNIT		0x08

/** A.6 Audio Class-Specific AS Interface Descriptor Subtypes */
#define UAC_AS_GENERAL			0x01
#define UAC_FORMAT_TYPE			0x02
#define UAC_FORMAT_SPECIFIC		0x03

/** A.7 Processing Unit Process Types */
#define UAC_PROCESS_UNDEFINED		0x00
#define UAC_PROCESS_UP_DOWNMIX		0x01
#define UAC_PROCESS_DOLBY_PROLOGIC	0x02
#define UAC_PROCESS_STEREO_EXTENDER	0x03
#define UAC_PROCESS_REVERB		0x04
#define UAC_PROCESS_CHORUS		0x05
#define UAC_PROCESS_DYN_RANGE_COMP	0x06

/** A.8 Audio Class-Specific Endpoint Descriptor Subtypes */
#define UAC_EP_GENERAL			0x01

/** A.9 Audio Class-Specific Request Codes */

#define UAC_GET_STAT			0xff

/** A.10 Control Selector Codes */

/** A.10.1 Terminal Control Selectors */
#define UAC_TERM_COPY_PROTECT		0x01

/** A.10.2 Feature Unit Control Selectors */
#define UAC_FU_MUTE			0x01
#define UAC_FU_VOLUME			0x02
#define UAC_FU_BASS			0x03
#define UAC_FU_MID			0x04
#define UAC_FU_TREBLE			0x05
#define UAC_FU_GRAPHIC_EQUALIZER	0x06
#define UAC_FU_AUTOMATIC_GAIN		0x07
#define UAC_FU_DELAY			0x08
#define UAC_FU_BASS_BOOST		0x09
#define UAC_FU_LOUDNESS			0x0a

#define UAC_CONTROL_BIT(CS)	(1 << ((CS) - 1))

/** A.10.3.1 Up/Down-mix Processing Unit Controls Selectors */
#define UAC_UD_ENABLE			0x01
#define UAC_UD_MODE_SELECT		0x02

/** A.10.3.2 Dolby Prologic (tm) Processing Unit Controls Selectors */
#define UAC_DP_ENABLE			0x01
#define UAC_DP_MODE_SELECT		0x02

/** A.10.3.3 3D Stereo Extender Processing Unit Control Selectors */
#define UAC_3D_ENABLE			0x01
#define UAC_3D_SPACE			0x02

/** A.10.3.4 Reverberation Processing Unit Control Selectors */
#define UAC_REVERB_ENABLE		0x01
#define UAC_REVERB_LEVEL		0x02
#define UAC_REVERB_TIME			0x03
#define UAC_REVERB_FEEDBACK		0x04

/** A.10.3.5 Chorus Processing Unit Control Selectors */
#define UAC_CHORUS_ENABLE		0x01
#define UAC_CHORUS_LEVEL		0x02
#define UAC_CHORUS_RATE			0x03
#define UAC_CHORUS_DEPTH		0x04

/** A.10.3.6 Dynamic Range Compressor Unit Control Selectors */
#define UAC_DCR_ENABLE			0x01
#define UAC_DCR_RATE			0x02
#define UAC_DCR_MAXAMPL			0x03
#define UAC_DCR_THRESHOLD		0x04
#define UAC_DCR_ATTACK_TIME		0x05
#define UAC_DCR_RELEASE_TIME		0x06

/** A.10.4 Extension Unit Control Selectors */
#define UAC_XU_ENABLE			0x01

/** MIDI - A.1 MS Class-Specific Interface Descriptor Subtypes */
#define UAC_MS_HEADER			0x01
#define UAC_MIDI_IN_JACK		0x02
#define UAC_MIDI_OUT_JACK		0x03

/** MIDI - A.1 MS Class-Specific Endpoint Descriptor Subtypes */
#define UAC_MS_GENERAL			0x01

/** Terminals - 2.1 USB Terminal Types */
#define UAC_TERMINAL_UNDEFINED		0x0100
#define UAC_TERMINAL_STREAMING		0x0101
#define UAC_TERMINAL_VENDOR_SPEC	0x01FF

/** Terminal Control Selectors */
/** 4.3.2  Class-Specific AC Interface Descriptor */
struct uac1_ac_header_descriptor {
    uByte  bLength;			/** 8 + n */
    uByte  bDescriptorType;		/** USB_DT_CS_INTERFACE */
    uByte  bDescriptorSubtype;	/** UAC_MS_HEADER */
    uWord  bcdADC;			/** 0x0100 */
    uWord  wTotalLength;		/** includes Unit and Terminal desc. */
    uByte  bInCollection;		/** n */
    uByte  baInterfaceNr[];		/** [n] */
} __attribute__((packed));

#define UAC_DT_AC_HEADER_SIZE(n)	(8 + (n))

#define UAC_AC_HEADER_DESCRIPTOR(n) \
	uac1_ac_header_descriptor_##n

/** As above, but more useful for defining your own descriptors: */
#define DECLARE_UAC_AC_HEADER_DESCRIPTOR(n)			\
    struct uac1_ac_header_descriptor_##n {			\
        uByte  bLength;						\
        uByte  bDescriptorType;					\
        uByte  bDescriptorSubtype;				\
        uWord  bcdADC;						\
        uWord  wTotalLength;					\
        uByte  bInCollection;					\
        uByte  baInterfaceNr[n];					\
    } __attribute__ ((packed))

/** 4.3.2.1 Input Terminal Descriptor */
struct uac_input_terminal_descriptor {
    uByte  bLength;			/** in bytes: 12 */
    uByte  bDescriptorType;		/** CS_INTERFACE descriptor type */
    uByte  bDescriptorSubtype;	/** INPUT_TERMINAL descriptor subtype */
    uByte  bTerminalID;		/** Constant uniquely terminal ID */
    uWord  wTerminalType;		/** USB Audio Terminal Types */
    uByte  bAssocTerminal;		/** ID of the Output Terminal associated */
    uByte  bNrChannels;		/** Number of logical output channels */
    uWord  wChannelConfig;
    uByte  iChannelNames;
    uByte  iTerminal;
} __attribute__((packed));

#define UAC_DT_INPUT_TERMINAL_SIZE			12

/** Terminals - 2.2 Input Terminal Types */
#define UAC_INPUT_TERMINAL_UNDEFINED			0x200
#define UAC_INPUT_TERMINAL_MICROPHONE			0x201
#define UAC_INPUT_TERMINAL_DESKTOP_MICROPHONE		0x202
#define UAC_INPUT_TERMINAL_PERSONAL_MICROPHONE		0x203
#define UAC_INPUT_TERMINAL_OMNI_DIR_MICROPHONE		0x204
#define UAC_INPUT_TERMINAL_MICROPHONE_ARRAY		0x205
#define UAC_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY	0x206

/** Terminals - control selectors */

#define UAC_TERMINAL_CS_COPY_PROTECT_CONTROL		0x01

/** 4.3.2.2 Output Terminal Descriptor */
struct uac1_output_terminal_descriptor {
    uByte  bLength;			/** in bytes: 9 */
    uByte  bDescriptorType;		/** CS_INTERFACE descriptor type */
    uByte  bDescriptorSubtype;	/** OUTPUT_TERMINAL descriptor subtype */
    uByte  bTerminalID;		/** Constant uniquely terminal ID */
    uWord  wTerminalType;		/** USB Audio Terminal Types */
    uByte  bAssocTerminal;		/** ID of the Input Terminal associated */
    uByte  bSourceID;		/** ID of the connected Unit or Terminal*/
    uByte  iTerminal;
} __attribute__((packed));

#define UAC_DT_OUTPUT_TERMINAL_SIZE			9

/** Terminals - 2.3 Output Terminal Types */
#define UAC_OUTPUT_TERMINAL_UNDEFINED			0x300
#define UAC_OUTPUT_TERMINAL_SPEAKER			0x301
#define UAC_OUTPUT_TERMINAL_HEADPHONES			0x302
#define UAC_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO	0x303
#define UAC_OUTPUT_TERMINAL_DESKTOP_SPEAKER		0x304
#define UAC_OUTPUT_TERMINAL_ROOM_SPEAKER		0x305
#define UAC_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER	0x306
#define UAC_OUTPUT_TERMINAL_LOW_FREQ_EFFECTS_SPEAKER	0x307
#define UAC_OUTPUT_TERMINAL_HEADSET     0x0402

/** Set bControlSize = 1 as default setting */
#define UAC_DT_FEATURE_UNIT_SIZE(ch)		(7 + ((ch) + 1))

#define UAC_FEATURE_UNIT_DESCRIPTOR(ch) \
	uac_feature_unit_descriptor_##ch

/** As above, but more useful for defining your own descriptors: */
#define DECLARE_UAC_FEATURE_UNIT_DESCRIPTOR(ch)			\
    struct uac_feature_unit_descriptor_##ch {			\
        uByte  bLength;						\
        uByte  bDescriptorType;					\
        uByte  bDescriptorSubtype;				\
        uByte  bUnitID;						\
        uByte  bSourceID;					\
        uByte  bControlSize;					\
        uByte  bmaControls[ch + 1];				\
        uByte  iFeature;						\
    } __attribute__ ((packed))

/** 4.3.2.3 Mixer Unit Descriptor */
struct uac_mixer_unit_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bUnitID;
    uByte bNrInPins;
    uByte baSourceID[];
} __attribute__((packed));



/** 4.3.2.4 Selector Unit Descriptor */
struct uac_selector_unit_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bUintID;
    uByte bNrInPins;
    uByte baSourceID[];
	// uByte iSelector;
} __attribute__((packed));

/** Set bControlSize = 1 as default setting */
#define UAC_DT_SELECTOR_UNIT_SIZE(p)		(6 + (p))

#define UAC_SELECTOR_UNIT_DESCRIPTOR(p) \
	uac_selector_unit_descriptor_##p

/** As above, but more useful for defining your own descriptors: */
#define DECLARE_UAC_SELECTOR_UNIT_DESCRIPTOR(p)			\
    struct uac_selector_unit_descriptor_##p {			\
        uByte bLength;                     \
        uByte bDescriptorType;             \
        uByte bDescriptorSubtype;          \
        uByte bUintID;                     \
        uByte bNrInPins;                   \
        uByte baSourceID[p];                \
	    uByte iSelector;				   \
    } __attribute__ ((packed))

/** 4.3.2.5 Feature Unit Descriptor */
struct uac_feature_unit_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bUnitID;
    uByte bSourceID;
    uByte bControlSize;
    uByte bmaControls[0]; /** variable length */
} __attribute__((packed));



/** 4.3.2.6 Processing Unit Descriptors */
struct uac_processing_unit_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bUnitID;
    uWord wProcessType;
    uByte bNrInPins;
    uByte baSourceID[];
} __attribute__((packed));




/** 4.5.2 Class-Specific AS Interface Descriptor */
struct uac1_as_header_descriptor {
    uByte  bLength;			/** in bytes: 7 */
    uByte  bDescriptorType;		/** USB_DT_CS_INTERFACE */
    uByte  bDescriptorSubtype;	/** AS_GENERAL */
    uByte  bTerminalLink;		/** Terminal ID of connected Terminal */
    uByte  bDelay;			/** Delay introduced by the data path */
    uWord  wFormatTag;		/** The Audio Data Format */
} __attribute__((packed));

#define UAC_DT_AS_HEADER_SIZE		7

/** Formats - A.1.1 Audio Data Format Type I Codes */
#define UAC_FORMAT_TYPE_I_UNDEFINED	0x0
#define UAC_FORMAT_TYPE_I_PCM		0x1
#define UAC_FORMAT_TYPE_I_PCM8		0x2
#define UAC_FORMAT_TYPE_I_IEEE_FLOAT	0x3
#define UAC_FORMAT_TYPE_I_ALAW		0x4
#define UAC_FORMAT_TYPE_I_MULAW		0x5

struct uac_format_type_i_continuous_descriptor {
    uByte  bLength;			/** in bytes: 8 + (ns * 3) */
    uByte  bDescriptorType;		/** USB_DT_CS_INTERFACE */
    uByte  bDescriptorSubtype;	/** FORMAT_TYPE */
    uByte  bFormatType;		/** FORMAT_TYPE_1 */
    uByte  bNrChannels;		/** physical channels in the stream */
    uByte  bSubframeSize;		/** */
    uByte  bBitResolution;
    uByte  bSamFreqType;
    uByte  tLowerSamFreq[3];
    uByte  tUpperSamFreq[3];
} __attribute__((packed));

#define UAC_FORMAT_TYPE_I_CONTINUOUS_DESC_SIZE	14

struct uac_format_type_i_discrete_descriptor {
    uByte  bLength;			/** in bytes: 8 + (ns * 3) */
    uByte  bDescriptorType;		/** USB_DT_CS_INTERFACE */
    uByte  bDescriptorSubtype;	/** FORMAT_TYPE */
    uByte  bFormatType;		/** FORMAT_TYPE_1 */
    uByte  bNrChannels;		/** physical channels in the stream */
    uByte  bSubframeSize;		/** */
    uByte  bBitResolution;
    uByte  bSamFreqType;
    uByte  tSamFreq[][3];
} __attribute__((packed));

#define UAC_FORMAT_TYPE_I_DISCRETE_DESC_SIZE(n)	(8 + (n * 3))

#define UAC_FORMAT_TYPE_I_DISCRETE_DESCRIPTOR(n) \
	uac_format_type_i_discrete_descriptor_##n

#define DECLARE_UAC_FORMAT_TYPE_I_DISCRETE_DESC(n)		\
    struct uac_format_type_i_discrete_descriptor_##n {		\
        uByte  bLength;						\
        uByte  bDescriptorType;					\
        uByte  bDescriptorSubtype;				\
        uByte  bFormatType;					\
        uByte  bNrChannels;					\
        uByte  bSubframeSize;					\
        uByte  bBitResolution;					\
        uByte  bSamFreqType;					\
        uByte  tSamFreq[n][3];					\
    } __attribute__ ((packed))

struct uac_format_type_i_ext_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bFormatType;
    uByte bSubslotSize;
    uByte bBitResolution;
    uByte bHeaderLength;
    uByte bControlSize;
    uByte bSideBandProtocol;
} __attribute__((packed));

/** Formats - Audio Data Format Type I Codes */

#define UAC_FORMAT_TYPE_II_MPEG	0x1001
#define UAC_FORMAT_TYPE_II_AC3	0x1002

struct uac_format_type_ii_discrete_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bFormatType;
    uWord  wMaxBitRate;
    uWord  wSamplesPerFrame;
    uByte bSamFreqType;
    uByte tSamFreq[][3];
} __attribute__((packed));

struct uac_format_type_ii_ext_descriptor {
    uByte bLength;
    uByte bDescriptorType;
    uByte bDescriptorSubtype;
    uByte bFormatType;
    uWord wMaxBitRate;
    uWord wSamplesPerFrame;
    uByte bHeaderLength;
    uByte bSideBandProtocol;
} __attribute__((packed));

/** type III */
#define UAC_FORMAT_TYPE_III_IEC1937_AC3	0x2001
#define UAC_FORMAT_TYPE_III_IEC1937_MPEG1_LAYER1	0x2002
#define UAC_FORMAT_TYPE_III_IEC1937_MPEG2_NOEXT	0x2003
#define UAC_FORMAT_TYPE_III_IEC1937_MPEG2_EXT	0x2004
#define UAC_FORMAT_TYPE_III_IEC1937_MPEG2_LAYER1_LS	0x2005
#define UAC_FORMAT_TYPE_III_IEC1937_MPEG2_LAYER23_LS	0x2006

/** Formats - A.2 Format Type Codes */
#define UAC_FORMAT_TYPE_UNDEFINED	0x0
#define UAC_FORMAT_TYPE_I		0x1
#define UAC_FORMAT_TYPE_II		0x2
#define UAC_FORMAT_TYPE_III		0x3
#define UAC_EXT_FORMAT_TYPE_I		0x81
#define UAC_EXT_FORMAT_TYPE_II		0x82
#define UAC_EXT_FORMAT_TYPE_III		0x83

struct uac_standard_iso_endpoint_descriptor {
    uByte  bLength;
    uByte  bDescriptorType;
    uByte  bEndpointAddress;
    uByte  bmAttributes;
    uWord  wMaxPacketSize;
    uByte  bInterval;
    uByte  bRefresh;
    uByte  bSynchAddress;
} __attribute__((packed));
#define UAC_STANDARD_ISO_ENDPOINT_DESC_SIZE	9

struct uac_iso_endpoint_descriptor {
    uByte  bLength;			/** in bytes: 7 */
    uByte  bDescriptorType;		/** USB_DT_CS_ENDPOINT */
    uByte  bDescriptorSubtype;	/** EP_GENERAL */
    uByte  bmAttributes;
    uByte  bLockDelayUnits;
    uWord  wLockDelay;
} __attribute__((packed));
#define UAC_ISO_ENDPOINT_DESC_SIZE	7

#define UAC_EP_CS_ATTR_SAMPLE_RATE	0x01
#define UAC_EP_CS_ATTR_PITCH_CONTROL	0x02
#define UAC_EP_CS_ATTR_FILL_MAX		0x80

/** status word format (3.7.1.1) */

#define UAC1_STATUS_TYPE_ORIG_MASK		0x0f
#define UAC1_STATUS_TYPE_ORIG_AUDIO_CONTROL_IF	0x0
#define UAC1_STATUS_TYPE_ORIG_AUDIO_STREAM_IF	0x1
#define UAC1_STATUS_TYPE_ORIG_AUDIO_STREAM_EP	0x2

#define UAC1_STATUS_TYPE_IRQ_PENDING		(1 << 7)
#define UAC1_STATUS_TYPE_MEM_CHANGED		(1 << 6)

struct uac1_status_word {
    uByte bStatusType;
    uByte bOriginator;
} __attribute__((packed));


typedef enum {
    UAC_PCM = 0,
    UAC_MAX,
} uac_fmt_t;

typedef void (*uac_spk_user_start_func_t)(void);
typedef void (*uac_spk_user_stop_func_t)(void);

typedef void (*uac_mic_user_start_func_t)(void);
typedef void (*uac_mic_user_stop_func_t)(void);

typedef struct _uac_func_op_t {
    int  (*uac_mic_commit_frame)(audio_frame_buf_t *cur);
    void (*uac_mic_register_start_cb)(uac_mic_user_start_func_t func);
    void (*uac_mic_register_stop_cb)(uac_mic_user_stop_func_t func);
} uac_func_op_t;

int  uac_init(const char *dev_name);
int  uac_cleanup(void);

int  uac_mic_commit_frame(audio_frame_buf_t *cur);
void uac_mic_register_user_start_cb(uac_mic_user_start_func_t func);
void uac_mic_register_user_stop_cb(uac_mic_user_stop_func_t func);

#ifdef __cplusplus
}
#endif

#endif // __UAC_COMM_H__
