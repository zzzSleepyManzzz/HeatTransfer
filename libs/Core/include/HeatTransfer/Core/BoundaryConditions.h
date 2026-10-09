#pragma once

namespace HeatTransfer::Core
{
    struct BoundaryConditions
    {
        double TopEdge;
        double BottomEdge;
        double LeftEdge;
        double RightEdge;
        double InnerSquare;
        double CenterPoint;

        constexpr static const BoundaryConditions Default()
        {
            return {.TopEdge = 200.0,
                    .BottomEdge = 100.0,
                    .LeftEdge = 200.0,
                    .RightEdge = 100.0,
                    .InnerSquare = 150.0,
                    .CenterPoint = 0.0};
        }
    };
}