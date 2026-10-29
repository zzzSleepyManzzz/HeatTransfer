#include <catch2/catch_all.hpp>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/IMultigridSolver.h"
#include "HeatTransfer/Solvers/ISolver.h"

namespace
{
    // Exposes ISolver's protected multigrid helpers
    class DummySolver : public HeatTransfer::Solvers::IMultigridSolver
    {
    public:
        using IMultigridSolver::CoarsenMask;
        using IMultigridSolver::Prolongate;
        using IMultigridSolver::Restrict;
        using ISolver::InsulatedMask;
        using ISolver::IsInsulated;

    private:
        void Smoothen(Eigen::MatrixXd&, const Eigen::MatrixXd&, const InsulatedMask&) override {}
    };
}

namespace HeatTransfer::Tests
{
    TEST_CASE("Restrict and Prolongate give the coarse and fine sizes")
    {
        auto [rows, cols] =
            GENERATE(table<Eigen::Index, Eigen::Index>({{65, 65}, {64, 64}, {9, 2000}}));

        const Eigen::MatrixXd fine = Eigen::MatrixXd::Constant(rows, cols, 2.5);
        const Eigen::MatrixXd coarse = DummySolver::Restrict(fine);

        REQUIRE(coarse.rows() == rows / 2 + 1);
        REQUIRE(coarse.cols() == cols / 2 + 1);

        const Eigen::MatrixXd back = DummySolver::Prolongate(
            Eigen::MatrixXd::Constant(coarse.rows(), coarse.cols(), 2.5), rows, cols);

        REQUIRE(back.rows() == rows);
        REQUIRE(back.cols() == cols);
        REQUIRE((back.array() - 2.5).abs().maxCoeff() < 1e-12);
    }

    TEST_CASE("Prolongate reproduces a linear field")
    {
        // Coarse node I sits on fine node 2I
        Eigen::MatrixXd coarse(17, 21);
        for (Eigen::Index J = 0; J < coarse.cols(); J++)
            for (Eigen::Index I = 0; I < coarse.rows(); I++)
                coarse(I, J) = 2.0 * I - 3.0 * (2.0 * J) + 1.0;

        const Eigen::MatrixXd fine = DummySolver::Prolongate(coarse, 32, 40);

        for (Eigen::Index j = 0; j < fine.cols(); j++)
            for (Eigen::Index i = 0; i < fine.rows(); i++)
                REQUIRE(fine(i, j) == Catch::Approx(i - 3.0 * j + 1.0));
    }

    TEST_CASE("Restrict is the transpose of Prolongate divided by 4")
    {
        auto [rows, cols] = GENERATE(table<Eigen::Index, Eigen::Index>({{33, 33}, {40, 31}}));

        Eigen::MatrixXd u = Eigen::MatrixXd::Random(rows, cols);
        u.row(0).setZero();
        u.row(rows - 1).setZero();
        u.col(0).setZero();
        u.col(cols - 1).setZero();

        Eigen::MatrixXd v = Eigen::MatrixXd::Random(rows / 2 + 1, cols / 2 + 1);
        v.row(0).setZero();
        v.row(v.rows() - 1).setZero();
        v.col(0).setZero();
        v.col(v.cols() - 1).setZero();

        const double coarseSide = 4.0 * (DummySolver::Restrict(u).array() * v.array()).sum();
        const double fineSide = (u.array() * DummySolver::Prolongate(v, rows, cols).array()).sum();

        REQUIRE(coarseSide == Catch::Approx(fineSide).epsilon(1e-12));
    }

    TEST_CASE("CoarsenMask keeps even lines and thickens odd ones")
    {
        DummySolver::InsulatedMask fine = DummySolver::InsulatedMask::Constant(33, 33, false);
        fine.row(0).setConstant(true);
        fine.row(32).setConstant(true);
        fine.col(0).setConstant(true);
        fine.col(32).setConstant(true);
        fine.block(8, 4, 1, 20).setConstant(true);  // even row: one coarse line
        fine.block(4, 15, 20, 1).setConstant(true); // odd column: two coarse lines

        const auto coarse = DummySolver::CoarsenMask(fine);

        REQUIRE(coarse.rows() == 17);
        REQUIRE(coarse.row(0).all());
        REQUIRE(coarse.col(16).all());

        REQUIRE(coarse(4, 5));
        REQUIRE_FALSE(coarse(3, 5));
        REQUIRE_FALSE(coarse(5, 5));

        REQUIRE(coarse(6, 7));
        REQUIRE(coarse(6, 8));
        REQUIRE_FALSE(coarse(6, 6));
        REQUIRE_FALSE(coarse(6, 9));
    }
}
