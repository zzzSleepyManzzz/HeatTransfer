#pragma once

#include <vector>

namespace HeatTransfer::Visualisation
{
    enum class SliceAxis
    {
        X = 0,
        Y = 1,
        Z = 2
    };

    struct SliceData
    {
        int UCount = 0;
        int VCount = 0;

        std::vector<double> U; // Horizontal plot coordinate of each sample [cells]
        std::vector<double> V; // Vertical plot coordinate of each sample [cells]
        std::vector<double> Values;

        // Position of each sample in the 3D domain [cells]
        std::vector<double> Xs;
        std::vector<double> Ys;
        std::vector<double> Zs;

        double UMin = 0.0;
        double UMax = 0.0;
        double VMin = 0.0;
        double VMax = 0.0;
        double ValueMin = 0.0;
        double ValueMax = 0.0;

        const char* ULabel = "";
        const char* VLabel = "";

        bool IsPlane() const;
    };
}
