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
            return {.TopEdge = 100.0,
                    .BottomEdge = 0.0,
                    .LeftEdge = 100.0,
                    .RightEdge = 0.0,
                    .InnerSquare = 50.0,
                    .CenterPoint = -100.0};
        }
    };
}