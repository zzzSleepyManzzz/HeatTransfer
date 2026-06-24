#include "HeatTransfer/Solvers/SuccessiveOverRelaxation.h"

namespace HeatTransfer::Solvers
{
    SuccessiveOverRelaxation::SuccessiveOverRelaxation(
        const Core::SimulationParameters& parameters,
        const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, boundaryCondition)
    {
    }

    void SuccessiveOverRelaxation::IterateTemperature(int maxIterations,
                                                      double tolerance,
                                                      double relaxationFactor,
                                                      int& totalIterations)
    {
        auto& T = *_temperatureMatrix;

        const Eigen::Index rows = T.rows();
        const Eigen::Index cols = T.cols();

        int localIterations = 0;
        double errorMax = tolerance + 1.0;
        double errorMean = tolerance + 1.0;
        double errorRMS = tolerance + 1.0;

        // Preallocate Eigen matrices for performance
        Eigen::MatrixXd oldTemperature(rows, cols);
        Eigen::ArrayXXd errorArray(rows, cols);

        while (localIterations < maxIterations && errorMax > tolerance)
        {
            oldTemperature = T;

            for (auto i = 1u; i < rows - 1; i++)
            {
                for (auto j = 1u; j < cols - 1; j++)
                {
                    T(i, j) = (1 - relaxationFactor) * T(i, j) +
                              relaxationFactor * 0.25f *
                                  (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1));
                }
            }

            ApplyBoundaryConditions();

            errorArray = (T - oldTemperature).array();
            errorMax = errorArray.abs().maxCoeff();
            errorMean = errorArray.abs().mean();
            errorRMS = std::sqrt(errorArray.square().mean());

            localIterations++;
            totalIterations++;

            auto error = std::make_shared<Core::IterationAndError>(
                totalIterations, errorMax, errorMean, errorRMS);

            _errors.push_back(error);
        }
    }

    void SuccessiveOverRelaxation::ComputeSimulation()
    {
        int rows = _parameters.rows / std::pow(_parameters.expansion, _parameters.numExpansions);
        int columns =
            _parameters.columns / std::pow(_parameters.expansion, _parameters.numExpansions);
        int totalIterations = 0;

        _temperatureMatrix = std::make_shared<Eigen::MatrixXd>(rows, columns);
        _temperatureMatrix->setZero();

        ApplyBoundaryConditions();
        IterateTemperature(_parameters.maxInitialIterations,
                           _parameters.initialTolerance,
                           _parameters.relaxationFactor,
                           totalIterations);

        for (int i = 0; i < _parameters.numExpansions; i++)
        {
            ExpandMatrix();
            ApplyBoundaryConditions();
            IterateTemperature(_parameters.maxExpandedIterations,
                               _parameters.expandedTolerance,
                               _parameters.relaxationFactor,
                               totalIterations);
        }
    }
}
