#ifndef XNN_BLOB_H
#define XNN_BLOB_H

#include <string>
#include <vector>
#include "platform.h"

namespace xnn {

class Blob
{
public:
    // empty
    Blob();
    ~Blob();

    int clear_consumed_status();

public:
    // blob name
    std::string name;
    // layer index which produce this blob as output
    int producer;
    // layer index which need this blob as input
    std::vector<int> consumers;
    std::vector<int> consumed_status;
};

} // namespace xnn

#endif // XNN_BLOB_H
