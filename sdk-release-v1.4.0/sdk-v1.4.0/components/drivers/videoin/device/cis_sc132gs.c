#include <math.h>
#include "cis_sc132gs.h"
#include "aiva_utils.h"
#include "aiva_sleep.h"
#include "sysctl.h"
#include "i2c.h"
#include "gpio.h"
#include "syslog.h"
#include "udevice.h"

#define SC132GS_REG_CTRL_MODE       0x0100
#define CHIP_ID                     0x0132
#define SC132GS_REG_CHIP_ID         0x3107

#define REG_DLY                    (0xffff)
#define SENSOR_ADDR_WR             (0x30)

/*
曝光时间下限为 0 行，上限为帧长减去 8 行，
帧长=寄存器{16‘h320e,16’h320f}的值， 
即 写 入 的 {16’h3e00[3:0],16’h3e01,16’h3e02} 值 下 限 为 0 ， 
上 限 为({16’h320e,16’h320f}-8)*16
*/
#define MAX_EXPOSURE              (0x0960 - 8)     // unit: lines
#define DEFAULT_EXPOSURE          (1342)


typedef struct _SC132GS_REG_T {
    uint16_t        reg_addr;
    uint8_t         data;
} SC132GS_REG_T;

typedef struct _SC132GS_GAIN_T {
    uint8_t coarse;
    uint8_t fine;
    float   gain;
} SC132GS_GAIN_T;

static const char* TAG = "cis_sc132gs";

static SC132GS_REG_T sc132gs_start_regs[] __ATTR_ALIGN__(32) = {
    {SC132GS_REG_CTRL_MODE, 0x01},
};

static SC132GS_REG_T sc132gs_stop_regs[] __ATTR_ALIGN__(32) = {
    {SC132GS_REG_CTRL_MODE, 0x00},
};

// 0.31 step again table, 1.0 ~ 28.547, 154 steps
static const SC132GS_GAIN_T sc132gs_again_table[] = {
    {0x03, 0x20,  1.000},
    {0x03, 0x21,  1.031},
    {0x03, 0x22,  1.063},
    {0x03, 0x23,  1.094},
    {0x03, 0x24,  1.125},
    {0x03, 0x25,  1.156},
    {0x03, 0x26,  1.188},
    {0x03, 0x27,  1.219},
    {0x03, 0x28,  1.250},
    {0x03, 0x29,  1.281},
    {0x03, 0x2A,  1.313},
    {0x03, 0x2B,  1.344},
    {0x03, 0x2C,  1.375},
    {0x03, 0x2D,  1.406},
    {0x03, 0x2E,  1.438},
    {0x03, 0x2F,  1.469},
    {0x03, 0x30,  1.500},
    {0x03, 0x31,  1.531},
    {0x03, 0x32,  1.563},
    {0x03, 0x33,  1.594},
    {0x03, 0x34,  1.625},
    {0x03, 0x35,  1.656},
    {0x03, 0x36,  1.688},
    {0x03, 0x37,  1.719},
    {0x03, 0x38,  1.750},
    {0x03, 0x39,  1.781},
    {0x23, 0x20,  1.813},
    {0x23, 0x21,  1.869},
    {0x23, 0x22,  1.926},
    {0x23, 0x23,  1.982},
    {0x23, 0x24,  2.039},
    {0x23, 0x25,  2.096},
    {0x23, 0x26,  2.152},
    {0x23, 0x27,  2.209},
    {0x23, 0x28,  2.266},
    {0x23, 0x29,  2.322},
    {0x23, 0x2A,  2.379},
    {0x23, 0x2B,  2.436},
    {0x23, 0x2C,  2.492},
    {0x23, 0x2D,  2.549},
    {0x23, 0x2E,  2.605},
    {0x23, 0x2F,  2.662},
    {0x23, 0x30,  2.719},
    {0x23, 0x31,  2.775},
    {0x23, 0x32,  2.832},
    {0x23, 0x33,  2.889},
    {0x23, 0x34,  2.945},
    {0x23, 0x35,  3.002},
    {0x23, 0x36,  3.059},
    {0x23, 0x37,  3.115},
    {0x23, 0x38,  3.172},
    {0x23, 0x39,  3.229},
    {0x23, 0x3A,  3.285},
    {0x23, 0x3B,  3.342},
    {0x23, 0x3C,  3.398},
    {0x23, 0x3D,  3.455},
    {0x23, 0x3E,  3.512},
    {0x23, 0x3F,  3.568},
    {0x27, 0x20,  3.625},
    {0x27, 0x21,  3.738},
    {0x27, 0x22,  3.852},
    {0x27, 0x23,  3.965},
    {0x27, 0x24,  4.078},
    {0x27, 0x25,  4.191},
    {0x27, 0x26,  4.305},
    {0x27, 0x27,  4.418},
    {0x27, 0x28,  4.531},
    {0x27, 0x29,  4.645},
    {0x27, 0x2A,  4.758},
    {0x27, 0x2B,  4.871},
    {0x27, 0x2C,  4.984},
    {0x27, 0x2D,  5.098},
    {0x27, 0x2E,  5.211},
    {0x27, 0x2F,  5.324},
    {0x27, 0x30,  5.438},
    {0x27, 0x31,  5.551},
    {0x27, 0x32,  5.664},
    {0x27, 0x33,  5.777},
    {0x27, 0x34,  5.891},
    {0x27, 0x35,  6.004},
    {0x27, 0x36,  6.117},
    {0x27, 0x37,  6.230},
    {0x27, 0x38,  6.344},
    {0x27, 0x39,  6.457},
    {0x27, 0x3A,  6.570},
    {0x27, 0x3B,  6.684},
    {0x27, 0x3C,  6.797},
    {0x27, 0x3D,  6.910},
    {0x27, 0x3E,  7.023},
    {0x27, 0x3F,  7.137},
    {0x2F, 0x20,  7.250},
    {0x2F, 0x21,  7.477},
    {0x2F, 0x22,  7.703},
    {0x2F, 0x23,  7.930},
    {0x2F, 0x24,  8.156},
    {0x2F, 0x25,  8.383},
    {0x2F, 0x26,  8.609},
    {0x2F, 0x27,  8.836},
    {0x2F, 0x28,  9.063},
    {0x2F, 0x29,  9.289},
    {0x2F, 0x2A,  9.516},
    {0x2F, 0x2B,  9.742},
    {0x2F, 0x2C,  9.969},
    {0x2F, 0x2D, 10.195},
    {0x2F, 0x2E, 10.422},
    {0x2F, 0x2F, 10.648},
    {0x2F, 0x30, 10.875},
    {0x2F, 0x31, 11.102},
    {0x2F, 0x32, 11.328},
    {0x2F, 0x33, 11.555},
    {0x2F, 0x34, 11.781},
    {0x2F, 0x35, 12.008},
    {0x2F, 0x36, 12.234},
    {0x2F, 0x37, 12.461},
    {0x2F, 0x38, 12.688},
    {0x2F, 0x39, 12.914},
    {0x2F, 0x3A, 13.141},
    {0x2F, 0x3B, 13.367},
    {0x2F, 0x3C, 13.594},
    {0x2F, 0x3D, 13.820},
    {0x2F, 0x3E, 14.047},
    {0x2F, 0x3F, 14.273},
    {0x3F, 0x20, 14.500},
    {0x3F, 0x21, 14.953},
    {0x3F, 0x22, 15.406},
    {0x3F, 0x23, 15.859},
    {0x3F, 0x24, 16.313},
    {0x3F, 0x25, 16.766},
    {0x3F, 0x26, 17.219},
    {0x3F, 0x27, 17.672},
    {0x3F, 0x28, 18.125},
    {0x3F, 0x29, 18.578},
    {0x3F, 0x2A, 19.031},
    {0x3F, 0x2B, 19.484},
    {0x3F, 0x2C, 19.938},
    {0x3F, 0x2D, 20.391},
    {0x3F, 0x2E, 20.844},
    {0x3F, 0x2F, 21.297},
    {0x3F, 0x30, 21.750},
    {0x3F, 0x31, 22.203},
    {0x3F, 0x32, 22.656},
    {0x3F, 0x33, 23.109},
    {0x3F, 0x34, 23.563},
    {0x3F, 0x35, 24.016},
    {0x3F, 0x36, 24.469},
    {0x3F, 0x37, 24.922},
    {0x3F, 0x38, 25.375},
    {0x3F, 0x39, 25.828},
    {0x3F, 0x3A, 26.281},
    {0x3F, 0x3B, 26.734},
    {0x3F, 0x3C, 27.188},
    {0x3F, 0x3D, 27.641},
    {0x3F, 0x3E, 28.094},
    {0x3F, 0x3F, 28.547},
};

