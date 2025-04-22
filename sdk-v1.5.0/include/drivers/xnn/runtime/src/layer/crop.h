#ifndef LAYER_CROP_H
#define LAYER_CROP_H

#include "layer.h"

namespace xnn {

class Crop : public Layer
{
public:
    Crop();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

    virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt) const;

protected:
    void resolve_crop_roi(const Mat& bottom_blob, int& woffset, int& hoffset, int& coffset, int& outw, int& outh, int& outc) const;
    void resolve_crop_roi(const Mat& bottom_blob, const Mat& reference_blob, int& woffset, int& hoffset, int& coffset, int& outw, int& outh, int& outc) const;
    void resolve_crop_roi(const Mat& bottom_blob, const int* param_data, int& woffset, int& hoffset, int& coffset, int& outw, int& outh, int& outc) const;
    virtual int forward(const Mat& bottom_blob, const Mat& reference_blob, Mat& top_blob, const Option& opt) const;

public:
    // -233 = dynamic offset from reference blob
    int woffset;
    int hoffset;
    int coffset;

    // -233 = remaining
    int outw;
    int outh;
    int outc;

    // tail offset for cropping, ignored if reference blob is provided
    // woffset is aka left, and woffset2 is aka right
    int woffset2;
    int hoffset2;
    int coffset2;

    // numpy-style slice
    // if provided, all the above attributes will be ignored
    Mat starts;
    Mat ends;
    Mat axes;

    // gather and slice will use crop layer
    // if only gather one channel, then the result dimention will reduce 1
    // >>> a = np.ones((4,3,2))
    // >>> a.shape
    // (4, 3, 2)
    // >>> b = a[:,:,0] ==> gather operator
    // >>> b.shape
    // (4, 3)
    // >>> c = a[:,:,0:1] ==> slice operator
    // >>> c.shape
    // (4, 3, 1)
    bool is_gather;
};

} // namespace xnn

#endif // LAYER_CROP_H
