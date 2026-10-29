#include "HeatTransfer/Solvers/ISolver.h"

#include <algorithm>

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    ISolver::ISolver(const Core::SimulationParameters& parameters,
                     const Core::MaterialProperties& materialProperties,
                     const Core::BoundaryConditions& boundaryCondition)
        : _parameters(parameters)
        , _materialProperties(materialProperties)
        , _boundaryCondition(boundaryCondition)
    {
    }

    void ISolver::ComputeSimulation()
    {
        ZoneScoped;

        int rows = _parameters.Rows;
        int columns = _parameters.Columns;

        _temperatureMatrix = std::make_shared<Eigen::MatrixXd>(rows, columns);
        _temperatureMatrix->setZero();

        _residualMatrix = std::make_shared<Eigen::MatrixXd>(rows, columns);
        _residualMatrix->setZero();

        ApplyBoundaryConditions();
        BuildLevels();

        IterateTemperature();
    }

    void ISolver::PrintResidual()
    {
        for (const auto& residual : _residualMetrics)
        {
            std::cout << std::format("Iteration     : {}", residual->Iteration) << std::endl;
            std::cout << std::format("Max residual  : {}", residual->ResidualMax) << std::endl;
            std::cout << std::format("Mean residual : {}", residual->ResidualMean) << std::endl;
            std::cout << std::format("RMS residual  : {}", residual->ResidualRMS) << std::endl;
            std::cout << std::endl;
        }
    }

    bool ISolver::IsConverged()
    {
        using enum HeatTransfer::Core::TrackingMetric;

        auto finalResidual = _residualMetrics.back();

        double finalTargetResidual;

        switch (_parameters.ResidualTrackingMetric)
        {
            case MAX:
                finalTargetResidual = finalResidual->ResidualMax;
                break;
            case MEAN:
                finalTargetResidual = finalResidual->ResidualMean;
                break;
            case RMS:
                finalTargetResidual = finalResidual->ResidualRMS;
                break;
            default:
                throw std::runtime_error("Unknown residual tracking metric found");
        }

        return finalTargetResidual <= _parameters.ResidualTolerance;
    }

    const std::vector<std::shared_ptr<Core::ErrorMetric>>& ISolver::GetErrorMetrics()
    {
        return _errorMetrics;
    }

    const std::vector<std::shared_ptr<Core::ResidualMetric>>& ISolver::GetResidualMetrics()
    {
        return _residualMetrics;
    }

    std::shared_ptr<Eigen::MatrixXd> ISolver::GetTemperatureMatrix()
    {
        return _temperatureMatrix;
    }

    std::shared_ptr<Eigen::MatrixXd> ISolver::GetResidualMatrix()
    {
        return _residualMatrix;
    }

    void ISolver::ApplyBoundaryConditions()
    {
        auto& T = *_temperatureMatrix;

        const Eigen::Index rows = T.rows();
        const Eigen::Index cols = T.cols();

        T.row(0) = Eigen::VectorXd::Constant(cols, _boundaryCondition.TopEdge);
        T.row(rows - 1) = Eigen::VectorXd::Constant(cols, _boundaryCondition.BottomEdge);
        T.col(0) = Eigen::VectorXd::Constant(rows, _boundaryCondition.LeftEdge);
        T.col(cols - 1) = Eigen::VectorXd::Constant(rows, _boundaryCondition.RightEdge);

        // Set inner square
        const int h1 = std::floor(rows / 4);
        const int h2 = std::floor(3 * rows / 4);
        const int h3 = std::floor(cols / 4);
        const int h4 = std::floor(3 * cols / 4);
        const int c1 = std::floor(rows / 2) - 1;
        const int c2 = std::floor(cols / 2) - 1;

        T(Eigen::seq(h1, h2), h3) =
            Eigen::VectorXd::Constant(h2 - h1 + 1, _boundaryCondition.InnerSquare);

        T(Eigen::seq(h1, h2), h4) =
            Eigen::VectorXd::Constant(h2 - h1 + 1, _boundaryCondition.InnerSquare);

        T(h1, Eigen::seq(h3, h4)) =
            Eigen::RowVectorXd::Constant(h4 - h3 + 1, _boundaryCondition.InnerSquare);

        T(h2, Eigen::seq(h3, h4)) =
            Eigen::RowVectorXd::Constant(h4 - h3 + 1, _boundaryCondition.InnerSquare);

        T(c1, c2) = _boundaryCondition.CenterPoint;
    }

    void ISolver::IterateTemperature()
    {
        ZoneScoped;

        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        const int maxIterations = _parameters.MaxIterations;
        const double residualTolerance = _parameters.ResidualTolerance;
        const double errorTolerance = _parameters.ErrorTolerance;

        int iterations = 0;

        // Start above the tolerances so the loop runs at least once
        std::vector<double> residualTracker(3, residualTolerance + 1.0);
        std::vector<double> errorTracker(3, errorTolerance + 1.0);

        auto indexR = (size_t)_parameters.ResidualTrackingMetric;
        auto indexE = (size_t)_parameters.ErrorTrackingMetric;

        // Preallocate Eigen matrices for performance
        Eigen::MatrixXd oldTemperature(rows, cols);
        Eigen::ArrayXXd errorArray(rows, cols);
        Eigen::ArrayXXd residualArray(rows, cols);

        while (iterations < maxIterations && residualTracker[indexR] > residualTolerance &&
               errorTracker[indexE] > errorTolerance)
        {
            oldTemperature = *_temperatureMatrix;

            VCycle();

            UpdateResidualMatrix();

            errorArray = (*_temperatureMatrix - oldTemperature).array();
            const double errorMax = errorArray.abs().maxCoeff();
            const double errorMean = errorArray.abs().mean();
            const double errorRMS = std::sqrt(errorArray.square().mean());

            residualArray = (*_residualMatrix).array();
            const double residualMax = residualArray.abs().maxCoeff();
            const double residualMean = residualArray.abs().mean();
            const double residualRMS = std::sqrt(residualArray.square().mean());

            residualTracker = {residualMax, residualMean, residualRMS};
            errorTracker = {errorMax, errorMean, errorRMS};

            iterations++;

            auto error =
                std::make_shared<Core::ErrorMetric>(iterations, errorMax, errorMean, errorRMS);

            _errorMetrics.push_back(error);

            auto residual = std::make_shared<Core::ResidualMetric>(
                iterations, residualMax, residualMean, residualRMS);

            _residualMetrics.push_back(residual);
        }
    }

    bool ISolver::IsInsulated(Eigen::Index row,
                              Eigen::Index col,
                              Eigen::Index numRows,
                              Eigen::Index numCols)
    {
        if (row == 0 || row == numRows - 1 || col == 0 || col == numCols - 1)
            return true;

        // Inner square
        const int h1 = std::floor(numRows / 4);
        const int h2 = std::floor(3 * numRows / 4);
        const int h3 = std::floor(numCols / 4);
        const int h4 = std::floor(3 * numCols / 4);
        const int c1 = std::floor(numRows / 2) - 1;
        const int c2 = std::floor(numCols / 2) - 1;

        if ((row == h1 || row == h2) && col >= h3 && col <= h4)
            return true;

        if ((col == h3 || col == h4) && row >= h1 && row <= h2)
            return true;

        return row == c1 && col == c2;
    }

    void ISolver::UpdateResidualMatrix()
    {
        ZoneScoped;

        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        _residualMatrix = std::make_shared<Eigen::MatrixXd>(rows, cols);
        _residualMatrix->setZero();

        auto& T = *_temperatureMatrix;
        auto& R = *_residualMatrix;

        auto q_g = _materialProperties.InternalHeatGeneration;
        auto k = _materialProperties.ThermalConductivity;
        auto h = 1.0 / std::max(rows, cols);
        auto heating = Eigen::MatrixXd::Constant(rows - 2, cols - 2, pow(h, 2) * q_g / k);

        R.block(1, 1, rows - 2, cols - 2) =
            T.block(1, 1, rows - 2, cols - 2) -
            (T.block(0, 1, rows - 2, cols - 2) + T.block(2, 1, rows - 2, cols - 2) +
             T.block(1, 0, rows - 2, cols - 2) + T.block(1, 2, rows - 2, cols - 2) + heating) *
                0.25f;

        const int h1 = std::floor(rows / 4);
        const int h2 = std::floor(3 * rows / 4);
        const int h3 = std::floor(cols / 4);
        const int h4 = std::floor(3 * cols / 4);
        const int c1 = std::floor(rows / 2) - 1;
        const int c2 = std::floor(cols / 2) - 1;

        R.row(0) = Eigen::VectorXd::Constant(cols, 0);
        R.row(rows - 1) = Eigen::VectorXd::Constant(cols, 0);
        R.col(0) = Eigen::VectorXd::Constant(rows, 0);
        R.col(cols - 1) = Eigen::VectorXd::Constant(rows, 0);

        R(Eigen::seq(h1, h2), h3) = Eigen::VectorXd::Constant(h2 - h1 + 1, 0);
        R(Eigen::seq(h1, h2), h4) = Eigen::VectorXd::Constant(h2 - h1 + 1, 0);
        R(h1, Eigen::seq(h3, h4)) = Eigen::RowVectorXd::Constant(h4 - h3 + 1, 0);
        R(h2, Eigen::seq(h3, h4)) = Eigen::RowVectorXd::Constant(h4 - h3 + 1, 0);

        R(c1, c2) = 0;

        R = R.cwiseAbs().eval();
    }

    Eigen::MatrixXd ISolver::ComputeResidual(const Eigen::MatrixXd& matrix,
                                             const Eigen::MatrixXd& rhs,
                                             const InsulatedMask& insulated)
    {
        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        const auto& E = matrix;
        Eigen::MatrixXd R = Eigen::MatrixXd::Zero(rows, cols);

        R.block(1, 1, rows - 2, cols - 2) =
            rhs.block(1, 1, rows - 2, cols - 2) - E.block(1, 1, rows - 2, cols - 2) +
            (E.block(0, 1, rows - 2, cols - 2) + E.block(2, 1, rows - 2, cols - 2) +
             E.block(1, 0, rows - 2, cols - 2) + E.block(1, 2, rows - 2, cols - 2)) *
                0.25;

        R.array() = insulated.select(0.0, R.array());
        return R;
    }
}