// 0.31 step dgain table, 1.0 ~ 31.5, 160 step
static const SC132GS_GAIN_T sc132gs_dgain_table[] = {
    {0x00, 0x80,  1.000},
    {0x00, 0x84,  1.031},
    {0x00, 0x88,  1.063},
    {0x00, 0x8C,  1.094},
    {0x00, 0x90,  1.125},
    {0x00, 0x94,  1.156},
    {0x00, 0x98,  1.188},
    {0x00, 0x9C,  1.219},
    {0x00, 0xA0,  1.250},
    {0x00, 0xA4,  1.281},
    {0x00, 0xA8,  1.313},
    {0x00, 0xAC,  1.344},
    {0x00, 0xB0,  1.375},
    {0x00, 0xB4,  1.406},
    {0x00, 0xB8,  1.438},
    {0x00, 0xBC,  1.469},
    {0x00, 0xC0,  1.500},
    {0x00, 0xC4,  1.531},
    {0x00, 0xC8,  1.563},
    {0x00, 0xCC,  1.594},
    {0x00, 0xD0,  1.625},
    {0x00, 0xD4,  1.656},
    {0x00, 0xD8,  1.688},
    {0x00, 0xDC,  1.719},
    {0x00, 0xE0,  1.750},
    {0x00, 0xE4,  1.781},
    {0x00, 0xE8,  1.813},
    {0x00, 0xEC,  1.844},
    {0x00, 0xF0,  1.875},
    {0x00, 0xF4,  1.906},
    {0x00, 0xF8,  1.938},
    {0x00, 0xFC,  1.969},
    {0x01, 0x80,  2.000},
    {0x01, 0x84,  2.063},
    {0x01, 0x88,  2.125},
    {0x01, 0x8C,  2.188},
    {0x01, 0x90,  2.250},
    {0x01, 0x94,  2.313},
    {0x01, 0x98,  2.375},
    {0x01, 0x9C,  2.438},
    {0x01, 0xA0,  2.500},
    {0x01, 0xA4,  2.563},
    {0x01, 0xA8,  2.625},
    {0x01, 0xAC,  2.688},
    {0x01, 0xB0,  2.750},
    {0x01, 0xB4,  2.813},
    {0x01, 0xB8,  2.875},
    {0x01, 0xBC,  2.938},
    {0x01, 0xC0,  3.000},
    {0x01, 0xC4,  3.063},
    {0x01, 0xC8,  3.125},
    {0x01, 0xCC,  3.188},
    {0x01, 0xD0,  3.250},
    {0x01, 0xD4,  3.313},
    {0x01, 0xD8,  3.375},
    {0x01, 0xDC,  3.438},
    {0x01, 0xE0,  3.500},
    {0x01, 0xE4,  3.563},
    {0x01, 0xE8,  3.625},
    {0x01, 0xEC,  3.688},
    {0x01, 0xF0,  3.750},
    {0x01, 0xF4,  3.813},
    {0x01, 0xF8,  3.875},
    {0x01, 0xFC,  3.938},
    {0x03, 0x80,  4.000},
    {0x03, 0x84,  4.125},
    {0x03, 0x88,  4.250},
    {0x03, 0x8C,  4.375},
    {0x03, 0x90,  4.500},
    {0x03, 0x94,  4.625},
    {0x03, 0x98,  4.750},
    {0x03, 0x9C,  4.875},
    {0x03, 0xA0,  5.000},
    {0x03, 0xA4,  5.125},
    {0x03, 0xA8,  5.250},
    {0x03, 0xAC,  5.375},
    {0x03, 0xB0,  5.500},
    {0x03, 0xB4,  5.625},
    {0x03, 0xB8,  5.750},
    {0x03, 0xBC,  5.875},
    {0x03, 0xC0,  6.000},
    {0x03, 0xC4,  6.125},
    {0x03, 0xC8,  6.250},
    {0x03, 0xCC,  6.375},
    {0x03, 0xD0,  6.500},
    {0x03, 0xD4,  6.625},
    {0x03, 0xD8,  6.750},
    {0x03, 0xDC,  6.875},
    {0x03, 0xE0,  7.000},
    {0x03, 0xE4,  7.125},
    {0x03, 0xE8,  7.250},
    {0x03, 0xEC,  7.375},
    {0x03, 0xF0,  7.500},
    {0x03, 0xF4,  7.625},
    {0x03, 0xF8,  7.750},
    {0x03, 0xFC,  7.875},
    {0x07, 0x80,  8.000},
    {0x07, 0x84,  8.250},
    {0x07, 0x88,  8.500},
    {0x07, 0x8C,  8.750},
    {0x07, 0x90,  9.000},
    {0x07, 0x94,  9.250},
    {0x07, 0x98,  9.500},
    {0x07, 0x9C,  9.750},
    {0x07, 0xA0, 10.000},
    {0x07, 0xA4, 10.250},
    {0x07, 0xA8, 10.500},
    {0x07, 0xAC, 10.750},
    {0x07, 0xB0, 11.000},
    {0x07, 0xB4, 11.250},
    {0x07, 0xB8, 11.500},
    {0x07, 0xBC, 11.750},
    {0x07, 0xC0, 12.000},
    {0x07, 0xC4, 12.250},
    {0x07, 0xC8, 12.500},
    {0x07, 0xCC, 12.750},
    {0x07, 0xD0, 13.000},
    {0x07, 0xD4, 13.250},
    {0x07, 0xD8, 13.500},
    {0x07, 0xDC, 13.750},
    {0x07, 0xE0, 14.000},
    {0x07, 0xE4, 14.250},
    {0x07, 0xE8, 14.500},
    {0x07, 0xEC, 14.750},
    {0x07, 0xF0, 15.000},
    {0x07, 0xF4, 15.250},
    {0x07, 0xF8, 15.500},
    {0x07, 0xFC, 15.750},
    {0x0F, 0x80, 16.000},
    {0x0F, 0x84, 16.500},
    {0x0F, 0x88, 17.000},
    {0x0F, 0x8C, 17.500},
    {0x0F, 0x90, 18.000},
    {0x0F, 0x94, 18.500},
    {0x0F, 0x98, 19.000},
    {0x0F, 0x9C, 19.500},
    {0x0F, 0xA0, 20.000},
    {0x0F, 0xA4, 20.500},
    {0x0F, 0xA8, 21.000},
    {0x0F, 0xAC, 21.500},
    {0x0F, 0xB0, 22.000},
    {0x0F, 0xB4, 22.500},
    {0x0F, 0xB8, 23.000},
    {0x0F, 0xBC, 23.500},
    {0x0F, 0xC0, 24.000},
    {0x0F, 0xC4, 24.500},
    {0x0F, 0xC8, 25.000},
    {0x0F, 0xCC, 25.500},
    {0x0F, 0xD0, 26.000},
    {0x0F, 0xD4, 26.500},
    {0x0F, 0xD8, 27.000},
    {0x0F, 0xDC, 27.500},
    {0x0F, 0xE0, 28.000},
    {0x0F, 0xE4, 28.500},
    {0x0F, 0xE8, 29.000},
    {0x0F, 0xEC, 29.500},
    {0x0F, 0xF0, 30.000},
    {0x0F, 0xF4, 30.500},
    {0x0F, 0xF8, 31.000},
    {0x0F, 0xFC, 31.500},
};

