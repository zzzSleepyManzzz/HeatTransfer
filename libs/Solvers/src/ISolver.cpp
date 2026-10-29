#include "HeatTransfer/Solvers/ISolver.h"

#include <algorithm>
#include <numbers>

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
        int totalIterations = 0;

        _temperatureMatrix = std::make_shared<Eigen::MatrixXd>(rows, columns);
        _temperatureMatrix->setZero();

        _residualMatrix = std::make_shared<Eigen::MatrixXd>(rows, columns);
        _residualMatrix->setZero();

        ApplyBoundaryConditions();
        BuildLevels();

        IterateTemperature(_parameters.MaxExpandedIterations,
                           _parameters.ExpandedResidualTolerance,
                           _parameters.ExpandedErrorTolerance,
                           totalIterations);
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

        auto finalTargetResidual = finalResidual->ResidualMax;

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

        return finalTargetResidual <= _parameters.ExpandedResidualTolerance;
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

    void ISolver::IterateTemperature(int maxIterations,
                                     double residualTolerance,
                                     double errorTolerance,
                                     int& totalIterations)
    {
        ZoneScoped;

        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        int localIterations = 0;
        double errorMax = errorTolerance + 1.0;
        double errorMean = errorTolerance + 1.0;
        double errorRMS = errorTolerance + 1.0;

        double residualMax = residualTolerance + 1.0;
        double residualMean = residualTolerance + 1.0;
        double residualRMS = residualTolerance + 1.0;

        std::vector<double> residualTracker = {residualMax, residualMean, residualRMS};
        std::vector<double> errorTracker = {errorMax, errorMean, errorRMS};

        auto indexR = (size_t)_parameters.ResidualTrackingMetric;
        auto indexE = (size_t)_parameters.ErrorTrackingMetric;

        // Preallocate Eigen matrices for performance
        Eigen::MatrixXd oldTemperature(rows, cols);
        Eigen::ArrayXXd errorArray(rows, cols);
        Eigen::ArrayXXd residualArray(rows, cols);

        while (localIterations < maxIterations && residualTracker[indexR] > residualTolerance &&
               errorTracker[indexE] > errorTolerance)
        {
            oldTemperature = *_temperatureMatrix;

            VCycle();

            UpdateResidualMatrix();

            errorArray = (*_temperatureMatrix - oldTemperature).array();
            errorMax = errorArray.abs().maxCoeff();
            errorMean = errorArray.abs().mean();
            errorRMS = std::sqrt(errorArray.square().mean());

            residualArray = (*_residualMatrix).array();
            residualMax = residualArray.abs().maxCoeff();
            residualMean = residualArray.abs().mean();
            residualRMS = std::sqrt(residualArray.square().mean());

            residualTracker = {residualMax, residualMean, residualRMS};
            errorTracker = {errorMax, errorMean, errorRMS};

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

    void ISolver::BuildLevels()
    {
        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        InsulatedMask insulated(rows, cols);
        for (Eigen::Index j = 0; j < cols; j++)
            for (Eigen::Index i = 0; i < rows; i++)
                insulated(i, j) = IsInsulated(i, j, rows, cols);

        // Level 0: the heat source term, on free nodes only
        auto q_g = _materialProperties.InternalHeatGeneration;
        auto k = _materialProperties.ThermalConductivity;
        auto h = 1.0 / std::max(rows, cols);

        Eigen::MatrixXd heating = Eigen::MatrixXd::Constant(rows, cols, 0.25 * pow(h, 2) * q_g / k);
        heating.array() = insulated.select(0.0, heating.array());

        _errorStack = {Eigen::MatrixXd()};
        _residualStack = {heating};
        _insulatedStack = {insulated};

        // Coarser levels, halving until the next grid would be too small
        while (CoarseSize(_insulatedStack.back().rows()) >= MinCoarseSize &&
               CoarseSize(_insulatedStack.back().cols()) >= MinCoarseSize)
        {
            _insulatedStack.push_back(CoarsenMask(_insulatedStack.back()));

            const auto& mask = _insulatedStack.back();
            _errorStack.push_back(Eigen::MatrixXd::Zero(mask.rows(), mask.cols()));
            _residualStack.push_back(Eigen::MatrixXd::Zero(mask.rows(), mask.cols()));
        }
    }

    Eigen::MatrixXd& ISolver::LevelMatrix(std::size_t level)
    {
        return level == 0 ? *_temperatureMatrix : _errorStack[level];
    }

    void ISolver::VCycle()
    {
        ZoneScoped;

        const std::size_t numLevels = _insulatedStack.size();

        // Down: smooth, then restrict the residual to the next level. The equation is
        // scaled by h^2 / 4 and h doubles, so the coarse right-hand side is 4x the residual.
        for (std::size_t i = 0; i + 1 < numLevels; i++)
        {
            auto& E = LevelMatrix(i);

            for (int s = 0; s < PreSmoothingSweeps; s++)
                Smoothen(E, _residualStack[i], _insulatedStack[i]);

            auto& R = _residualStack[i + 1];
            R = 4.0 * Restrict(ComputeResidual(E, _residualStack[i], _insulatedStack[i]));
            R.array() = _insulatedStack[i + 1].select(0.0, R.array());

            _errorStack[i + 1].setZero();
        }

        if (numLevels > 1)
            SolveCoarsest();
        else
            Smoothen(*_temperatureMatrix, _residualStack[0], _insulatedStack[0]);

        // Up: add the interpolated correction (never on insulated nodes), then smooth
        for (std::size_t i = numLevels - 1; i-- > 0;)
        {
            auto& E = LevelMatrix(i);

            Eigen::MatrixXd correction = Prolongate(_errorStack[i + 1], E.rows(), E.cols());
            correction.array() = _insulatedStack[i].select(0.0, correction.array());
            E += correction;

            for (int s = 0; s < PostSmoothingSweeps; s++)
                Smoothen(E, _residualStack[i], _insulatedStack[i]);
        }
    }

    void ISolver::SolveCoarsest()
    {
        auto& E = _errorStack.back();
        const auto& R = _residualStack.back();
        const auto& insulated = _insulatedStack.back();

        const Eigen::Index rows = E.rows();
        const Eigen::Index cols = E.cols();

        for (int s = 0; s < CoarseSweeps; s++)
        {
            for (Eigen::Index j = 1; j < cols - 1; j++)
            {
                for (Eigen::Index i = 1; i < rows - 1; i++)
                {
                    if (!insulated(i, j))
                    {
                        Smoothen(E, R, insulated);
                    }
                }
            }
        }
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

    // Coarse node I sits on fine node 2I, so the outer boundary (and any line at an even
    // index) lands on the coarse grid. For even n the coarse grid ends one fine step past
    // the edge, which is close enough for a correction.
    Eigen::Index ISolver::CoarseSize(Eigen::Index n)
    {
        return n / 2 + 1;
    }

    // A coarse node is insulated if any fine node around it (2I +/- 1, 2J +/- 1) is, so a
    // line at an odd fine index becomes two coarse lines instead of vanishing
    ISolver::InsulatedMask ISolver::CoarsenMask(const InsulatedMask& mask)
    {
        const Eigen::Index rows = CoarseSize(mask.rows());
        const Eigen::Index cols = CoarseSize(mask.cols());

        InsulatedMask coarse = InsulatedMask::Constant(rows, cols, false);
        coarse.row(0).setConstant(true);
        coarse.row(rows - 1).setConstant(true);
        coarse.col(0).setConstant(true);
        coarse.col(cols - 1).setConstant(true);

        for (Eigen::Index J = 1; J < cols - 1; J++)
        {
            for (Eigen::Index I = 1; I < rows - 1; I++)
            {
                const Eigen::Index numRows = std::min<Eigen::Index>(3, mask.rows() - (2 * I - 1));
                const Eigen::Index numCols = std::min<Eigen::Index>(3, mask.cols() - (2 * J - 1));

                coarse(I, J) = mask.block(2 * I - 1, 2 * J - 1, numRows, numCols).any();
            }
        }

        return coarse;
    }

    // Full weighting: 1/16 [1 2 1; 2 4 2; 1 2 1] around fine node (2I, 2J)
    Eigen::MatrixXd ISolver::Restrict(const Eigen::MatrixXd& matrix)
    {
        ZoneScoped;

        const Eigen::Index rows = matrix.rows();
        const Eigen::Index cols = matrix.cols();

        Eigen::MatrixXd restricted = Eigen::MatrixXd::Zero(CoarseSize(rows), CoarseSize(cols));

        // Fine nodes past the edge (even sizes only) count as 0
        auto M = [&](Eigen::Index i, Eigen::Index j)
        { return (i < rows && j < cols) ? matrix(i, j) : 0.0; };

        for (Eigen::Index J = 1; J < restricted.cols() - 1; J++)
        {
            for (Eigen::Index I = 1; I < restricted.rows() - 1; I++)
            {
                const Eigen::Index i = 2 * I;
                const Eigen::Index j = 2 * J;

                restricted(I, J) =
                    (4.0 * M(i, j) + 2.0 * (M(i - 1, j) + M(i + 1, j) + M(i, j - 1) + M(i, j + 1)) +
                     M(i - 1, j - 1) + M(i - 1, j + 1) + M(i + 1, j - 1) + M(i + 1, j + 1)) /
                    16.0;
            }
        }

        return restricted;
    }

    // Bilinear interpolation: even fine nodes copy a coarse node, odd ones average neighbours
    Eigen::MatrixXd
    ISolver::Prolongate(const Eigen::MatrixXd& matrix, Eigen::Index rows, Eigen::Index cols)
    {
        ZoneScoped;

        Eigen::MatrixXd prolongated(rows, cols);

        for (Eigen::Index j = 0; j < cols; j++)
        {
            const Eigen::Index J0 = j / 2;
            const Eigen::Index J1 = (j % 2 == 0) ? J0 : J0 + 1;

            for (Eigen::Index i = 0; i < rows; i++)
            {
                const Eigen::Index I0 = i / 2;
                const Eigen::Index I1 = (i % 2 == 0) ? I0 : I0 + 1;

                prolongated(i, j) =
                    0.25 * (matrix(I0, J0) + matrix(I1, J0) + matrix(I0, J1) + matrix(I1, J1));
            }
        }

        return prolongated;
    }
}
