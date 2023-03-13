// See board.h on "magic numbers"
// File: ai.cpp

#include "ai.h"
#include <random>
#include <algorithm>
#include <iostream>


namespace Chess {
    std::mt19937 mt(time(0));


    void moveorder(MoveSet& ms) {
        std::random_shuffle(ms.begin(), ms.end());
    }

    static int evaluate_leaf(const Board& b) {
        int sum = 0;
        for(size_t i = 21; i < 99; ++i) {
            if(b.get(i).piece_color == Color::none)
                continue;
            int c = b.get(i).piece_color == Color::white ? 1 : -1;
            int v = 0;
            switch(b.get(i).piece){
                case Piece::pawn:
                case Piece::pawn_en_passant:
                    v = 1;
                    break;
                case Piece::rook:
                case Piece::rook_castle:
                    v = 5;
                    break;
                case Piece::knight:
                case Piece::bishop:
                    v = 3;
                    break;
                case Piece::queen:
                    v = 9;
                    break;
                default:
                break;
            }
            sum += c*v;
        }
        return sum;
    }

    static Color flipped_turn(Color turn) {
        if(turn == Color::white)
            return Color::black;
        return Color::white;
    }

    int ai_move(Board& b, Boardhistory& bh, Color turn, int depth, Move& _bm, int alpha, int beta) {
            /*
            MoveSet valid = valid_moves(b, turn);
            _bm = valid[mt()%valid.size()];
            return 0;
            */

        Move best_move;
        int best_score = turn == Color::white ? -300 : 300;
        if(!depth)
            return evaluate_leaf(b);

        MoveSet vmoves = valid_moves(b, turn);
        moveorder(vmoves);
                //int progress = 0; // Temporary hack to show progress
        for(Move m : vmoves){
                //if(depth == 8) // Temporary hack to show progress
                //  std::cout<<"\r"<<++progress<<"/"<<vmoves.size()<<std::flush;

            if(b.get(m.to).piece == Piece::king || b.get(m.to).piece == Piece::king_castle){
                best_score = turn==Color::white ? 200+depth : -200-depth;
                best_move = m;
                break;
            }

            do_move(m, b, bh);
            int new_score = ai_move(b, bh, flipped_turn(turn), depth-1, _bm, alpha, beta);
            undo_move(b,bh);

            if((turn == Color::white && new_score > best_score) || (turn == Color::black && new_score < best_score)){
                best_move = m;
                best_score = new_score;

                if(turn == Color::black) {
                    beta = new_score;
                    if(beta <= alpha)
                        break;
                }

                if(turn == Color::white) {
                    alpha = new_score;
                    if(alpha >= beta)
                        break;
                }
            }

        }
        _bm = best_move;
        return best_score;

    }
}