// 24MHz input, 480Mbps, 2 lane, 10bit, 1080x1280@30fps
static const SC132GS_REG_T sc132gs_setting_2lane[] __ATTR_ALIGN__(32) = {
    {0x0103, 0x01},
    {0x0100, 0x00},
    {0x36e9, 0x80},
    {0x36f9, 0x80},
    {0x3018, 0x32},
    {0x3019, 0x0c},
    {0x301a, 0xb4},
    {0x3031, 0x0a},
    {0x3032, 0x60},
    {0x3038, 0x44},
    {0x3207, 0x17},
    {0x320c, 0x05},
    {0x320d, 0xdc},
    {0x320e, 0x09},
    {0x320f, 0x60},
    {0x3250, 0xcc},
    {0x3251, 0x02},
    {0x3252, 0x09},
    {0x3253, 0x5b},
    {0x3254, 0x05},
    {0x3255, 0x3b},
    {0x3306, 0x78},
    {0x330a, 0x00},
    {0x330b, 0xc8},
    {0x330f, 0x24},
    {0x3314, 0x80},
    {0x3315, 0x40},
    {0x3317, 0xf0},
    {0x331f, 0x12},
    {0x3364, 0x00},
    {0x3385, 0x41},
    {0x3387, 0x41},
    {0x3389, 0x09},
    {0x33ab, 0x00},
    {0x33ac, 0x00},
    {0x33b1, 0x03},
    {0x33b2, 0x12},
    {0x33f8, 0x02},
    {0x33fa, 0x01},
    {0x3409, 0x08},
    {0x34f0, 0xc0},
    {0x34f1, 0x20},
    {0x34f2, 0x03},
    {0x3622, 0xf5},
    {0x3630, 0x5c},
    {0x3631, 0x80},
    {0x3632, 0xc8},
    {0x3633, 0x32},
    {0x3638, 0x2a},
    {0x3639, 0x07},
    {0x363b, 0x48},
    {0x363c, 0x83},
    {0x363d, 0x10},
    {0x36ea, 0x38},
    {0x36fa, 0x25},
    {0x36fb, 0x05},
    {0x36fd, 0x04},
    {0x3900, 0x11},
    {0x3901, 0x05},
    {0x3902, 0xc5},
    {0x3904, 0x04},
    {0x3908, 0x91},
    {0x391e, 0x00},
    {0x3e01, 0x53},
    {0x3e02, 0xe0},
    {0x3e09, 0x20},
    {0x3e0e, 0xd2},
    {0x3e14, 0xb0},
    {0x3e1e, 0x7c},
    {0x3e26, 0x20},
    {0x4418, 0x38},
    {0x4503, 0x10},
    {0x4837, 0x21},
    {0x5000, 0x0e},
    {0x540c, 0x51},
    {0x550f, 0x38},
    {0x5780, 0x67},
    {0x5784, 0x10},
    {0x5785, 0x06},
    {0x5787, 0x02},
    {0x5788, 0x00},
    {0x5789, 0x00},
    {0x578a, 0x02},
    {0x578b, 0x00},
    {0x578c, 0x00},
    {0x5790, 0x00},
    {0x5791, 0x00},
    {0x5792, 0x00},
    {0x5793, 0x00},
    {0x5794, 0x00},
    {0x5795, 0x00},
    {0x5799, 0x04},
    {0x36e9, 0x20},
    {0x36f9, 0x24},

    {0x3e03, 0x0b}, // gain mode: look up table
    {0x3e08, 0x23}, // again 3.000
    {0x3e09, 0x35}, 
    {0x3e06, 0x00}, // dgain 1.000
    {0x3e07, 0x80}, 

    {0x3361, 0x0}, // strobe enable

#if 0
    // gain<2
    {0x33f8, 0x02},
    {0x3314, 0x80},
    {0x33fa, 0x01},
    {0x3317, 0xf0},
#endif

    // gain>=2
    {0x33f8, 0x02},
    {0x3314, 0x80},
    {0x33fa, 0x02},
    {0x3317, 0x0a},
};

