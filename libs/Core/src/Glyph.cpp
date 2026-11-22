#include "HeatTransfer/Core/Glyph.h"

#include <algorithm>
#include <stdexcept>

namespace HeatTransfer::Core
{
    bool Glyph::Contains(Letter letter, double s, double t)
    {
        // (s, t) both normalized between [0, 1]

        if (s < 0.0 || s > 1.0 || t < 0.0 || t > 1.0)
            return false;

        const auto& bitmap = GetBitmap(letter);

        const int row = std::min(ROWS - 1, (int)((1.0 - t) * ROWS));
        const int column = std::min(COLUMNS - 1, (int)(s * COLUMNS));

        return bitmap[row][column] == '#';
    }

    const std::array<std::string, Glyph::ROWS>& Glyph::GetBitmap(Letter letter)
    {
        switch (letter)
        {
            case Letter::H:
                return LETTER_H;
            case Letter::T:
                return LETTER_T;
            default:
                throw std::runtime_error("Invalid letter");
        }
    }
}
