#pragma once

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"
#include "HeatTransfer/Core/Tensor3d.h"

#include "HeatTransfer/Solver/Discretisation/SystemMatrix.h"

namespace HeatTransfer::Solver
{
    class Geometry
    {
    public:
        Geometry(const Core::SimulationParameters& parameters,
                 const Core::MaterialProperties& materialProperties,
                 const Core::BoundaryConditions& boundaryConditions);

        Core::Tensor3d BuildFixedTemperatureMask() const;
        SystemMatrix::BiotNumbers BuildBiotNumbers() const;
        Core::Tensor3d BuildRhs() const;

        void ApplyFixedTemperatures(Core::Tensor3d& temperature) const;

    private:
        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryConditions;
        Core::Tensor3d::Dimensions _dimensions;

        bool HasFixedTemperature(Eigen::Index i, Eigen::Index j, Eigen::Index k) const;

        bool IsInsideLetter(const Core::LetterPatch& letter,
                            Eigen::Index i,
                            Eigen::Index j,
                            Eigen::Index k) const;

        bool IsOnFixedFace(Core::Face face, Eigen::Index i, Eigen::Index j, Eigen::Index k) const;

        double FixedTemperature(Eigen::Index i, Eigen::Index j, Eigen::Index k) const;
    };
}
