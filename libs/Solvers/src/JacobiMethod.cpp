#include "HeatTransfer/Solvers/JacobiMethod.h"

namespace HeatTransfer::Solvers
{
    JacobiMethod::JacobiMethod(const Core::SimulationParameters& parameters,
                               const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, boundaryCondition)
    {
    }

    void JacobiMethod::IterateTemperature(int maxIterations, double tolerance, int& totalIterations)
    {
        const Eigen::Index rows = T.rows();
        const Eigen::Index cols = T.cols();

        int localIterations = 0;
        double errorMax = tolerance + 1.0;
        double errorMean = tolerance + 1.0;
        double errorRMS = tolerance + 1.0;

        // Preallocate Eigen matrices for performance
        Eigen::MatrixXd oldT(rows, cols);
        Eigen::ArrayXXd errorArray(rows, cols);

        while (localIterations < maxIterations && errorMax > tolerance)
        {
            oldT = T;

            T.block(1, 1, rows - 2, cols - 2) =
                (oldT.block(0, 1, rows - 2, cols - 2) + oldT.block(2, 1, rows - 2, cols - 2) +
                 oldT.block(1, 0, rows - 2, cols - 2) + oldT.block(1, 2, rows - 2, cols - 2)) *
                0.25f;

            ApplyBoundaryConditions();

            errorArray = (T - oldT).array();
            errorMax = errorArray.abs().maxCoeff();
            errorMean = errorArray.abs().mean();
            errorRMS = std::sqrt(errorArray.square().mean());

            localIterations++;
            totalIterations++;

            auto error = Core::IterationAndError{.iteration = totalIterations,
                                                 .errorMax = errorMax,
                                                 .errorMean = errorMean,
                                                 .errorRMS = errorRMS};

            _errors.push_back(std::move(error));
        }
    }

    void JacobiMethod::ComputeSimulation()
    {
        int rows = _parameters.rows / std::pow(_parameters.expansion, _parameters.numExpansions);
        int columns =
            _parameters.columns / std::pow(_parameters.expansion, _parameters.numExpansions);
        int totalIterations = 0;

        T = Eigen::MatrixXd::Zero(rows, columns);
        ApplyBoundaryConditions();
        IterateTemperature(
            _parameters.maxInitialIterations, _parameters.initialTolerance, totalIterations);

        for (int i = 0; i < _parameters.numExpansions; i++)
        {
            ExpandMatrix();
            ApplyBoundaryConditions();
            IterateTemperature(
                _parameters.maxExpandedIterations, _parameters.expandedTolerance, totalIterations);
        }
    }
}
