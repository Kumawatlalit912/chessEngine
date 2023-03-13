#include <iostream>
#include "move.h"

// Read move.h for specifications

namespace Chess {

    Move::Move(ssize_t _from, ssize_t _to) {
        from = _from;
        to = _to;
    }

    Move::Move(){}

    void undo_move(Board& b, Boardhistory& ml){
        if(!ml.size())
            return;
        if(ml.back().where == move_done)
            ml.pop_back();

        while(ml.size() && ml.back().where != move_done){
            b.set( ml.back().where, ml.back().old_square );
            ml.pop_back();
        }
    }

    static void do_change(Board& b, Boardhistory& bh, size_t where, Square new_square) {
        BoardChange change;
        change.old_square = b.get(where);
        change.where = where;
        bh.push_back(change);
        b.set(where, new_square);
    }

    void do_move(Move m, Board& b, Boardhistory& bh, Piece pawn_promotion){
        // Pawn promotion
        if(b.get(m.from).piece == Piece::pawn && m.to/10 == 2)
                do_change(b, bh, m.from, Square(pawn_promotion, Color::white));

        if(b.get(m.from).piece == Piece::pawn && m.to/10 == 9)
                do_change(b, bh, m.from, Square(pawn_promotion, Color::black));

        // Move rook if castling
        if(b.get(m.from).piece == Piece::king_castle && (m.from-m.to == 2 || m.from-m.to == -2)){
            if(m.to == 23){
                do_change(b, bh, 21, Square(Piece::none, Color::none));
                do_change(b, bh, 24, Square(Piece::rook, Color::black));
            }

            if(m.to == 27){
                do_change(b, bh, 28, Square(Piece::none, Color::none));
                do_change(b, bh, 26, Square(Piece::rook, Color::black));
            }

            if(m.to == 93){
                do_change(b, bh, 91, Square(Piece::none, Color::none));
                do_change(b, bh, 94, Square(Piece::rook, Color::white));
            }

            if(m.to == 97){
                do_change(b, bh, 98, Square(Piece::none, Color::none));
                do_change(b, bh, 96, Square(Piece::rook, Color::white));
            }
        }


        Piece pawn_replaced = b.get(m.to).piece;
        // Regular piece move
        do_change(b, bh, m.to, b.get(m.from));
        do_change(b, bh, m.from, Square(Piece::none, Color::none));


         // Pawn replaced empty square
        if(b.get(m.to).piece == Piece::pawn && pawn_replaced == Piece::none) {
            // En passant move
            if(b.get(m.from-1).piece == Piece::pawn_en_passant && (m.from-m.to == 11 || m.from-m.to == -9))
                do_change(b, bh, m.from-1, Square(Piece::none, Color::none));
            else if (b.get(m.from+1).piece == Piece::pawn_en_passant  && (m.from-m.to == 9 || m.from-m.to == -11))
                do_change(b, bh, m.from+1, Square(Piece::none, Color::none));
        }


        // clear flag of pawns with en passant potential
        for(size_t i=1; i < 9; ++i){
            if(b.get(50+i).piece == Piece::pawn_en_passant)
                do_change(b, bh, 50+i, Square(Piece::pawn, Color::black));
            if(b.get(60+i).piece == Piece::pawn_en_passant)
                do_change(b, bh, 60+i, Square(Piece::pawn, Color::white));
        }

        // Give two-square moved pawns en passant flag
        if(b.get(m.to).piece == Piece::pawn) {
            if(m.from/10 == 3 && m.to/10 == 5)
                do_change(b, bh, m.to, Square(Piece::pawn_en_passant, Color::black));

            if(m.from/10 == 8 && m.to/10 == 6)
                do_change(b, bh, m.to, Square(Piece::pawn_en_passant, Color::white));
        }

        // Lose castling potential
        if(b.get(m.to).piece == Piece::king_castle)
            do_change(b, bh, m.to, Square(Piece::king, b.get(m.to).piece_color));
        if(b.get(m.to).piece == Piece::rook_castle)
            do_change(b, bh, m.to, Square(Piece::rook, b.get(m.to).piece_color));

        BoardChange done;
        done.where = move_done;
        bh.push_back(done);
    }