// 24MHz input, 720Mbps, 1 lane, 10bit, 1080x1280@30fps
static const SC132GS_REG_T sc132gs_setting_1lane[] __ATTR_ALIGN__(32) = {
    {0x0103, 0x01},
    {0x0100, 0x00},
    {0x36e9, 0x80},
    {0x36f9, 0x80},
    {0x3018, 0x12},
    {0x3019, 0x0e},
    {0x301a, 0xb4},
    {0x3031, 0x0a},
    {0x3032, 0x60},
    {0x3038, 0x44},
    {0x3207, 0x17},
    {0x320c, 0x06},
    {0x320d, 0xa0},
    {0x320e, 0x08},
    {0x320f, 0x4a},
    {0x3250, 0xcc},
    {0x3251, 0x02},
    {0x3252, 0x08},
    {0x3253, 0x45},
    {0x3254, 0x05},
    {0x3255, 0x3b},
    {0x3306, 0x78},
    {0x330a, 0x00},
    {0x330b, 0xc8},
    {0x330f, 0x24},
    {0x3314, 0x80},
    {0x3315, 0x40},
    {0x3317, 0xf0},
    {0x331f, 0x12},
    {0x3364, 0x00},
    {0x3385, 0x41},
    {0x3387, 0x41},
    {0x3389, 0x09},
    {0x33ab, 0x00},
    {0x33ac, 0x00},
    {0x33b1, 0x03},
    {0x33b2, 0x12},
    {0x33f8, 0x02},
    {0x33fa, 0x01},
    {0x3409, 0x08},
    {0x34f0, 0xc0},
    {0x34f1, 0x20},
    {0x34f2, 0x03},
    {0x3622, 0xf5},
    {0x3630, 0x5c},
    {0x3631, 0x80},
    {0x3632, 0xc8},
    {0x3633, 0x32},
    {0x3638, 0x2a},
    {0x3639, 0x07},
    {0x363b, 0x48},
    {0x363c, 0x83},
    {0x363d, 0x10},
    {0x36ea, 0x3a},
    {0x36fa, 0x25},
    {0x36fb, 0x05},
    {0x36fd, 0x04},
    {0x3900, 0x11},
    {0x3901, 0x05},
    {0x3902, 0xc5},
    {0x3904, 0x04},
    {0x3908, 0x91},
    {0x391e, 0x00},
    {0x3e01, 0x53},
    {0x3e02, 0xe0},
    {0x3e09, 0x20},
    {0x3e0e, 0xd2},
    {0x3e14, 0xb0},
    {0x3e1e, 0x7c},
    {0x3e26, 0x20},
    {0x4418, 0x38},
    {0x4503, 0x10},
    {0x4837, 0x14},
    {0x5000, 0x0e},
    {0x540c, 0x51},
    {0x550f, 0x38},
    {0x5780, 0x67},
    {0x5784, 0x10},
    {0x5785, 0x06},
    {0x5787, 0x02},
    {0x5788, 0x00},
    {0x5789, 0x00},
    {0x578a, 0x02},
    {0x578b, 0x00},
    {0x578c, 0x00},
    {0x5790, 0x00},
    {0x5791, 0x00},
    {0x5792, 0x00},
    {0x5793, 0x00},
    {0x5794, 0x00},
    {0x5795, 0x00},
    {0x5799, 0x04},
    {0x36e9, 0x24},
    {0x36f9, 0x24},

    {0x3e03, 0x0b}, // gain mode: look up table
    {0x3e08, 0x23}, // again 3.000
    {0x3e09, 0x35}, 
    {0x3e06, 0x00}, // dgain 1.000
    {0x3e07, 0x80}, 

    {0x3361, 0x0}, // strobe enable

#if 0
    // gain<2
    {0x33f8, 0x02},
    {0x3314, 0x80},
    {0x33fa, 0x01},
    {0x3317, 0xf0},
#endif

    // gain>=2
    {0x33f8, 0x02},
    {0x3314, 0x80},
    {0x33fa, 0x02},
    {0x3317, 0x0a},    
};

