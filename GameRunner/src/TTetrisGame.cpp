#include <iostream>
#include <algorithm>
#include "TTetrisGame.h"

TTetrisGame::TTetrisGame( pair<size_t, size_t> fieldSize )
    : gameField( fieldSize.first, fieldSize.second )
{
};

TTetrisGame::~TTetrisGame()
{

}

optional<size_t> TTetrisGame::checkFullLines()
{
    optional<size_t> maybeLineIndex;

    auto isNotVoidCell = []( const TCell& cell )
    {
        return ( cell.currentState != TCellStates::backgroundStateKey ) && ( cell.canBeMoved == false);
    };

    for ( auto i = gameField.field.size(); i > 0; i-- )
    {
        const auto& currentLine = gameField.field[i - 1];

        if ( all_of( currentLine.begin(), currentLine.end(), isNotVoidCell ) )
        {
            maybeLineIndex = i - 1;
            break;
        }  
    }

    return maybeLineIndex;
};

void TTetrisGame::gameThread()
{
    size_t id = 0;
    size_t timeToSleep = 500;

    bool blockStep = false;

    while ( !quit )
    {
        syncPoint.lock();

        if ( !pauseGame )
        {
            if ( allBlocks.empty() )
                allBlocks.push_back( createFigure( id++ ) );
            
            if ( !allBlocks.empty() )
                if ( !allBlocks.back().canMove )
                    allBlocks.push_back( createFigure( id++ ) );

            if ( allBlocks.back().isGameOver() )
            {
                quit = true;
                cout << "Game over" << endl;
            }

            const auto maybeFullLines = checkFullLines();

            if ( maybeFullLines )
            {
                gameField.scrollField( { 1, 0 }, maybeFullLines );
                timeToSleep = 100;
            }
            else
            {
                timeToSleep = 500;

                if ( !allBlocks.empty() )
                {
                    allBlocks.back().step();

                    if ( checkFullLines() )
                        timeToSleep = 100;
                }
            }
                

            //! TODO ������� �������� ����� �� �������
        }
        
        syncPoint.unlock();

        SDL_Delay( timeToSleep );
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

    bool figureFallSignal = false;

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

                if ( keyValue != SDLK_ESCAPE )
                {
                    if ( keyValue == SDLK_UP || keyValue == SDLK_w )
                        vectorNext = vectorUp;

                    if ( keyValue == SDLK_DOWN || keyValue == SDLK_s )
                        vectorNext = vectorDown;

                    if ( keyValue == SDLK_LEFT || keyValue == SDLK_a )
                        vectorNext = vectorLeft;

                    if ( keyValue == SDLK_RIGHT || keyValue == SDLK_d )
                        vectorNext = vectorRight;

                    if ( keyValue == SDLK_SPACE )
                        figureFallSignal = true;
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
                            if ( vectorNext == vectorUp )
                            {
                                allBlocks.back().turn( {0, 0} );
                            }
                            else
                            {
                                allBlocks.back().moveDirection = vectorNext;
                                allBlocks.back().step();
                            }
                        }

                        vectorNext = vectorDefault;
                    }

                    if ( figureFallSignal )
                    {
                        figureFallSignal = false;

                        while ( allBlocks.back().canMove )
                            allBlocks.back().step();
                    }
                }

                syncPoint.unlock();
            }
        }

        syncPoint.lock();
        quitLocal = quit;
        fDrawer->draw();
        syncPoint.unlock();

        SDL_Delay( 1 );
    }

    cout << "Tetris io thread done" << endl;
};

TCellsBlock TTetrisGame::createFigure( const size_t id )
{
    cout << "Create figure " << id << endl;
    vector<pair<size_t, size_t>> v0 { {0, 0}, {0, 1}, {0, 2}, {1, 1} };
    vector<pair<size_t, size_t>> v1 { {0, 0}, {0, 1}, {0, 2}, {0, 3} };
    vector<pair<size_t, size_t>> v2 { {0, 0}, {0, 1}, {0, 2}, {1, 0} };
    vector<pair<size_t, size_t>> v3 { {0, 0}, {1, 0}, {1, 1}, {1, 2} };
    vector<pair<size_t, size_t>> v4 { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
    vector<pair<size_t, size_t>> v5 { {0, 0}, {0, 1}, {1, 1}, {1, 2} };
    vector<pair<size_t, size_t>> v6 { {0, 1}, {0, 2}, {1, 0}, {1, 1} };

    TCellsBlock block( gameField );

    size_t idLocal = id % 7;

    if ( idLocal == 0 )
    {
        block.initFigure( {0, 5}, v0, TCellStates::blueColorStateKey, id + 1 );
    }
        
    if ( idLocal == 1 )
    {
        block.initFigure( {0, 5}, v1, TCellStates::greenColorStateKey, id + 1 );
        block.setRotatePoint( { 0, block.getRotatePoint().second + 1 } );
    }

    if ( idLocal == 2 )
    {
        block.initFigure( {0, 5}, v2, TCellStates::redColorStateKey, id + 1 );
    }

    if ( idLocal == 3 )
    {
        block.initFigure( {0, 5}, v3, TCellStates::cyanColorStateKey, id + 1 );
    }

    if ( idLocal == 4 )
    {
        block.initFigure( {0, 5}, v4, TCellStates::magentaColorStateKey, id + 1 );
    }

    if ( idLocal == 5 )
    {
        block.initFigure( {0, 5}, v5, TCellStates::yellowColorStateKey, id + 1 );
    }

    if ( idLocal == 6 )
    {
        block.initFigure( {0, 5}, v6, TCellStates::orangeColorStateKey, id + 1 );
    }

    return block;
};
