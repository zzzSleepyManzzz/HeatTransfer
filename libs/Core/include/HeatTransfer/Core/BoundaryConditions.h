#pragma once

#include <vector>

#include "HeatTransfer/Core/Face.h"
#include "HeatTransfer/Core/FaceBoundaryCondition.h"
#include "HeatTransfer/Core/LetterPatch.h"

namespace HeatTransfer::Core
{
    struct BoundaryConditions
    {
        FaceBoundaryCondition Left;
        FaceBoundaryCondition Right;
        FaceBoundaryCondition Top;
        FaceBoundaryCondition Bottom;
        FaceBoundaryCondition Front;
        FaceBoundaryCondition Back;
        std::vector<LetterPatch> Letters;

        bool IsWellPosed() const;

        const FaceBoundaryCondition& GetFace(Face face) const;
        void SetFace(Face face, const FaceBoundaryCondition& faceBoundaryCondition);

        static BoundaryConditions Default();
    };
}