// 24MHz input, 720Mbps, 1 lane, 10bit, 1080x1280@30fps
static const SC132GS_REG_T sc132gs_setting_1lane_master[] __ATTR_ALIGN__(32) = {
    {0x0103, 0x01},
    {0x0100, 0x00},
    {0x36e9, 0x80},
    {0x36f9, 0x80},
    {0x3018, 0x12},
    {0x3019, 0x0e},
    {0x301a, 0xb4},
    {0x3031, 0x0a},
    {0x3032, 0x60},
    {0x3038, 0x44},
    {0x3207, 0x17},
    {0x320c, 0x06},
    {0x320d, 0xa0},
    {0x320e, 0x08},
    {0x320f, 0x4a},
    {0x3250, 0xcc},
    {0x3251, 0x02},
    {0x3252, 0x08},
    {0x3253, 0x45},
    {0x3254, 0x05},
    {0x3255, 0x3b},
    {0x3306, 0x78},
    {0x330a, 0x00},
    {0x330b, 0xc8},
    {0x330f, 0x24},
    {0x3314, 0x80},
    {0x3315, 0x40},
    {0x3317, 0xf0},
    {0x331f, 0x12},
    {0x3364, 0x00},
    {0x3385, 0x41},
    {0x3387, 0x41},
    {0x3389, 0x09},
    {0x33ab, 0x00},
    {0x33ac, 0x00},
    {0x33b1, 0x03},
    {0x33b2, 0x12},
    {0x33f8, 0x02},
    {0x33fa, 0x01},
    {0x3409, 0x08},
    {0x34f0, 0xc0},
    {0x34f1, 0x20},
    {0x34f2, 0x03},
    {0x3622, 0xf5},
    {0x3630, 0x5c},
    {0x3631, 0x80},
    {0x3632, 0xc8},
    {0x3633, 0x32},
    {0x3638, 0x2a},
    {0x3639, 0x07},
    {0x363b, 0x48},
    {0x363c, 0x83},
    {0x363d, 0x10},
    {0x36ea, 0x3a},
    {0x36fa, 0x25},
    {0x36fb, 0x05},
    {0x36fd, 0x04},
    {0x3900, 0x11},
    {0x3901, 0x05},
    {0x3902, 0xc5},
    {0x3904, 0x04},
    {0x3908, 0x91},
    {0x391e, 0x00},
    {0x3e01, 0x53},
    {0x3e02, 0xe0},
    {0x3e09, 0x20},
    {0x3e0e, 0xd2},
    {0x3e14, 0xb0},
    {0x3e1e, 0x7c},
    {0x3e26, 0x20},
    {0x4418, 0x38},
    {0x4503, 0x10},
    {0x4837, 0x14},
    {0x5000, 0x0e},
    {0x540c, 0x51},
    {0x550f, 0x38},
    {0x5780, 0x67},
    {0x5784, 0x10},
    {0x5785, 0x06},
    {0x5787, 0x02},
    {0x5788, 0x00},
    {0x5789, 0x00},
    {0x578a, 0x02},
    {0x578b, 0x00},
    {0x578c, 0x00},
    {0x5790, 0x00},
    {0x5791, 0x00},
    {0x5792, 0x00},
    {0x5793, 0x00},
    {0x5794, 0x00},
    {0x5795, 0x00},
    {0x5799, 0x04},
    {0x36e9, 0x24},
    {0x36f9, 0x24},
    {0x0100, 0x01},

    {0x3e03, 0x0b}, // gain mode: look up table
    {0x3e08, 0x23}, // again 3.000
    {0x3e09, 0x35}, 
    {0x3e06, 0x00}, // dgain 1.000
    {0x3e07, 0x80}, 

    // {0x3361, 0x0}, // strobe enable

#if 0
    // gain<2
    {0x33f8, 0x02},
    {0x3314, 0x80},
    {0x33fa, 0x01},
    {0x3317, 0xf0},
#endif

    // gain>=2
    // {0x33f8, 0x02},
    // {0x3314, 0x80},
    {0x33fa, 0x02},
    {0x3317, 0x00}, 

    // // master fsync
    {0x300a, 0x64},
    {0x3032, 0xa0},
    {0x3217, 0x08},
    {0x3218, 0x44},

};

// 24MHz input, 720Mbps, 1 lane, 10bit, 1080x1280@30fps
static const SC132GS_REG_T sc132gs_setting_1lane_slave[] __ATTR_ALIGN__(32) = {
    {0x0103,0x01},
    {0x0100,0x00},
    {0x36e9,0x80},
    {0x36f9,0x80},
    {0x3018,0x12},
    {0x3019,0x0e},
    {0x301a,0xb4},
    {0x3031,0x0a},
    {0x3032,0x60},
    {0x3038,0x44},
    {0x3207,0x17},
    {0x320c,0x06},
    {0x320d,0xa0},
    {0x320e,0x08},
    {0x320f,0x4a},
    {0x3250,0xcc},
    {0x3251,0x02},
    {0x3252,0x08},
    {0x3253,0x45},
    {0x3254,0x05},
    {0x3255,0x3b},
    {0x3306,0x78},
    {0x330a,0x00},
    {0x330b,0xc8},
    {0x330f,0x24},
    {0x3314,0x80},
    {0x3315,0x40},
    {0x3317,0xf0},
    {0x331f,0x12},
    {0x3364,0x00},
    {0x3385,0x41},
    {0x3387,0x41},
    {0x3389,0x09},
    {0x33ab,0x00},
    {0x33ac,0x00},
    {0x33b1,0x03},
    {0x33b2,0x12},
    {0x33f8,0x02},
    {0x33fa,0x01},
    {0x3409,0x08},
    {0x34f0,0xc0},
    {0x34f1,0x20},
    {0x34f2,0x03},
    {0x3622,0xf5},
    {0x3630,0x5c},
    {0x3631,0x80},
    {0x3632,0xc8},
    {0x3633,0x32},
    {0x3638,0x2a},
    {0x3639,0x07},
    {0x363b,0x48},
    {0x363c,0x83},
    {0x363d,0x10},
    {0x36ea,0x3a},
    {0x36fa,0x25},
    {0x36fb,0x05},
    {0x36fd,0x04},
    {0x3900,0x11},
    {0x3901,0x05},
    {0x3902,0xc5},
    {0x3904,0x04},
    {0x3908,0x91},
    {0x391e,0x00},
    {0x3e01,0x53},
    {0x3e02,0xe0},
    {0x3e09,0x20},
    {0x3e0e,0xd2},
    {0x3e14,0xb0},
    {0x3e1e,0x7c},
    {0x3e26,0x20},
    {0x4418,0x38},
    {0x4503,0x10},
    {0x4837,0x14},
    {0x5000,0x0e},
    {0x540c,0x51},
    {0x550f,0x38},
    {0x5780,0x67},
    {0x5784,0x10},
    {0x5785,0x06},
    {0x5787,0x02},
    {0x5788,0x00},
    {0x5789,0x00},
    {0x578a,0x02},
    {0x578b,0x00},
    {0x578c,0x00},
    {0x5790,0x00},
    {0x5791,0x00},
    {0x5792,0x00},
    {0x5793,0x00},
    {0x5794,0x00},
    {0x5795,0x00},
    {0x5799,0x04},
    {0x3222,0x02},//slave
    {0x3223,0x44},
    {0x3226,0x06},
    {0x3227,0x06},
    {0x3228,0x08},
    {0x3229,0x44},
    {0x322b,0x0b},
    {0x3225,0x04},
    {0x3231,0x28},
    {0x300a,0x62},//slave
    {0x36e9,0x24},
    {0x36f9,0x24},
    {0x0100,0x01},

    {0x3e03, 0x0b}, // gain mode: look up table
    {0x3e08, 0x23}, // again 3.000
    {0x3e09, 0x35}, 
    {0x3e06, 0x00}, // dgain 1.000
    {0x3e07, 0x80}, 

    // {0x3361, 0xc0}, // slave strobe disable 
    // {0x3361, 0x00}, // slave strobe disable

    // {0x33f8, 0x02},
    // {0x3314, 0x80},
    {0x33fa, 0x02},
    {0x3317, 0x00},
};

