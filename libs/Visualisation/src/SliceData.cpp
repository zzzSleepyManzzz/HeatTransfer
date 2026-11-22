#include "HeatTransfer/Visualisation/SliceData.h"

namespace HeatTransfer::Visualisation
{
    bool SliceData::IsPlane() const
    {
        return UCount >= 2 && VCount >= 2;
    }
}
