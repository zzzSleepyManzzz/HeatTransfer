#include "HeatTransfer/Visualisation/Slicer.h"

#include <algorithm>
#include <cmath>

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Visualisation
{
    SliceData
    Slicer::Slice(const Core::Tensor3d& field, SliceAxis axis, int index, int targetNumPoints)
    {
        ZoneScoped;

        const Eigen::Index rows = field.dimension(0);
        const Eigen::Index cols = field.dimension(1);
        const Eigen::Index layers = field.dimension(2);

        const Eigen::Index fixedIndex = ClampIndex(field, axis, index);

        Eigen::Index uSize = cols;
        Eigen::Index vSize = rows;

        SliceData slice;

        switch (axis)
        {
            case SliceAxis::X:
                uSize = rows;
                vSize = layers;
                slice.ULabel = "Width [mm]";
                slice.VLabel = "Height [mm]";
                break;
            case SliceAxis::Y:
                uSize = cols;
                vSize = layers;
                slice.ULabel = "Length [mm]";
                slice.VLabel = "Height [mm]";
                break;
            case SliceAxis::Z:
                uSize = cols;
                vSize = rows;
                slice.ULabel = "Length [mm]";
                slice.VLabel = "Width [mm]";
                break;
        }

        const double total = (double)uSize * (double)vSize;
        const auto stride = std::max<Eigen::Index>(
            1, (Eigen::Index)std::ceil(std::sqrt(total / std::max(1, targetNumPoints))));

        const auto us = SampleIndices(uSize, stride);
        const auto vs = SampleIndices(vSize, stride);

        slice.UCount = (int)us.size();
        slice.VCount = (int)vs.size();

        const auto N = us.size() * vs.size();

        slice.U.reserve(N);
        slice.V.reserve(N);
        slice.Values.reserve(N);
        slice.Xs.reserve(N);
        slice.Ys.reserve(N);
        slice.Zs.reserve(N);

        for (const auto v : vs)
        {
            for (const auto u : us)
            {
                // Map the in-plane (u, v) back to (row, column, layer)
                Eigen::Index i = 0;
                Eigen::Index j = 0;
                Eigen::Index k = 0;

                switch (axis)
                {
                    case SliceAxis::X:
                        i = u;
                        j = fixedIndex;
                        k = v;
                        break;
                    case SliceAxis::Y:
                        i = fixedIndex;
                        j = u;
                        k = v;
                        break;
                    case SliceAxis::Z:
                        i = v;
                        j = u;
                        k = fixedIndex;
                        break;
                }

                slice.U.push_back((double)u);
                slice.V.push_back((double)v);
                slice.Values.push_back(field(i, j, k));

                slice.Xs.push_back((double)j);
                slice.Ys.push_back((double)i);
                slice.Zs.push_back((double)k);
            }
        }

        slice.UMin = 0.0;
        slice.UMax = (double)(uSize - 1);
        slice.VMin = 0.0;
        slice.VMax = (double)(vSize - 1);

        const auto [minIt, maxIt] = std::minmax_element(slice.Values.begin(), slice.Values.end());
        slice.ValueMin = *minIt;
        slice.ValueMax = *maxIt;

        return slice;
    }

    std::vector<Eigen::Index> Slicer::SampleIndices(Eigen::Index size, Eigen::Index stride)
    {
        std::vector<Eigen::Index> indices;

        for (Eigen::Index n = 0; n < size; n += stride)
            indices.push_back(n);

        if (indices.back() != size - 1)
            indices.push_back(size - 1);

        return indices;
    }

    int Slicer::ClampIndex(const Core::Tensor3d& field, SliceAxis axis, int index)
    {
        // Clamps a slice index to the field, where a negative index selects the middle slice

        const int size = AxisSize(field, axis);

        if (index < 0)
            return size / 2;

        return std::clamp(index, 0, size - 1);
    }

    int Slicer::AxisSize(const Core::Tensor3d& field, SliceAxis axis)
    {
        // The field is indexed (row, column, layer), i.e. (Y, X, Z)

        switch (axis)
        {
            case SliceAxis::X:
                return (int)field.dimension(1);
            case SliceAxis::Y:
                return (int)field.dimension(0);
            case SliceAxis::Z:
            default:
                return (int)field.dimension(2);
        }
    }
}