static void sc132gs_busy_delay(int32_t ms)
{
    aiva_msleep(ms);
}

static int sc132gs_write_reg(int i2c_num, uint16_t reg_addr, uint8_t reg_val)
{
    int ret;
    uint8_t data_buf[3];
    data_buf[0] = (reg_addr >> 8) & 0xff;
    data_buf[1] = (reg_addr >> 0) & 0xff;
    data_buf[2] = reg_val;
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, data_buf, 3);

    return ret;
}

static int sc132gs_read_reg(int i2c_num, uint16_t reg_addr, uint8_t *reg_val)
{
    int ret;

    uint8_t addr_buf[2];

    addr_buf[0] = (reg_addr >> 8) & 0xff;
    addr_buf[1] = (reg_addr >> 0) & 0xff;
    
    ret = i2c_send_data(i2c_num, SENSOR_ADDR_WR, &addr_buf[0], 2);
    if (ret < 0) {
        return ret;
    }

    ret = i2c_recv_data(i2c_num, SENSOR_ADDR_WR, 0, 0, reg_val, 1);
    
    return ret;
}

static int sc132gs_program_regs(
        int             i2c_num,
        const SC132GS_REG_T    *reg_list,
        int             cnt
        )
{
    int i;
    int ret = 0;
    for (i = 0; i < cnt; i++) {
        if (reg_list[i].reg_addr != REG_DLY) {
            ret = sc132gs_write_reg(i2c_num, reg_list[i].reg_addr, reg_list[i].data);
            if (ret < 0) {
                LOGE(__func__, "error: i = %d, reg_addr 0x%x, data 0x%x.",
                        i, reg_list[i].reg_addr, reg_list[i].data);
                break;
            }
        } else {
            sc132gs_busy_delay(reg_list[i].data);
        }
    }
    return ret;
}

static int sc132gs_i2c_test(int i2c_num)
{
    uint8_t id_hi, id_lo;
    int ret = -1;

    ret = sc132gs_read_reg(i2c_num, SC132GS_REG_CHIP_ID, &id_hi);
    if (ret != 0) {
        LOGE(TAG, "read sensor id error\n");
        return ret;
    }
    ret = sc132gs_read_reg(i2c_num, SC132GS_REG_CHIP_ID+1, &id_lo);
    if (ret != 0) {
        LOGE(TAG, "read sensor id error\n");
        return ret;
    }
    LOGD(TAG, "read sensor id: id_hi=0x%x, id_lo=0x%x.", id_hi, id_lo);
    uint16_t id = (id_hi << 8) | id_lo;
    if (id != CHIP_ID) {
        LOGE(TAG, "read sensor id error, id=0x%x, expected: 0x2311\n", id);
        return -1;
    }
    return 0;
}

static int sc132gs_init(int i2c_num)
{
    int ret;
    uint8_t tmp;

    //ret = sc132gs_program_regs(i2c_num, sc132gs_setting_2lane, AIVA_ARRAY_LEN(sc132gs_setting_2lane));
    ret = sc132gs_program_regs(i2c_num, sc132gs_setting_1lane, AIVA_ARRAY_LEN(sc132gs_setting_1lane));

    if (ret != 0) {
        LOGE(TAG, "sc132gs program regs fail\n");
    }

    return ret;
}

