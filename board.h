#pragma once

#include <array>

namespace Chess{

    const size_t board_size = 10*12;

    enum class Color { white, black, none };

    enum class Piece {
        king,       // a king without castle potential
        king_castle,    // a king with castle potential
        queen,
        pawn,       // a pawn without en passant potential
        pawn_en_passant, // a pawn with en passant potential
        rook,
        rook_castle,
        knight,
        bishop,
        none,
        out_of_board    // Illegal position
    };

    struct Square {
        Color piece_color;
        Piece piece;
        Square(Piece, Color);
        Square();
    };

    class Board {
        private:
            std::array<Square, board_size> squares;
        public:
            void set(const size_t where, Square);
            Square get(const size_t where) const;
    };

    void init_classic_board(Board&);

}
