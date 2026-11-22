#include "HeatTransfer/Core/Tensor3dMapper.h"

namespace HeatTransfer::Core
{
    Eigen::Map<Eigen::VectorXd> Tensor3dMapper::AsVector(Tensor3d& matrix)
    {
        return {matrix.data(), matrix.size()};
    }

    Eigen::Map<const Eigen::VectorXd> Tensor3dMapper::AsVector(const Tensor3d& matrix)
    {
        return {matrix.data(), matrix.size()};
    }
}