static int cis_sc132gs_init(cis_dev_driver_t *dev_driver)
{
    int i2c_num = dev_driver->i2c_num;

    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    } else {
        LOGE(TAG, "invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 400*1000);

    if (sc132gs_i2c_test(i2c_num) < 0) {
        LOGE(TAG, "sc132gs i2c test fail\n");
        return -1;
    }

    if (sc132gs_init(dev_driver->i2c_num) != 0) {
        LOGE(TAG, "sc132gs_init fail\n");
        return -1;
    }
    return 0;
}

static int cis_sc132gs_init_master(cis_dev_driver_t *dev_driver)
{
    int i2c_num = dev_driver->i2c_num;

    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    } else {
        LOGE(TAG, "invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 400*1000);

    if (sc132gs_i2c_test(i2c_num) < 0) {
        LOGE(TAG, "sc132gs i2c test fail\n");
        return -1;
    }

    if (sc132gs_program_regs(i2c_num, sc132gs_setting_1lane_master, AIVA_ARRAY_LEN(sc132gs_setting_1lane_master)) != 0) {
        LOGE(TAG, "sc132gs_init fail\n");
        return -1;
    }
    return 0;
}

static int cis_sc132gs_init_slave(cis_dev_driver_t *dev_driver)
{
    int i2c_num = dev_driver->i2c_num;

    // enable sensor mclk
    if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK0) {
        sysctl_set_sens_mclk(MCLK_ID0, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK0, 1);
    } else if (dev_driver->mclk_id == CIS_MCLK_ID_MCLK1) {
        sysctl_set_sens_mclk(MCLK_ID1, 24*1000*1000);
        sysctl_set_io_switch(IO_SWITCH_MCLK1, 1);
    } else {
        LOGE(TAG, "invalid mclk id: %d\n", dev_driver->mclk_id);
    }

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 400*1000);

    if (sc132gs_i2c_test(i2c_num) < 0) {
        LOGE(TAG, "sc132gs i2c test fail\n");
        return -1;
    }

    if (sc132gs_program_regs(i2c_num, sc132gs_setting_1lane_slave, AIVA_ARRAY_LEN(sc132gs_setting_1lane_slave)) != 0) {
        LOGE(TAG, "sc132gs_init fail\n");
        return -1;
    }
    return 0;
}

static int cis_sc132gs_start_stream(cis_dev_driver_t *dev_driver, const cis_config_t *config)
{
    int ret;
    SC132GS_REG_T       *reg_list;
    int                 cnt;

    reg_list = sc132gs_start_regs;
    cnt     = AIVA_ARRAY_LEN(sc132gs_start_regs);
    ret     = sc132gs_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(TAG, "sc132gs_start failed!");
    }
    return ret;
}

static int cis_sc132gs_stop_stream(cis_dev_driver_t *dev_driver)
{
    int ret;
    SC132GS_REG_T       *reg_list;
    int                 cnt;

    reg_list = sc132gs_stop_regs;
    cnt     = AIVA_ARRAY_LEN(sc132gs_stop_regs);
    ret     = sc132gs_program_regs(dev_driver->i2c_num, reg_list, cnt);
    if (ret < 0) {
        LOGE(TAG, "sc132gs_stop failed!");
    }
    return ret;
}

static void cis_sc132gs_power_on(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    sc132gs_busy_delay(3);
    gpio_set_pin(power_pin, GPIO_PV_HIGH);
    sc132gs_busy_delay(5);
    return;
}

static void cis_sc132gs_power_off(cis_dev_driver_t *dev_driver)
{
    int power_pin = dev_driver->power_pin;

    gpio_set_drive_mode(power_pin, GPIO_DM_OUTPUT);
    gpio_set_pin(power_pin, GPIO_PV_LOW);
    sc132gs_busy_delay(5);
    return;
}

static void cis_sc132gs_reset(cis_dev_driver_t *dev_driver)
{
    return;
}

static int cis_sc132gs_wake(cis_dev_driver_t *dev_driver)
{
    return 0;
}

static int cis_sc132gs_sleep(cis_dev_driver_t *dev_driver)
{
    return 0;
}

static int cis_sc132gs_get_interface_param(cis_dev_driver_t *dev_driver, cis_interface_param_t *param)
{
    param->interface_type                   = CIS_INTERFACE_TYPE_MIPI;

//    param->mipi_param.freq                  = 480;
//    param->mipi_param.lane_num              = 2;

    param->mipi_param.freq                  = 720;
    param->mipi_param.lane_num              = 1;    

    param->mipi_param.vc_num                = 1;
    param->mipi_param.virtual_channels[0]   = MIPI_VIRTUAL_CHANNEL_0;
    param->mipi_param.data_type             = MIPI_DATA_TYPE_RAW_10;

    return 0;
}

static int cis_sc132gs_get_exposure_param(cis_dev_driver_t *dev_driver, cis_exposure_param_t *exp_param)
{
    exp_param->min_again    = 1.0;
    exp_param->max_again    = 28.547;
    exp_param->step_again   = 0.31;

    exp_param->min_dgain    = 1.0;
    exp_param->max_dgain    = 31.5;
    exp_param->step_dgain   = 0.31;

    exp_param->min_itime    = 0.0;
    exp_param->max_itime    = MAX_EXPOSURE;
    exp_param->step_itime   = 1.0;
    return 0;
}

static int cis_sc132gs_split_again(float again, uint8_t *again_coarse, uint8_t *again_fine)
{
    int i;
    int items = sizeof(sc132gs_again_table)/sizeof(SC132GS_GAIN_T);

    for (i = 0; i < items; i++)
    {
        if (again <= sc132gs_again_table[i].gain)
        {
            *again_coarse = sc132gs_again_table[i].coarse;
            *again_fine   = sc132gs_again_table[i].fine;
            return 0;
        }
    }

    LOGE(__func__, "analog gain out of range: again=%f", again);
    *again_coarse = 0x03;
    *again_fine = 0x20;
    return -1;
}

static int cis_sc132gs_split_dgain(float dgain, uint8_t *dgain_coarse, uint8_t *dgain_fine)
{
    int i;
    int items = sizeof(sc132gs_dgain_table)/sizeof(SC132GS_GAIN_T);

    for (i = 0; i < items; i++)
    {
        if (dgain <= sc132gs_dgain_table[i].gain)
        {
            *dgain_coarse = sc132gs_dgain_table[i].coarse;
            *dgain_fine   = sc132gs_dgain_table[i].fine;
            return 0;
        }
    }

    LOGE(__func__, "digital gain out of range: dgain=%f", dgain);
    *dgain_coarse = 0x00;
    *dgain_fine = 0x80;
    return -1;
}

