#include "TMinesweeperGame.h"

TMinesweeperGame::TMinesweeperGame( TCoords fieldSize ) :
    gameField( fieldSize.first, fieldSize.second ), bestScoreStorage( pathToBestScoreFile )
{
    this->fieldSize = fieldSize;
}

void TMinesweeperGame::gameThread()
{
    bool quitLocal = false;

    while ( !quitLocal )
    {
        syncPoint.lock();
        quitLocal = quit;
        syncPoint.unlock();
        SDL_Delay( 1 );
    }
}

void TMinesweeperGame::ioThread()
{
    bestScore = bestScoreStorage.getScore();

    if ( !bestScore )
       bestScore = UINT_MAX;

    SDL_Event event;
    bool quitLocal = false;

    while ( !quitLocal )
    {
        while ( SDL_PollEvent( &event ) != 0 )
        {
            if ( event.type == SDL_QUIT )
                quitLocal = true;

            if ( event.type == SDL_KEYDOWN )
            {
                const auto keyValue = event.key.keysym.sym;

                if ( keyValue == SDLK_ESCAPE )
                {
                    syncPoint.lock();
                    const time_t timeBeforePause = time( nullptr );
                    
                    if ( pauseMenu->show() != 0 )
                        quitLocal = true;

                    const time_t timeAfterPause = time( nullptr );

                    if ( startTimeFlag )
                        secondsPause += difftime( timeAfterPause, timeBeforePause );

                    syncPoint.unlock();
                }
            }

            if ( event.type == SDL_MOUSEBUTTONDOWN )
            {
                if ( minesweeper.isGameOver() || minesweeper.isWin() )
                   continue;

                const auto x = event.button.x;
                const auto y = event.button.y;

                syncPoint.lock();

                if ( const auto cellCoords = cellRectangles->getCellCoords( x, y ) )
                {
                    if ( !startTimeFlag )
                    {
                        startTimeFlag = true;
                        startTime = time( nullptr );
                    }

                    const auto [cx, cy] = *cellCoords;

                    if ( event.button.button == SDL_BUTTON_LEFT )
                        minesweeper.performStep( cy, cx, 'a' );

                    if ( event.button.button == SDL_BUTTON_RIGHT )
                        minesweeper.performStep( cy, cx, 'f' );
                }

                syncPoint.unlock();
            }
        }

        syncPoint.lock();

        Field& mField = minesweeper.getField();

        for ( int i = 0; i < fieldSize.first; ++i )
        {
            for ( int j = 0; j < fieldSize.second; ++j )
            {
                if ( mField.isOpened( i, j ) )
                {
                    if ( mField.isBomb( i, j ) )
                    {
                        if ( minesweeper.isGameOver() )
                            gameField.field[i][j].currentState = "b";
                        else
                           gameField.field[i][j].currentState = "h";
                    }
                    else
                       gameField.field[i][j].currentState = to_string( mField.getValue( i, j ) );
                }
                else
                {
                    if ( mField.isFlag( i, j ) )
                       gameField.field[i][j].currentState = "f";
                    else
                       gameField.field[i][j].currentState = "h";
                }
            }
        }

        quit = quitLocal;

        if ( minesweeper.isGameOver() )
        {
            gameStatus->isVisible = true;
            gameStatus->setText( "Game over" );
            flagsCountDrawer->isVisible = false;
        }

        if ( minesweeper.isWin() )
        {
            gameStatus->isVisible = true;
            gameStatus->setText( "You win!" );
            flagsCountDrawer->isVisible = false;
        }

        if ( startTimeFlag )
        {
            if ( !minesweeper.isGameOver() && !minesweeper.isWin() )
            {
                const time_t currentTime = time( nullptr );
                seconds = difftime( currentTime, startTime ) - secondsPause;
                secondsDrawer->setText( to_string( static_cast<int>( seconds ) ) );
            }
        }

        timeDrawer->setText( "Time:" );

        currentScore = static_cast<unsigned>( seconds );

        const string bestTimePrefix = "Best time:  ";

        if ( bestScore != UINT_MAX )
            bestTimeDrawer->setText( bestTimePrefix + to_string( bestScore ) );
        else
            bestTimeDrawer->setText( bestTimePrefix + "----" );
        
        flagsCountDrawer->setText( "Flags: " +  to_string( minesweeper.getFlagsCount() ) );

        if ( !quitLocal )
            mainDrawer->draw();

        syncPoint.unlock();

        SDL_Delay( 1 );
    }

    if ( minesweeper.isWin() )
        if ( currentScore < bestScore )
            bestScoreStorage.setScore( currentScore );
}