    MoveSet valid_moves(Board& b, Color turn){
        MoveSet ret;
        Color enemy_color = (turn == Color::white) ? Color::black : Color::white;
        int pawn_dir = (turn == Color::white) ? -1 : 1;


        for(size_t from = 21 /*skip padding*/; from < 99 /*padding after this*/ ; ++from){
            if(b.get(from).piece_color == turn){
                switch(b.get(from).piece){
                    case Piece::king_castle:
                        if(from == 95 && b.get(96).piece == Piece::none && b.get(97).piece == Piece::none && b.get(98).piece == Piece::rook_castle)
                            ret.push_back(Move(from, 97));
                        if(from == 25 && b.get(26).piece == Piece::none && b.get(27).piece == Piece::none && b.get(28).piece == Piece::rook_castle)
                            ret.push_back(Move(from, 27));                  
                        if(from == 95 && b.get(94).piece == Piece::none && b.get(93).piece == Piece::none && b.get(92).piece == Piece::none && b.get(91).piece == Piece::rook_castle)
                            ret.push_back(Move(from, 93));
                        if(from == 25 && b.get(24).piece == Piece::none && b.get(23).piece == Piece::none && b.get(22).piece == Piece::none && b.get(21).piece == Piece::rook_castle)
                            ret.push_back(Move(from, 23));
                    // fallthrough
                    case Piece::king:
                        for(auto to : {from-11, from-10, from-9,from-1, from+1, from+9, from+10,from+11}) {
                            if(b.get(to).piece_color == turn || b.get(to).piece == Piece::out_of_board)
                                continue;
                            ret.push_back(Move(from, to));
                        }
                        break;
                    case Piece::pawn:
                        if(b.get(from+pawn_dir*11).piece_color == enemy_color)
                            ret.push_back(Move(from, from+pawn_dir*11));
                        if(b.get(from+pawn_dir*9).piece_color == enemy_color)
                            ret.push_back(Move(from, from+pawn_dir*9));
                        if(b.get(from+pawn_dir*10).piece == Piece::none)
                            ret.push_back(Move(from, from+pawn_dir*10));
                        if(b.get(from+pawn_dir*10).piece == Piece::none && b.get(from+pawn_dir*20).piece == Piece::none){
                            size_t row = from/10;
                            if((row == 3 && pawn_dir == 1) || (row == 8 && pawn_dir == -1))
                                ret.push_back(Move(from, from+pawn_dir*20));
                        }
                        if(b.get(from-1).piece == Piece::pawn_en_passant && pawn_dir == -1)
                            ret.push_back( Move(from, from-11) );
                        if(b.get(from+1).piece == Piece::pawn_en_passant && pawn_dir == -1)
                            ret.push_back( Move(from, from-9) );
                        if(b.get(from-1).piece == Piece::pawn_en_passant && pawn_dir == 1)
                            ret.push_back( Move(from, from+9) );
                        if(b.get(from+1).piece == Piece::pawn_en_passant && pawn_dir == 1)
                            ret.push_back( Move(from, from+11) );
                        break;
                    case Piece::knight:
                        for(auto to : {from+8, from+12, from+19, from+21, from-8, from-12, from-21, from-19}) {
                            if(b.get(to).piece_color == turn || b.get(to).piece == Piece::out_of_board)
                                continue;
                            ret.push_back(Move(from, to));
                        }
                        break;
                    case Piece::queen:
                    //fallthrough
                    case Piece::rook:
                    case Piece::rook_castle:
                        for(int dd : {1,-1,10,-10})
                            for(int d=dd; b.get(from+d).piece_color != b.get(from).piece_color && b.get(from+d).piece != Piece::out_of_board ;d+=dd) {
                                ret.push_back(Move(from, from+d));
                                if(b.get(from+d).piece != Piece::none)
                                    break;
                            }
                    if(b.get(from).piece != Piece::queen)
                        break;
                    case Piece::bishop:
                        for(int dd : {9,11,-9,-11})
                            for(int d=dd; b.get(from+d).piece_color != b.get(from).piece_color && b.get(from+d).piece != Piece::out_of_board ;d+=dd) {
                                ret.push_back(Move(from, from+d));
                                if(b.get(from+d).piece != Piece::none)
                                    break;
                            }
                        break;
                    default: // warnings unwanted
                    break;
                }
            }
        }
        return ret;
    }
}
