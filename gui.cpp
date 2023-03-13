// File: gui.cpp
// This is quick, ugly, pragmatic, temporary.

#include <iostream>
#include <cctype>
#include <map>
#include "gui.h"



namespace Chess {

    using namespace std;

    static const std::map<Piece, char> sprites = { 
        { Piece::pawn,      'A' },
        { Piece::pawn_en_passant,   'P' },
        { Piece::rook,      'H' },
        { Piece::rook_castle,   'H' },
        { Piece::knight,        'F' },
        { Piece::bishop,        'I' },
        { Piece::king,      'K' },
        { Piece::king_castle,   'K' },
        { Piece::none,      '+' },
        { Piece::out_of_board,      '#' },
        { Piece::queen,         'Q' }
    };

    void print_board(const Board& b, Move last_move) {
        cout<<"\n   1 2 3 4 5 6 7 8";
        for(ssize_t i=20; i < (ssize_t)board_size; ++i){
            if(i%10 == 9)
                continue;
            if(i/10 == 10)
                break;
            if(i%10 == 0) {
                cout<<"\n "<<i/10<<" ";
                continue;
            }
            char s = sprites.at(b.get(i).piece);
            if(b.get(i).piece_color == Color::black)
                s = tolower(s);
            cout<<s;
            if(last_move.from == i || last_move.to == i)
                cout<<'<';
            else
                cout<<' ';          
        }
        cout<<"\n"<<endl;
    }

    Move read_move(const MoveSet& valid_moves, Color turn) {
        if(cin.fail()) {
            cin.clear();
            string dummy;
            cin >> dummy;
        }

        int in;
        Move ret;
        cout<<"Your move, "<<( turn == Color::white ? "white" : "black" )<<": ";
        cin>>in;

        // Command to undo 1 or 2 moves (2 to revert AI+own)
        if(in == 0 || in == 1 || in == 2){
            ret.from = in;
            return ret;
        }

        ret.to  = in%10+in/10%10*10;
        in /= 100;
        ret.from  = in%10+in/10%10*10;

        for(const auto m : valid_moves)
            if(m.from == ret.from && m.to == ret.to)
                return ret;
        cout<<"Invalid move\n";
        return read_move(valid_moves, turn);
    }

}
