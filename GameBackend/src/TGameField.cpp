#include "TGameField.h"
#include <iostream>
#include <iomanip>

namespace game_backend
{
    using namespace std;

    TGameField::TGameField( size_t linesCount, size_t columnsCount )
    {
        for ( size_t i = 0; i < linesCount; ++i )
        {
            field.push_back( vector<TCell>() );

            for ( size_t j = 0; j < columnsCount; ++j )
            {
                field[i].push_back( TCell() );
                auto& cell = field[i][j];
                cell.currentState = TCellStates::backgroundStateKey;
                cell.moveDirection = { 0, 0 };
                cell.moveDirectionDelta = { 0, 0 };
                cell.thisCoordinates = { i, j };
            }
        }

        fieldSize = make_pair( linesCount, columnsCount );
    };

    TGameField::~TGameField()
    {};

    void TGameField::checkFood()
    {
        // Is required?
        bool isRequiredFood = true;

        for ( auto line : field )
        {
            for ( auto cell : line )
                if ( cell.currentState == TCellStates::eatStateKey )
                    isRequiredFood = false;
        }

        if ( isRequiredFood )
        {
            auto x = rand() % field.size();
            auto y = rand() % field[0].size();
            field[x][y].currentState = TCellStates::eatStateKey;
        }
    }

    void TGameField::debugPrint()
    {
        cout << "   ";
        
        for ( size_t j = 0; j < field[0].size(); ++j )
        {
            cout << setw(3) << j;
        }

        cout << endl;

        for ( size_t i = 0; i < field.size(); ++i )
        {
            cout << setw( 3 ) << i;
            cout << "|";

            for ( size_t j = 0; j < field[i].size(); ++j )
            {
                auto cell = field[i][j];

                if ( cell.currentState == TCellStates::backgroundStateKey )
                    cout << setw(2) << " ";
                
                if ( cell.currentState == TCellStates::snakeBodyStateKey )
                    cout << setw(2) << "O";
                
                if ( cell.currentState == TCellStates::snakeHeadStateKey )
                    cout << setw(2) << "@";
                
                if ( cell.currentState == TCellStates::snakeTailStateKey )
                    cout << setw(2) << "*";

                cout << "|";
            }

            cout << endl;
        }

        cout << endl;
    };
}
