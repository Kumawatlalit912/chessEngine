/*
    // File: main.cpp

    TODO: don't allow moves that leaves king in check, and count check-mate as victory. Hence, perft will be off by a few.
    TODO: prompt pawn promotion, instead of the default queen.
    TODO: handle cases of no valid moves
*/

#include <iostream>
#include <ctime>
#include "board.h"
#include "move.h"
#include "gui.h"
#include "ai.h"

using namespace Chess;


unsigned long perft(Board &b, Boardhistory &h, int depth, Color turn) {
    turn = (turn == Color::white) ? Color::black : Color::white;
    if(!depth)
        return 1;
    int leafs = 0;
    for(Move m : valid_moves(b, turn)){
        if(b.get(m.to).piece == Piece::king || b.get(m.to).piece == Piece::king_castle){
            ++leafs;
            continue;
        }
        do_move(m,b,h);
        leafs += perft(b,h,depth-1,turn);
        undo_move(b,h);
    }
    return leafs;
}


int main(){
    std::cout<<"\nChesscpu 1.0\n\n";
    std::cout<<"Commands:\nyxyx: fromto move.\n0: regret move (last AI move will be reverted as well).\n1: change color (AI will make this move)\n2: exit.\n\n";
    Board b;
    Boardhistory h;
    init_classic_board(b);

    Color turn = Color::white;
    Color ai_color = Color::black;

    bool ai_has_king = true;
    bool human_has_king = true;


    if(false) {
        unsigned long t = time(0);
        std::cout<<"DEBUG: Perft(5) result (expecting 4897256): "<<perft(b,h,5,Color::black);
        t = time(0) - t;
        std::cout<<"\nTime "<<t<<"\n";
        return 0;
    }

    if(false){
        Move mv;
        unsigned long t = time(0);
        ai_move(b, h, turn, 7, mv);
        t = time(0) - t;
        std::cout<<"\nAI Time: "<<t<<"\n";
        return 0;
    }

    Move mv {0,0};
    while(ai_has_king && human_has_king){
        print_board(b, mv);
        if(turn == ai_color)
            ai_move(b, h, turn, 7, mv);
        else
            mv = read_move(valid_moves(b, turn), turn);

        if(mv.from == 0) {
            undo_move(b,h);
            undo_move(b,h);
            continue;
        } else if (mv.from == 1) {
            ai_color = ai_color == Color::white ? Color::black : Color::white;
            continue;
        } else if (mv.from == 2) {
            human_has_king = false;
            break;
        }

        do_move(mv, b, h);
        turn = turn == Color::white ? Color::black : Color::white;

        ai_has_king = human_has_king = false;
        for(size_t i = 21; i < 99; ++i) { // board.h about these magic numbers
            if(b.get(i).piece == Piece::king || b.get(i).piece == Piece::king_castle) {
                if(b.get(i).piece_color == ai_color) {
                    ai_has_king = true;
                } else {
                    human_has_king = true;
                }
            }
        }

    }
    print_board(b, mv);

    std::cout<<"\n\n";
    if(!human_has_king)
        std::cout<<"You lose.";
    if(!ai_has_king)
        std::cout<<"You win.";
    std::cout<<"\n\nBye!\n\n";
}
