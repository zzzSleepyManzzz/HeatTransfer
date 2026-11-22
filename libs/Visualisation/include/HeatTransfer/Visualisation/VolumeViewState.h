#pragma once

#include <array>
#include <memory>

#include "HeatTransfer/Visualisation/RenderFrameData.h"
#include "HeatTransfer/Visualisation/VolumeMesh.h"

namespace HeatTransfer::Visualisation
{
    enum class VolumeField
    {
        TEMPERATURE,
        RESIDUAL
    };

    struct VolumeViewState
    {
        VolumeField Field = VolumeField::TEMPERATURE;

        bool ResetZoom = false;
        int SelectedColorMap = 6; // Hot by default

        bool ShowOuterFaces = true;

        std::array<bool, 3> ShowPlane = {false, false, false};
        std::array<int, 3> PlaneIndex = {-1, -1, -1};

        bool ShowLines = true;

        std::array<VolumeMesh, 6> FaceMeshes; // Ordered as Core::Faces::ALL
        VolumeMesh SliceMesh;
        std::shared_ptr<RenderFrameData> MeshSource;
        std::array<bool, 3> MeshShowPlane = {};
        std::array<int, 3> MeshPlaneIndex = {};
        bool MeshShowOuterFaces = false;
        int MeshColorMap = -1;
    };
}
