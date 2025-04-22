#ifndef _XNN_REG_OPS_POOL_H_
#define _XNN_REG_OPS_POOL_H_


#include <vector>
#include <string>
#include <stdint.h>
#include "layer.h"


namespace xnn {

    void xnn_reg_ops_enable_pool();
    void xnn_reg_ops_disable_pool();
    int xnn_reg_ops_pool_start(std::vector<xnn::xnn_instruction_t>* instruction_pool);
    int xnn_reg_ops_pool_end();

} //namespace xnn

#endif
