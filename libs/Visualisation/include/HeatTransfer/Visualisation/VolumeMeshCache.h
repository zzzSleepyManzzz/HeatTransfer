#pragma once

#include <array>
#include <memory>

#include "HeatTransfer/Visualisation/PlotField.h"
#include "HeatTransfer/Visualisation/RenderFrameData.h"
#include "HeatTransfer/Visualisation/VolumeMesh.h"
#include "HeatTransfer/Visualisation/VolumeViewState.h"

namespace HeatTransfer::Visualisation
{
    class VolumeMeshCache
    {
    public:
        void Update(std::shared_ptr<RenderFrameData> data,
                    PlotField field,
                    const VolumeViewState& state);

        std::array<VolumeMesh, 6>& GetFaceMeshes();
        VolumeMesh& GetSliceMesh();

    private:
        static constexpr int PLANE_POINTS = 2500;

        std::array<VolumeMesh, 6> _faceMeshes; // Ordered as Core::Faces::ALL
        VolumeMesh _sliceMesh;

        // What the meshes were last built from
        std::shared_ptr<RenderFrameData> _source;
        PlotField _field = PlotField::TEMPERATURE;
        std::array<bool, 3> _showPlane = {};
        std::array<int, 3> _planeIndex = {};
        bool _showOuterFaces = false;
        int _colorMap = -1;
    };
}
