#pragma once

#include <vector>

#include <imgui.h>

#include "HeatTransfer/Visualisation/SliceData.h"

namespace HeatTransfer::Visualisation
{
    class VolumeMesh
    {
    public:
        void AddSlice(const SliceData& slice, double valueMin, double valueMax, int colormap);

        void Clear();
        bool IsEmpty() const;

        const std::vector<double>& GetXs() const;
        const std::vector<double>& GetYs() const;
        const std::vector<double>& GetZs() const;
        const std::vector<unsigned int>& GetIndices() const;
        std::vector<ImU32>& GetColours();

    private:
        std::vector<double> _xs;
        std::vector<double> _ys;
        std::vector<double> _zs;
        std::vector<unsigned int> _indices;
        std::vector<ImU32> _colours; // One per index, since ImPlot3D colours triangle corners
    };
}
