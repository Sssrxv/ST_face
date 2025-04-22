#ifndef XNN_LAYER_REGISTRY_X86_H
#define XNN_LAYER_REGISTRY_X86_H

#include "platform.h"
#include "layer.h"
#include "layer_type.h"
#include "layer_registry.h"


namespace xnn {

// create layer from type name
Layer* create_layer_x86(const char* type);
// create layer from layer type 
Layer* create_layer_x86(LayerType::LayerType layer_type);



} // namespace xnn 


#endif // XNN_LAYER_REGISTRY_X86_H
