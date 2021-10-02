#include <stdlib.h>
#include <iostream>
#include <string>
#include <functional>
#include "sdl2_include.h"

#include "TWindow.h"
#include "TSdlWrapper.h"
#include "TRenderer.h"
#include "TSurface.h"
#include "TTexture.h"
#include "TDrawer.h"
#include "TParams.h"
#include "TCellRectangles.h"
#include "TCell.h"
#include "TGameField.h"
#include "TCellsChain.h"
#include "TSnakeGame.h"
#include "TFieldDrawer.h"

using namespace io_submodule;
using namespace game_backend;

int main( int argc, char **argv )
{
    TSdlWrapper::getInstance();

    //! Main window params
    size_t wholeWidth = 640;
    size_t wholeHeight = 480;
    size_t xStartBias = 10;
    size_t yStartBias = 50;
    size_t activeGameFieldWidth = wholeWidth - xStartBias * 2;
    size_t activeGameFieldHeight = wholeHeight - yStartBias - 10;
    
    //! Main window params
    TRectangleDescription mainWindowParams;
    mainWindowParams.width = wholeWidth;
    mainWindowParams.height = wholeHeight;
    mainWindowParams.isFilled = true;
    mainWindowParams.color = { 0, 0, 0, 0xFF };

    //! Area for cells
    TRectangleDescription activeGameField;
    activeGameField.xStart = xStartBias;
    activeGameField.yStart = yStartBias;
    activeGameField.width = activeGameFieldWidth;
    activeGameField.height = activeGameFieldHeight;
    activeGameField.color = { 0, 0, 0, 0xFF };
    activeGameField.isFilled = true;

    //! Border around area for cells
    TRectangleDescription gameFieldBound = activeGameField;
    gameFieldBound.xStart -= 1;
    gameFieldBound.yStart -= 1;
    gameFieldBound.width += 2;
    gameFieldBound.height += 2;
    gameFieldBound.isFilled = false;
    gameFieldBound.color = { 0xFF, 0xFF, 0xFF, 0xFF };

    //! Cells params
    TCellsFieldParams cellsFieldParams;
    cellsFieldParams.xCellsCount = 31;
    cellsFieldParams.yCellsCount = 21;
    cellsFieldParams.cellHeight = 20;
    cellsFieldParams.cellWidth = 20;
    
    //! Create window, renderer and drawer
    TWindow wnd( "Main", mainWindowParams );
    TRenderer renderer( wnd );
    TDrawer drawer( renderer );

    //! Draw static objects
    drawer.draw( mainWindowParams );
    drawer.draw( activeGameField );
    drawer.draw( gameFieldBound );

    //! Calc cells coordinates
    //! Mapping (xCell, yCell) to (xWindow, yWindow)
    //! where [x|y]Cell - cells coordinate in 2d array of cells
    //! [x|y]Window - cells coordinate in pixels for corresponding window
    TCellRectangles cellRectangles;
    cellRectangles.setCellsFieldParams( activeGameField, cellsFieldParams );
    cellRectangles.calcGrid();

    //! Game backend
    TSnakeGame snakeGame( { cellsFieldParams.yCellsCount, cellsFieldParams.xCellsCount }, 5 );

    TFieldDrawer fDrawer( snakeGame.gameField, drawer, cellRectangles );
    fDrawer.draw();
    drawer.updateScreen();

    bool gameOver = false;
    
    // Main loop
    bool quit = false;
    SDL_Event exitEvent;

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
                    snakeGame.turn( { -1, 0 } );
                    
                if ( keyValue == SDLK_DOWN || keyValue == SDLK_s )
                    snakeGame.turn( { 1, 0 } );

                if ( keyValue == SDLK_LEFT || keyValue == SDLK_a )
                    snakeGame.turn( { 0, -1 } );

                if ( keyValue == SDLK_RIGHT || keyValue == SDLK_d )
                    snakeGame.turn( { 0, 1 } );
            }
        }

        if ( !gameOver )
        {
            gameOver = snakeGame.step();

            if ( gameOver )
                cout << "Game over, score: " << snakeGame.snake.snakeCells.size() << endl;
        }
        
        fDrawer.draw();
        
        SDL_Delay( 100 );
    }

    TSdlWrapper::deteteInstance();

   return 0;
};
