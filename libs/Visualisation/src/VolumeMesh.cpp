#include "HeatTransfer/Visualisation/VolumeMesh.h"

#include <algorithm>

#include <implot.h>

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Visualisation
{
    void
    VolumeMesh::AddSlice(const SliceData& slice, double valueMin, double valueMax, int colormap)
    {
        // Triangulates the slice (two triangles per grid cell) and colours each vertex by mapping
        // its value from [valueMin, valueMax] onto the colormap. Degenerate slices are skipped

        ZoneScoped;

        if (!slice.IsPlane())
            return;

        const auto base = (unsigned int)_xs.size();

        _xs.insert(_xs.end(), slice.Xs.begin(), slice.Xs.end());
        _ys.insert(_ys.end(), slice.Ys.begin(), slice.Ys.end());
        _zs.insert(_zs.end(), slice.Zs.begin(), slice.Zs.end());

        // Colour of every vertex in this slice
        const double range = valueMax > valueMin ? valueMax - valueMin : 1.0;

        std::vector<ImU32> vertexColours;
        vertexColours.reserve(slice.Values.size());

        for (const auto value : slice.Values)
        {
            const auto t = (float)std::clamp((value - valueMin) / range, 0.0, 1.0);
            vertexColours.push_back(
                ImGui::ColorConvertFloat4ToU32(ImPlot::SampleColormap(t, colormap)));
        }

        // Two triangles per grid cell
        auto addCorner = [&](int u, int v)
        {
            const auto local = (unsigned int)(v * slice.UCount + u);
            _indices.push_back(base + local);
            _colours.push_back(vertexColours[local]);
        };

        for (int v = 0; v < slice.VCount - 1; v++)
        {
            for (int u = 0; u < slice.UCount - 1; u++)
            {
                addCorner(u, v);
                addCorner(u + 1, v);
                addCorner(u + 1, v + 1);

                addCorner(u, v);
                addCorner(u + 1, v + 1);
                addCorner(u, v + 1);
            }
        }
    }

    void VolumeMesh::Clear()
    {
        _xs.clear();
        _ys.clear();
        _zs.clear();
        _indices.clear();
        _colours.clear();
    }

    bool VolumeMesh::IsEmpty() const
    {
        return _indices.empty();
    }

    const std::vector<double>& VolumeMesh::GetXs() const
    {
        return _xs;
    }

    const std::vector<double>& VolumeMesh::GetYs() const
    {
        return _ys;
    }

    const std::vector<double>& VolumeMesh::GetZs() const
    {
        return _zs;
    }

    const std::vector<unsigned int>& VolumeMesh::GetIndices() const
    {
        return _indices;
    }

    std::vector<ImU32>& VolumeMesh::GetColours()
    {
        return _colours;
    }
}
