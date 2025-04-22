#ifndef LAYER_TILE_H
#define LAYER_TILE_H

#include "layer.h"

namespace xnn {

class Tile : public Layer
{
public:
    Tile();

    virtual int load_param(const ParamDict& pd);

    virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const;

public:
    int dim;
    int tiles;
};

} // namespace xnn

#endif // LAYER_TILE_H
