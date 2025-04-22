#ifndef _ISPHW_ISP_EXP_H_
#define _ISPHW_ISP_EXP_H_

#include "isp_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AIVA_AE_ISP_EXP_H_OFFSET_MAX (0x00000F50)  //4096-176
#define AIVA_AE_ISP_EXP_V_OFFSET_MAX (0x00000B74)  //3072-140
// 35 <= value <= 516
#define AIVA_AE_ISP_EXP_H_SIZE_MAX   (0x00000333)  //FIXME: need to double check this value
// 28 <= value <= 390
#define AIVA_AE_ISP_EXP_V_SIZE_MAX   (0x00000266)  //FIXME: need to double check this value

#define EXP_GRID_ITEMS          25  /**< number of grid items (see @ref isphwMeanLuma_t) */

#define EXP_GRID_00              0  /**< array position grid item 00 (see @ref MeanLuma_t) */
#define EXP_GRID_10              1  /**< array position grid item 10 (see @ref MeanLuma_t) */
#define EXP_GRID_20              2  /**< array position grid item 20 (see @ref MeanLuma_t) */
#define EXP_GRID_30              3  /**< array position grid item 30 (see @ref MeanLuma_t) */
#define EXP_GRID_40              4  /**< array position grid item 40 (see @ref MeanLuma_t) */
                                                                    
#define EXP_GRID_01              5  /**< array position grid item 01 (see @ref MeanLuma_t) */
#define EXP_GRID_11              6  /**< array position grid item 11 (see @ref MeanLuma_t) */
#define EXP_GRID_21              7  /**< array position grid item 21 (see @ref MeanLuma_t) */
#define EXP_GRID_31              8  /**< array position grid item 31 (see @ref MeanLuma_t) */
#define EXP_GRID_41              9  /**< array position grid item 41 (see @ref MeanLuma_t) */
                                                                    
#define EXP_GRID_02             10  /**< array position grid item 02 (see @ref MeanLuma_t) */
#define EXP_GRID_12             11  /**< array position grid item 12 (see @ref MeanLuma_t) */
#define EXP_GRID_22             12  /**< array position grid item 22 (see @ref MeanLuma_t) */
#define EXP_GRID_32             13  /**< array position grid item 32 (see @ref MeanLuma_t) */
#define EXP_GRID_42             14  /**< array position grid item 42 (see @ref MeanLuma_t) */
                                                                    
#define EXP_GRID_03             15  /**< array position grid item 03 (see @ref MeanLuma_t) */
#define EXP_GRID_13             16  /**< array position grid item 13 (see @ref MeanLuma_t) */
#define EXP_GRID_23             17  /**< array position grid item 23 (see @ref MeanLuma_t) */
#define EXP_GRID_33             18  /**< array position grid item 33 (see @ref MeanLuma_t) */
#define EXP_GRID_43             19  /**< array position grid item 43 (see @ref MeanLuma_t) */
                                                                    
#define EXP_GRID_04             20  /**< array position grid item 04 (see @ref MeanLuma_t) */
#define EXP_GRID_14             21  /**< array position grid item 14 (see @ref MeanLuma_t) */
#define EXP_GRID_24             22  /**< array position grid item 24 (see @ref MeanLuma_t) */
#define EXP_GRID_34             23  /**< array position grid item 34 (see @ref MeanLuma_t) */
#define EXP_GRID_44             24  /**< array position grid item 44 (see @ref MeanLuma_t) */


typedef enum _exp_mode_t
{
    EXP_MODE_1,
    EXP_MODE_2,
    EXP_MODE_MAX
}exp_mode_t;


int isphw_isp_exp_enable(void);
int isphw_isp_exp_disable(void);
int isphw_isp_exp_set_mode(exp_mode_t mode);
int isphw_isp_exp_set_window(const isp_window_t *window);
int isphw_isp_exp_get_result(exp_result_t *result);






#ifdef __cplusplus
}
#endif





#endif