static int cis_sc132gs_set_exposure(cis_dev_driver_t *dev_driver, const cis_exposure_t *exp)
{
    int i2c_num = dev_driver->i2c_num;
    float again = exp->again;
    float dgain = exp->dgain;
    float itime = exp->itime;

    i2c_init(i2c_num, SENSOR_ADDR_WR, 7, 350*1000);

    //-- group hold start
    sc132gs_write_reg(i2c_num, 0x3800, 0x00);

    // exposure
    uint8_t itime_h;
    uint8_t itime_m;    
    uint8_t itime_l;
    if (itime > MAX_EXPOSURE)
        itime = MAX_EXPOSURE;
    uint32_t exposure = (int)itime * 16;
    itime_l = exposure & 0xff;
    itime_m = (exposure >> 8) & 0xff;
    itime_h = (exposure >> 16) & 0xf;
    sc132gs_write_reg(i2c_num, 0x3e02, itime_l);
    sc132gs_write_reg(i2c_num, 0x3e01, itime_m);
    sc132gs_write_reg(i2c_num, 0x3e00, itime_h);

    // again
    uint8_t again_coarse;
    uint8_t again_fine;    
    if (cis_sc132gs_split_again(again, &again_coarse, &again_fine) != 0) {
        LOGE(__func__, "split again fail");
        return -1;
    }
    sc132gs_write_reg(i2c_num, 0x3e08, again_coarse);
    sc132gs_write_reg(i2c_num, 0x3e09, again_fine);

    // dgain
    uint8_t dgain_coarse;
    uint8_t dgain_fine;
    if (cis_sc132gs_split_dgain(dgain, &dgain_coarse, &dgain_fine) != 0) {
        LOGE(__func__, "split again fail");
        return -1;
    }
    sc132gs_write_reg(i2c_num, 0x3e06, dgain_coarse);
    sc132gs_write_reg(i2c_num, 0x3e07, dgain_fine);

    //-- group hold end
    sc132gs_write_reg(i2c_num, 0x3800, 0x10);
    sc132gs_write_reg(i2c_num, 0x3800, 0x60);

    return 0;
}

static int cis_sc132gs_get_frame_parameter(cis_dev_driver_t *dev_driver, cis_frame_param_t *param)
{
    param->width = 1080;
    param->height = 1280;
    param->framerate = 30;
    param->format = CIS_FORMAT_RGB_BAYER10;

    return 0;
}

static int cis_sc132gs_get_vcm_param(cis_dev_driver_t *dev_driver, cis_vcm_param_t *param)
{
    param->has_vcm_motor = 0;
    param->min_vcm_pos   = 0.0;
    param->max_vcm_pos   = 0.0;
    param->min_vcm_step  = 0.0;
    return 0;
}

static cis_dev_driver_t sc132gs_dev0 = {
    .name                   = "sc132gs_dev0",
    .i2c_num                = I2C_DEVICE_0,
    .power_pin              = GPIO_PIN0,
    .reset_pin              = GPIO_PIN0,
    .mclk_id                = CIS_MCLK_ID_MCLK0,
    .context                = NULL,
    .init                   = cis_sc132gs_init,
    .start_stream           = cis_sc132gs_start_stream,
    .stop_stream            = cis_sc132gs_stop_stream,
    .power_on               = cis_sc132gs_power_on,
    .power_off              = cis_sc132gs_power_off,
    .reset                  = cis_sc132gs_reset,
    .wake                   = cis_sc132gs_wake,
    .sleep                  = cis_sc132gs_sleep,
    .get_interface_param    = cis_sc132gs_get_interface_param,
    .get_exposure_param     = cis_sc132gs_get_exposure_param,
    .get_vcm_param          = cis_sc132gs_get_vcm_param,
    .set_exposure           = cis_sc132gs_set_exposure,
    .get_frame_parameter         = cis_sc132gs_get_frame_parameter
};

static cis_dev_driver_t sc132gs_dev1 = {
    .name                   = "sc132gs_dev1",
    .i2c_num                = I2C_DEVICE_1,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    // .init                   = cis_sc132gs_init_master,
    .init                   = cis_sc132gs_init_master,
    .start_stream           = cis_sc132gs_start_stream,
    .stop_stream            = cis_sc132gs_stop_stream,
    .power_on               = cis_sc132gs_power_on,
    .power_off              = cis_sc132gs_power_off,
    .reset                  = cis_sc132gs_reset,
    .wake                   = cis_sc132gs_wake,
    .sleep                  = cis_sc132gs_sleep,
    .get_interface_param    = cis_sc132gs_get_interface_param,
    .get_exposure_param     = cis_sc132gs_get_exposure_param,
    .get_vcm_param          = cis_sc132gs_get_vcm_param,
    .set_exposure           = cis_sc132gs_set_exposure,
    .get_frame_parameter         = cis_sc132gs_get_frame_parameter
};

static cis_dev_driver_t sc132gs_dev2 = {
    .name                   = "sc132gs_dev2",
    .i2c_num                = I2C_DEVICE_2,
    .power_pin              = GPIO_PIN1,
    .reset_pin              = GPIO_PIN1,
    .mclk_id                = CIS_MCLK_ID_MCLK1,
    .context                = NULL,
    // .init                   = cis_sc132gs_init_slave,
    .init                   = cis_sc132gs_init_slave,
    .start_stream           = cis_sc132gs_start_stream,
    .stop_stream            = cis_sc132gs_stop_stream,
    .power_on               = cis_sc132gs_power_on,
    .power_off              = cis_sc132gs_power_off,
    .reset                  = cis_sc132gs_reset,
    .wake                   = cis_sc132gs_wake,
    .sleep                  = cis_sc132gs_sleep,
    .get_interface_param    = cis_sc132gs_get_interface_param,
    .get_exposure_param     = cis_sc132gs_get_exposure_param,
    .get_vcm_param          = cis_sc132gs_get_vcm_param,
    .set_exposure           = cis_sc132gs_set_exposure,
    .get_frame_parameter         = cis_sc132gs_get_frame_parameter
};

cis_dev_driver_t *cis_sc132gs[] = {
    &sc132gs_dev0,
    &sc132gs_dev1,
    &sc132gs_dev2
};

/* Export Device:
 * 	    [uclass ID], [device name], [flags], [driver], [private pointer]
 */
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     sc132gs_dev0,    0,      &sc132gs_dev0,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     sc132gs_dev1,    0,      &sc132gs_dev1,    NULL);
UDEVICE_EXPORT(UCLASS_CIS_MIPI,     sc132gs_dev2,    0,      &sc132gs_dev2,    NULL);
