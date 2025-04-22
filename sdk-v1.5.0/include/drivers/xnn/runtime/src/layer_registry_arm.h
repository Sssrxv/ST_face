#ifndef XNN_LAYER_REGISTRY_ARM_H
#define XNN_LAYER_REGISTRY_ARM_H

#include "platform.h"
#include "layer.h"
#include "layer_type.h"
#include "layer_registry.h"


namespace xnn {

// create layer from type name
Layer* create_layer_arm(const char* type);
// create layer from layer type 
Layer* create_layer_arm(LayerType::LayerType layer_type);



} // namespace xnn 


#endif // XNN_LAYER_REGISTRY_ARM_H
