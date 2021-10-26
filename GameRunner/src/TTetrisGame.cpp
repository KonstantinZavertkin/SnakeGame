#include <iostream>
#include "TTetrisGame.h"

TTetrisGame::TTetrisGame( pair<size_t, size_t> fieldSize )
    : gameField( fieldSize.first, fieldSize.second )
{
};

TTetrisGame::~TTetrisGame()
{

};

void TTetrisGame::gameThread()
{
    bool quitLocal = false;

    while ( !quit )
    {
        syncPoint.lock();

        if ( !pauseGame )
        {
            if ( allBlocks.empty() )
                allBlocks.push_back( createFigure( 0 ) );
            
            if ( !allBlocks.empty() )
                if ( !allBlocks.back().canMove )
                    allBlocks.push_back( createFigure( 0 ) );

            if ( !allBlocks.empty() )
            {
                allBlocks.back().step();
            }
        }
        
        syncPoint.unlock();

        SDL_Delay( 500 );
    }
    
    cout << "Tetris game thread done" << endl;
};

void TTetrisGame::ioThread()
{
    SDL_Event exitEvent;
    SDL_Delay( 125 );
    bool pauseLocal = false;
    bool quitLocal = false;

    pair<int, int> vectorUp = { -1, 0 };
    pair<int, int> vectorDown = { 1, 0 };
    pair<int, int> vectorLeft = { 0, -1 };
    pair<int, int> vectorRight = { 0, 1 };
    pair<int, int> vectorLast = { 0, 0 };
    pair<int, int> vectorNext = { 0, 0 };
    pair<int, int> vectorDefault = { 0, 0 };

    while ( !quitLocal )
    {
        while ( SDL_PollEvent( &exitEvent ) != 0 )
        {
            if ( exitEvent.type == SDL_QUIT )
            {
                quitLocal = true;
                syncPoint.lock();
                quit = quitLocal;
                syncPoint.unlock();
            }

            if ( exitEvent.type == SDL_KEYDOWN )
            {
                auto keyValue = exitEvent.key.keysym.sym;

                if ( keyValue != SDLK_SPACE )
                {
                    if ( keyValue == SDLK_UP || keyValue == SDLK_w )
                        vectorNext = vectorUp;

                    if ( keyValue == SDLK_DOWN || keyValue == SDLK_s )
                        vectorNext = vectorDown;

                    if ( keyValue == SDLK_LEFT || keyValue == SDLK_a )
                        vectorNext = vectorLeft;

                    if ( keyValue == SDLK_RIGHT || keyValue == SDLK_d )
                        vectorNext = vectorRight;
                }
                else
                    pauseLocal = !pauseLocal;

                syncPoint.lock();

                pauseGame = pauseLocal;

                if ( !pauseLocal )
                {
                    if ( vectorNext != vectorDefault )
                    {
                        if ( !allBlocks.empty() )
                        {
                            allBlocks.back().moveDirection = vectorNext;
                            allBlocks.back().step();
                        }
                    }
                }

                syncPoint.unlock();
            }
        }

        syncPoint.lock();
        fDrawer->draw();
        syncPoint.unlock();

        SDL_Delay( 1 );
    }

    cout << "Tetris io thread done" << endl;
};

TCellsBlock TTetrisGame::createFigure( size_t id )
{
    vector<vector<pair<size_t, size_t>>> v0 { { {0, 0}, {0, 1}, {0, 2} }, { {1, 1} } };
    vector<vector<pair<size_t, size_t>>> v1 { { {0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4} } };

    TCellsBlock block( gameField );

    if ( id == 0 )
    {
        block.initFigure( {0, 5}, v0, TCellStates::blueColorStateKey );
    }
        
    if ( id == 1 )
    {
        block.initFigure( {0, 5}, v1, TCellStates::greenColorStateKey );
    }

    return block;
};