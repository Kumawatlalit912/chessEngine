#include "board.h"


namespace Chess {
    Square::Square(Piece p, Color c){
        piece = p;
        piece_color = c;
    }

    Square::Square(){
        piece = Piece::out_of_board;
        piece_color = Color::none;
    }


    void Board::set(const size_t where, Square s) {
        if (where >= board_size)
            return;
        squares[where] = s;
    }

    Square Board::get(const size_t where) const {
        if (where >= board_size)
            return Square (Piece::out_of_board, Color::none);
        return squares[where];
    }

    void init_classic_board(Board& b) {
        // Place pawns
        for(size_t i = 0; i < 8; ++i){
            b.set(31 /*col 1, 2nd row*/ + i, Square(Piece::pawn, Color::black));
            b.set(81 /*col 1, 7th row*/ + i, Square(Piece::pawn, Color::white));
        }

        // Place the rest but with s/kings/queens
        int w = 0;
        for( auto p : {Piece::rook_castle, Piece::knight, Piece::bishop, Piece::queen} ) {
            b.set(21+w, Square(p, Color::black));
            b.set(28 /*col 8, 1st row*/ - w, Square(p, Color::black));
            b.set(91 /*col 1, 8th row*/ + w, Square(p, Color::white));
            b.set(98 /*col 8, 8th row*/ - w, Square(p, Color::white));
            ++w;
        }

        // Place kings
        b.set(25, Square(Piece::king_castle, Color::black));
        b.set(95, Square(Piece::king_castle, Color::white));

        // Empty squares inbetween the pieces
        for(int x=0; x < 8; ++x)
            for(int y=0; y < 4; ++y)
                b.set(41+x+y*10,Square(Piece::none, Color::none));
    }

}
