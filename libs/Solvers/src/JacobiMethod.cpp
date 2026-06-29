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
        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        int localIterations = 0;
        double errorMax = tolerance + 1.0;
        double errorMean = tolerance + 1.0;
        double errorRMS = tolerance + 1.0;

        double residualMax = tolerance + 1.0;
        double residualMean = tolerance + 1.0;
        double residualRMS = tolerance + 1.0;

        // Preallocate Eigen matrices for performance
        Eigen::MatrixXd oldTemperature(rows, cols);
        Eigen::ArrayXXd errorArray(rows, cols);
        Eigen::ArrayXXd residualArray(rows, cols);

        while (localIterations < maxIterations && residualMax > tolerance)
        {
            oldTemperature = *_temperatureMatrix;

            _temperatureMatrix->block(1, 1, rows - 2, cols - 2) =
                (oldTemperature.block(0, 1, rows - 2, cols - 2) +
                 oldTemperature.block(2, 1, rows - 2, cols - 2) +
                 oldTemperature.block(1, 0, rows - 2, cols - 2) +
                 oldTemperature.block(1, 2, rows - 2, cols - 2)) *
                0.25f;

            ApplyBoundaryConditions();

            UpdateResidualMatrix();

            errorArray = (*_temperatureMatrix - oldTemperature).array();
            errorMax = errorArray.abs().maxCoeff();
            errorMean = errorArray.abs().mean();
            errorRMS = std::sqrt(errorArray.square().mean());

            residualArray = (*_residualMatrix).array();
            residualMax = residualArray.abs().maxCoeff();
            residualMean = residualArray.abs().mean();
            residualRMS = std::sqrt(residualArray.square().mean());

            localIterations++;
            totalIterations++;

            auto error =
                std::make_shared<Core::ErrorMetric>(totalIterations, errorMax, errorMean, errorRMS);

            _errorMetrics.push_back(error);

            auto residual = std::make_shared<Core::ResidualMetric>(
                totalIterations, residualMax, residualMean, residualRMS);

            _residualMetrics.push_back(residual);
        }
    }

    void JacobiMethod::ComputeSimulation()
    {
        int rows = _parameters.Rows / std::pow(_parameters.Expansion, _parameters.NumExpansions);
        int columns =
            _parameters.Columns / std::pow(_parameters.Expansion, _parameters.NumExpansions);
        int totalIterations = 0;

        _temperatureMatrix = std::make_shared<Eigen::MatrixXd>(rows, columns);
        _temperatureMatrix->setZero();

        ApplyBoundaryConditions();
        IterateTemperature(
            _parameters.MaxInitialIterations, _parameters.InitialTolerance, totalIterations);

        for (int i = 0; i < _parameters.NumExpansions; i++)
        {
            ExpandMatrix();
            ApplyBoundaryConditions();
            IterateTemperature(
                _parameters.MaxExpandedIterations, _parameters.ExpandedTolerance, totalIterations);
        }
    }
}
