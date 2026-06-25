#pragma once

namespace HeatTransfer::Core
{
    struct BoundaryConditions
    {
        double topEdge;
        double bottomEdge;
        double leftEdge;
        double rightEdge;
        double innerSquare;
        double centerPoint;

        constexpr static const BoundaryConditions Default()
        {
            return {.topEdge = 100.0,
                    .bottomEdge = 0.0,
                    .leftEdge = 100.0,
                    .rightEdge = 0.0,
                    .innerSquare = 50.0,
                    .centerPoint = -100.0};
        }
    };
}