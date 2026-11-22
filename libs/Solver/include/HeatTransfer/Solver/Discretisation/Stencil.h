#pragma once

#include "HeatTransfer/Core/Face.h"
#include "HeatTransfer/Core/Tensor3d.h"

#include "HeatTransfer/Solver/Discretisation/SystemMatrix.h"

namespace HeatTransfer::Solver
{
    class Stencil
    {
    public:
        Stencil() = delete;

        static constexpr double SCALE = 6.0;

        static Core::Tensor3d ComputeResidual(const Core::Tensor3d& matrix,
                                              const Core::Tensor3d& rhs,
                                              const SystemMatrix& system);

        static double LocalResidual(const Core::Tensor3d& matrix,
                                    const Core::Tensor3d& rhs,
                                    const SystemMatrix& system,
                                    Eigen::Index i,
                                    Eigen::Index j,
                                    Eigen::Index k);

        static double LocalSolve(const Core::Tensor3d& matrix,
                                 const Core::Tensor3d& rhs,
                                 const SystemMatrix& system,
                                 Eigen::Index i,
                                 Eigen::Index j,
                                 Eigen::Index k);

        static double
        SumNeighbours(const Core::Tensor3d& matrix, Eigen::Index i, Eigen::Index j, Eigen::Index k);

        static double SumNeighbourCoefficients(const Core::Tensor3d& matrix,
                                               Eigen::Index i,
                                               Eigen::Index j,
                                               Eigen::Index k);

        static double Weight(Eigen::Index index, Eigen::Index size);

        static bool IsOnFace(Core::Face face,
                             const Core::Tensor3d::Dimensions& dimensions,
                             Eigen::Index i,
                             Eigen::Index j,
                             Eigen::Index k);

        static double FaceArea(Core::Face face,
                               const Core::Tensor3d::Dimensions& dimensions,
                               Eigen::Index i,
                               Eigen::Index j,
                               Eigen::Index k);

        static double ControlVolume(const Core::Tensor3d::Dimensions& dimensions,
                                    Eigen::Index i,
                                    Eigen::Index j,
                                    Eigen::Index k);

        static void ZeroFixedCells(Core::Tensor3d& matrix,
                                   const Core::Tensor3d& fixedTemperatureMask);
    };
}
