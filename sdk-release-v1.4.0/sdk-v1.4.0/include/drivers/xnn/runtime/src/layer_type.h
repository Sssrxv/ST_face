#ifndef XNN_LAYER_TYPE_H
#define XNN_LAYER_TYPE_H

#include <string>

namespace xnn {

namespace LayerType {
enum LayerType
{
    XPE = 0,
    AbsVal,
    ArgMax,
    BatchNorm,
    Bias,
    BinaryOp,
    BNLL,
    ChannelAlign,
    Cast,
    CHW2HWC,
    Clip,
    Concat,
    Convolution,
    ConvolutionDepthWise,
    Crop,
    Deconvolution,
    DeconvolutionDepthWise,
    Dequantize,
    DequantizeInt8,
    DetectionOutput,
    Dropout,
    Eltwise,
    ELU,
    Embed,
    Exp,
    ExpandDims,
    Flatten,
    HardSigmoid,
    HWC2CHW,
    InnerProduct,
    Input,
    InputConvert,
    InputConvert2UINT8,
    InstanceNorm,
    Interp,
    Log,
    LRN,
    LSTM,
    MemoryData,
    MVN,
    Normalize,
    Packing,
    Padding,
    Permute,
    Pooling,
    Power,
    PReLU,
    PriorBox,
    Proposal,
    PSROIPooling,
    Quantize,
    Reduction,
    ReLU,
    Reorg,
    Requantize,
    Reshape,
    RNN,
    ROIAlign,
    ROIPooling,
    Scale,
    SELU,
    ShuffleChannel,
    Sigmoid,
    Slice,
    Softmax,
    Split,
    SPP,
    Squeeze,
    SubstractMeanThenNorm,
    TanH,
    Threshold,
    Tile,
    UnaryOp,
    YoloDetectionOutput,
    Yolov3DetectionOutput,
    CustomBit = (1<<8),
    MaxLayerType
};
} // namespace LayerType


std::string layertype_to_string(LayerType::LayerType layertype);


} // namespace xnn

#endif // XNN_LAYER_TYPE_H
