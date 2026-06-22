#pragma once

#include <vector>

namespace HeatTransfer::Visualisation
{
    struct FlattenedData
    {
        std::vector<float> x_values;
        std::vector<float> y_values;
        std::vector<float> z_values;

        int rows;
        int cols;

        float x_min;
        float x_max;

        float y_min;
        float y_max;

        float z_min;
        float z_max;
    };
}