#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    ISolver::ISolver(const Core::SimulationParameters& parameters,
                     const Core::BoundaryConditions& boundaryCondition)
        : _parameters(parameters), _boundaryCondition(boundaryCondition)
    {
    }

    void ISolver::PrintError()
    {
        for (const auto& error : _errors)
        {
            std::cout << std::format("Iteration  : {}", error->iteration) << std::endl;
            std::cout << std::format("Max error  : {}", error->errorMax) << std::endl;
            std::cout << std::format("Mean error : {}", error->errorMean) << std::endl;
            std::cout << std::format("RMS error  : {}", error->errorRMS) << std::endl;
            std::cout << std::endl;
        }
    }

    const std::vector<std::shared_ptr<Core::IterationAndError>>& ISolver::GetErrors()
    {
        return _errors;
    }

    std::shared_ptr<Eigen::MatrixXd> ISolver::GetTemperatureMatrix()
    {
        return _temperatureMatrix;
    }

    void ISolver::ApplyBoundaryConditions()
    {
        auto& T = *_temperatureMatrix;

        const Eigen::Index rows = T.rows();
        const Eigen::Index cols = T.cols();

        T.row(0) = Eigen::VectorXd::Constant(cols, _boundaryCondition.topEdge);
        T.row(rows - 1) = Eigen::VectorXd::Constant(cols, _boundaryCondition.bottomEdge);
        T.col(0) = Eigen::VectorXd::Constant(rows, _boundaryCondition.leftEdge);
        T.col(cols - 1) = Eigen::VectorXd::Constant(rows, _boundaryCondition.rightEdge);

        // Set inner square
        const int h1 = std::floor(rows / 4);
        const int h2 = std::floor(3 * rows / 4);
        const int h3 = std::floor(cols / 4);
        const int h4 = std::floor(3 * cols / 4);
        const int c1 = std::floor(rows / 2) - 1;
        const int c2 = std::floor(cols / 2) - 1;

        T(Eigen::seq(h1, h2), h3) =
            Eigen::VectorXd::Constant(h2 - h1 + 1, _boundaryCondition.innerSquare);

        T(Eigen::seq(h1, h2), h4) =
            Eigen::VectorXd::Constant(h2 - h1 + 1, _boundaryCondition.innerSquare);

        T(h1, Eigen::seq(h3, h4)) =
            Eigen::RowVectorXd::Constant(h4 - h3 + 1, _boundaryCondition.innerSquare);

        T(h2, Eigen::seq(h3, h4)) =
            Eigen::RowVectorXd::Constant(h4 - h3 + 1, _boundaryCondition.innerSquare);

        T(c1, c2) = _boundaryCondition.centerPoint;
    }

    void ISolver::ExpandMatrix()
    {
        auto& T = *_temperatureMatrix;

        int expansionFactor = _parameters.expansion;

        const Eigen::Index rows = T.rows();
        const Eigen::Index cols = T.cols();
        const Eigen::Index newRows = rows * expansionFactor;
        const Eigen::Index newCols = cols * expansionFactor;

        Eigen::MatrixXd oldTemperature = T;
        T = Eigen::MatrixXd(newRows, newCols);

        // Use block operations for efficiency
        for (auto i = 0u; i < rows; i++)
        {
            for (auto j = 0u; j < cols; j++)
            {
                T.block(i * expansionFactor, j * expansionFactor, expansionFactor, expansionFactor)
                    .setConstant(oldTemperature(i, j));
            }
        }
    }
}
