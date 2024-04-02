#ifndef XNN_LAYER_REGISTRY_H
#define XNN_LAYER_REGISTRY_H

#include "platform.h"
#include "layer.h"
#include "layer_type.h"


namespace xnn {

// layer factory function
typedef Layer* (*layer_creator_func)();

struct layer_registry_entry
{
    // layer type name
    const char* name;
    LayerType::LayerType layer_type;
    // layer factory entry
    layer_creator_func creator;
};

const int custom_layer_mask = 0x1 << 30;

// create layer from layer index
Layer* create_layer(int index);
// create layer from type name
Layer* create_layer(const char* type);
// create layer from layer type 
Layer* create_layer(LayerType::LayerType layer_type);



} // namespace xnn 


#endif // XNN_LAYER_REGISTRY_H
