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
            return {.TopEdge = 450.0,
                    .BottomEdge = 350.0,
                    .LeftEdge = 450.0,
                    .RightEdge = 350.0,
                    .InnerSquare = 400.0,
                    .CenterPoint = 250.0};
        }
    };
}