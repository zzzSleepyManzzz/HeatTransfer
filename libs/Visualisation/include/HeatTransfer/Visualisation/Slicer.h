#pragma once

#include "HeatTransfer/Core/Tensor3d.h"

#include "HeatTransfer/Visualisation/SliceData.h"

namespace HeatTransfer::Visualisation
{
    class Slicer
    {
    public:
        Slicer() = delete;

        static SliceData
        Slice(const Core::Tensor3d& field, SliceAxis axis, int index, int targetNumPoints);

        static int ClampIndex(const Core::Tensor3d& field, SliceAxis axis, int index);
        static int AxisSize(const Core::Tensor3d& field, SliceAxis axis);

    private:
        static std::vector<Eigen::Index> SampleIndices(Eigen::Index size, Eigen::Index stride);
    };
}
