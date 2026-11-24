#include "HeatTransfer/Visualisation/VolumeMeshCache.h"

#include <utility>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Visualisation/Slicer.h"

namespace HeatTransfer::Visualisation
{
    void VolumeMeshCache::Update(std::shared_ptr<RenderFrameData> data,
                                 PlotField field,
                                 const VolumeViewState& state)
    {
        // Slices the 6 outer faces and up to 3 planes, downsamples them, and builds coloured meshes
        // Only rebuilds on a new simulation result or a change to the colour map, planes or faces

        const auto& T = data->GetField(field);
        const int colormap =
            field == PlotField::RESIDUAL ? state.ResidualColorMap : state.TemperatureColorMap;

        std::array<int, 3> planeIndex;

        for (int axis = 0; axis < 3; axis++)
            planeIndex[axis] = Slicer::ClampIndex(T, (SliceAxis)axis, state.PlaneIndex[axis]);

        const bool upToDate = _source == data && _field == field && _showPlane == state.ShowPlane &&
                              _planeIndex == planeIndex &&
                              _showOuterFaces == state.ShowOuterFaces && _colorMap == colormap;

        if (upToDate)
            return;

        ZoneScoped;

        const double tMin = data->GetFieldMin(field);
        const double tMax = data->GetFieldMax(field);

        auto slice = [&](SliceAxis axis, int index)
        { return Slicer::Slice(T, axis, index, PLANE_POINTS); };

        _sliceMesh.Clear();

        for (int axis = 0; axis < 3; axis++)
        {
            if (state.ShowPlane[axis] && !state.ShowOuterFaces)
            {
                _sliceMesh.AddSlice(slice((SliceAxis)axis, planeIndex[axis]), tMin, tMax, colormap);
            }
        }

        // The outer faces, ordered as Core::Faces::ALL (left, right, top, bottom, front, back)
        const std::array<std::pair<SliceAxis, int>, 6> faces = {
            std::pair{SliceAxis::X, 0},
            std::pair{SliceAxis::X, Slicer::AxisSize(T, SliceAxis::X) - 1},
            std::pair{SliceAxis::Z, Slicer::AxisSize(T, SliceAxis::Z) - 1},
            std::pair{SliceAxis::Z, 0},
            std::pair{SliceAxis::Y, 0},
            std::pair{SliceAxis::Y, Slicer::AxisSize(T, SliceAxis::Y) - 1}};

        for (std::size_t i = 0; i < faces.size(); i++)
        {
            _faceMeshes[i].Clear();

            if (state.ShowOuterFaces)
            {
                _faceMeshes[i].AddSlice(
                    slice(faces[i].first, faces[i].second), tMin, tMax, colormap);
            }
        }

        _source = data;
        _field = field;
        _showPlane = state.ShowPlane;
        _planeIndex = planeIndex;
        _showOuterFaces = state.ShowOuterFaces;
        _colorMap = colormap;
    }

    std::array<VolumeMesh, 6>& VolumeMeshCache::GetFaceMeshes()
    {
        return _faceMeshes;
    }

    VolumeMesh& VolumeMeshCache::GetSliceMesh()
    {
        return _sliceMesh;
    }
}
