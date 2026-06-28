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
            std::cout << std::format("Iteration  : {}", error->Iteration) << std::endl;
            std::cout << std::format("Max error  : {}", error->ErrorMax) << std::endl;
            std::cout << std::format("Mean error : {}", error->ErrorMean) << std::endl;
            std::cout << std::format("RMS error  : {}", error->ErrorRMS) << std::endl;
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

    void ISolver::ExpandMatrix()
    {
        auto& T = *_temperatureMatrix;

        int expansionFactor = _parameters.Expansion;

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

    bool ISolver::IsInsulated(Eigen::Index row, Eigen::Index col)
    {
        auto& T = *_temperatureMatrix;

        const Eigen::Index numRows = T.rows();
        const Eigen::Index numCols = T.cols();

        if (row == 0 && col >= 0 && col < numCols)
            return true;

        if (row == numRows - 1 && col >= 0 && col < numCols)
            return true;

        if (col == 0 && row >= 0 && row < numRows)
            return true;

        if (col == numCols - 1 && row >= 0 && row < numRows)
            return true;

        // Set inner square
        const int h1 = std::floor(numRows / 4);
        const int h2 = std::floor(3 * numRows / 4);
        const int h3 = std::floor(numCols / 4);
        const int h4 = std::floor(3 * numCols / 4);
        const int c1 = std::floor(numRows / 2) - 1;
        const int c2 = std::floor(numCols / 2) - 1;

        if (row == h1 && col >= h3 && col <= h4)
            return true;

        if (row == h2 && col >= h3 && col <= h4)
            return true;

        if (col == h3 && row >= h1 && row <= h2)
            return true;

        if (col == h4 && row >= h1 && row <= h2)
            return true;

        if (row == c1 && col == c2)
            return true;

        return false;
    }

    double ISolver::InsulatedValue(Eigen::Index row, Eigen::Index col)
    {
        auto& T = *_temperatureMatrix;

        const Eigen::Index numRows = T.rows();
        const Eigen::Index numCols = T.cols();

        if (row == 0 && col >= 0 && col < numCols)
            return _boundaryCondition.TopEdge;

        if (row == numRows - 1 && col >= 0 && col < numCols)
            return _boundaryCondition.BottomEdge;

        if (col == 0 && row >= 0 && row < numRows)
            return _boundaryCondition.LeftEdge;

        if (col == numCols - 1 && row >= 0 && row < numRows)
            return _boundaryCondition.RightEdge;

        // Set inner square
        const int h1 = std::floor(numRows / 4);
        const int h2 = std::floor(3 * numRows / 4);
        const int h3 = std::floor(numCols / 4);
        const int h4 = std::floor(3 * numCols / 4);
        const int c1 = std::floor(numRows / 2) - 1;
        const int c2 = std::floor(numCols / 2) - 1;

        if (row == h1 && col >= h3 && col <= h4)
            return _boundaryCondition.InnerSquare;

        if (row == h2 && col >= h3 && col <= h4)
            return _boundaryCondition.InnerSquare;

        if (col == h3 && row >= h1 && row <= h2)
            return _boundaryCondition.InnerSquare;

        if (col == h4 && row >= h1 && row <= h2)
            return _boundaryCondition.InnerSquare;

        if (row == c1 && col == c2)
            return _boundaryCondition.CenterPoint;

        throw std::runtime_error("Asking for insulated value from non-insulated edge");
    }

    void ISolver::UpdateResidualMatrix()
    {
        const Eigen::Index rows = _temperatureMatrix->rows();
        const Eigen::Index cols = _temperatureMatrix->cols();

        _residualMatrix = std::make_shared<Eigen::MatrixXd>(rows, cols);
        _residualMatrix->setZero();

        auto& T = *_temperatureMatrix;
        auto& R = *_residualMatrix;

        R.block(1, 1, rows - 2, cols - 2) =
            T.block(1, 1, rows - 2, cols - 2) -
            (T.block(0, 1, rows - 2, cols - 2) + T.block(2, 1, rows - 2, cols - 2) +
             T.block(1, 0, rows - 2, cols - 2) + T.block(1, 2, rows - 2, cols - 2)) *
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

        R(c1, c2) = _boundaryCondition.CenterPoint;
    }
}
