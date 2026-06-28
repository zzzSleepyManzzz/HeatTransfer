#include "HeatTransfer/Solvers/GaussSeidelMethod.h"

namespace HeatTransfer::Solvers
{
    GaussSeidelMethod::GaussSeidelMethod(const Core::SimulationParameters& parameters,
                                         const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, boundaryCondition)
    {
    }

    void
    GaussSeidelMethod::IterateTemperature(int maxIterations, double tolerance, int& totalIterations)
    {
        auto& T = *_temperatureMatrix;

        const Eigen::Index rows = T.rows();
        const Eigen::Index cols = T.cols();

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

        while (localIterations < maxIterations && errorMax > tolerance)
        {
            oldTemperature = T;

            for (auto i = 0u; i < rows; i++)
            {
                for (auto j = 0u; j < cols; j++)
                {
                    if (IsInsulated(i, j))
                    {
                        T(i, j) = InsulatedValue(i, j);
                    }
                    else
                    {
                        T(i, j) = 0.25f * (T(i - 1, j) + T(i + 1, j) + T(i, j - 1) + T(i, j + 1));
                    }
                }
            }

            UpdateResidualMatrix();

            errorArray = (T - oldTemperature).array();
            errorMax = errorArray.abs().maxCoeff();
            errorMean = errorArray.abs().mean();
            errorRMS = std::sqrt(errorArray.square().mean());

            residualArray = (*_residualMatrix).array();
            residualMax = residualArray.abs().maxCoeff();
            residualMean = residualArray.abs().mean();
            residualRMS = std::sqrt(residualArray.square().mean());

            localIterations++;
            totalIterations++;

            auto error = std::make_shared<Core::IterationAndError>(
                totalIterations, errorMax, errorMean, errorRMS);

            _errors.push_back(error);

            auto residual = std::make_shared<Core::ResidualMetric>(
                totalIterations, residualMax, residualMean, residualMean);

            _residualMetrics.push_back(residual);
        }
    }

    void GaussSeidelMethod::ComputeSimulation()
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
