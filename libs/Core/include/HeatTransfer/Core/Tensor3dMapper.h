#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/Tensor3d.h"

namespace HeatTransfer::Core
{
    class Tensor3dMapper
    {
    public:
        Tensor3dMapper() = delete;

        static Eigen::Map<Eigen::VectorXd> AsVector(Tensor3d& matrix);
        static Eigen::Map<const Eigen::VectorXd> AsVector(const Tensor3d& matrix);
    };
}
