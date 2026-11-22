#include "HeatTransfer/Core/BoundaryConditions.h"

#include <stdexcept>

#include "HeatTransfer/Core/Faces.h"

namespace HeatTransfer::Core
{
    const FaceBoundaryCondition& BoundaryConditions::GetFace(Face face) const
    {
        switch (face)
        {
            case Face::LEFT:
                return Left;
            case Face::RIGHT:
                return Right;
            case Face::TOP:
                return Top;
            case Face::BOTTOM:
                return Bottom;
            case Face::FRONT:
                return Front;
            case Face::BACK:
                return Back;
            default:
                throw std::runtime_error("Invalid face");
        }
    }

    void BoundaryConditions::SetFace(Face face, const FaceBoundaryCondition& faceBoundaryCondition)
    {
        switch (face)
        {
            case Face::LEFT:
                Left = faceBoundaryCondition;
                break;
            case Face::RIGHT:
                Right = faceBoundaryCondition;
                break;
            case Face::TOP:
                Top = faceBoundaryCondition;
                break;
            case Face::BOTTOM:
                Bottom = faceBoundaryCondition;
                break;
            case Face::FRONT:
                Front = faceBoundaryCondition;
                break;
            case Face::BACK:
                Back = faceBoundaryCondition;
                break;
            default:
                throw std::runtime_error("Invalid face");
        }
    }

    bool BoundaryConditions::IsWellPosed() const
    {
        // Must have at least one face that is not a heat flux boundary

        for (const auto face : Faces::ALL)
        {
            if (GetFace(face).Type != BoundaryType::HEAT_FLUX)
                return true;
        }

        for (const auto& letter : Letters)
        {
            if (letter.Enabled)
                return true;
        }

        return false;
    }

    BoundaryConditions BoundaryConditions::Default()
    {
        return {.Left = FaceBoundaryCondition::FixedTemperature(350.0),
                .Right = FaceBoundaryCondition::FixedTemperature(250.0),
                .Top = FaceBoundaryCondition::Insulated(),
                .Bottom = FaceBoundaryCondition::Insulated(),
                .Front = FaceBoundaryCondition::Insulated(),
                .Back = FaceBoundaryCondition::Insulated(),
                .Letters = {{.Letter = Letter::H,
                             .Surface = Face::TOP,
                             .UMin = 0.15,
                             .UMax = 0.42,
                             .VMin = 0.15,
                             .VMax = 0.85,
                             .Temperature = 150.0},
                            {.Letter = Letter::T,
                             .Surface = Face::TOP,
                             .UMin = 0.58,
                             .UMax = 0.85,
                             .VMin = 0.15,
                             .VMax = 0.85,
                             .Temperature = 150.0}}};
    }
}
