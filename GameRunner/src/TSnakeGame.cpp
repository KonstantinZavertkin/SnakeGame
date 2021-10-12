#include "sdl2_include.h"
#include "TSnakeGame.h"

#include <algorithm>
#include <iostream>

TSnakeGame::TSnakeGame( pair<size_t, size_t> fieldSize, size_t snakeLength ) 
    : gameField( fieldSize.first, fieldSize.second ), snake( gameField )
{
    snake.initCellsChain( { 1, 1 }, snakeLength );
};

TSnakeGame::~TSnakeGame()
{
};

bool TSnakeGame::step()
{
    snake.step();
    checkFood();
    return snake.isGameOver();
};

void TSnakeGame::turn( pair<int, int> rotateVector )
{
    snake.turn( rotateVector );
};

void TSnakeGame::checkFood()
{
    // Is required?
    bool isRequiredFood = true;

    for ( auto line : gameField.field )
    {
        for ( auto cell : line )
            if ( cell.currentState == game_backend::TCellStates::eatStateKey )
                isRequiredFood = false;
    }

    if ( isRequiredFood )
    {
        auto x = rand() % gameField.field.size();
        auto y = rand() % gameField.field[0].size();
        gameField.field[x][y].currentState = game_backend::TCellStates::eatStateKey;
    }
};

void TSnakeGame::gameThread()
{
    bool quit = false;
    performStep = true;

    while ( !quit )
    {
        syncPoint.lock();

        if ( ( clockCounter % 4 ) == 0 )
            performStep = true;

        if ( performStep )
        {
            if ( !rotationsQueue.empty() )
            {
                turn( rotationsQueue.front() );
                rotationsQueue.pop_front();
            }

            quit = step();
            performStep = false;
        }

        syncPoint.unlock();

        if ( quit )
            cout << "Game over, score: " << snake.snakeCells.size() << endl;

        fDrawer->draw();

        if ( 50 - static_cast<int>( snake.snakeCells.size() ) < 0 )
            SDL_Delay( 0 );
        else
            SDL_Delay( 50 - snake.snakeCells.size() );

        clockCounter++;
    }
};

void TSnakeGame::ioThread()
{
    SDL_Event exitEvent;
    bool quit = false;

    pair<int, int> vectorUp = { -1, 0 };
    pair<int, int> vectorDown = { 1, 0 };
    pair<int, int> vectorLeft = { 0, -1 };
    pair<int, int> vectorRight = { 0, 1 };
    pair<int, int> vectorLast = { 0, 0 };
    pair<int, int> vectorNext = { 0, 0 };

    while ( !quit )
    {
        while ( SDL_PollEvent( &exitEvent ) != 0 )
        {
            if ( exitEvent.type == SDL_QUIT )
                quit = true;

            if ( exitEvent.type == SDL_KEYDOWN )
            {
                auto keyValue = exitEvent.key.keysym.sym;

                if ( keyValue == SDLK_UP || keyValue == SDLK_w )
                {
                    vectorNext = vectorUp;
                }
    
                if ( keyValue == SDLK_DOWN || keyValue == SDLK_s )
                {
                    vectorNext = vectorDown;
                }

                if ( keyValue == SDLK_LEFT || keyValue == SDLK_a )
                {
                    vectorNext = vectorLeft;
                }

                if ( keyValue == SDLK_RIGHT || keyValue == SDLK_d )
                {
                    vectorNext = vectorRight;
                }

                syncPoint.lock();

                if ( vectorNext == vectorLast )
                {
                    performStep = true;
                }
                else
                {
                    rotationsQueue.push_back( vectorNext );

                    while ( rotationsQueue.size() > 2 )
                        rotationsQueue.pop_front();

                    vectorLast = vectorNext;
                }

                syncPoint.unlock();
            }
        }
        
        SDL_Delay( 1 );
    }
};
