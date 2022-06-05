#include <stdlib.h>
#include <iostream>
#include <string>
#include <functional>
#include <thread>
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
#include "TFontTTF.h"
#include "TFontDrawer.h"

#include "TTetrisGame.h"

#include "CommonTypes.hpp"

using namespace io_submodule;
using namespace game_backend;

//! TODO �������� ���������� ������

int main( int argc, char **argv )
{
    std::srand( time( 0 ) );
    TSdlWrapper::getInstance();

    //! Main window params
    size_t wholeWidth = 800;
    size_t wholeHeight = 600;
    size_t xStartBias = 200;
    size_t yStartBias = 60;
    size_t activeGameFieldWidth = wholeWidth - xStartBias * 2;
    size_t activeGameFieldHeight = wholeHeight - yStartBias - 10;

    string fontFile = "Samson.ttf";
    int fontSize = 22;
    
    //! Main window params
    TRectangleDescription mainWindowParams;
    mainWindowParams.width = wholeWidth;
    mainWindowParams.height = wholeHeight;
    mainWindowParams.isFilled = true;
    mainWindowParams.color = { 0, 0, 0, 0xFF };

    TRectangleDescription background;
    background.xStart = 0;
    background.yStart = 0;
    background.width = wholeWidth;
    background.height = wholeHeight;
    background.isFilled = true;
    background.color = { 0, 0, 0, 0xFF };

    //! Create window, renderer and renderer
    TWindow wnd( "Main", mainWindowParams );
    TRenderer renderer( wnd );

    //! Cells params
    TCellsFieldParams cellsFieldParams;
    cellsFieldParams.xCellsCount = 10;
    cellsFieldParams.yCellsCount = 20;
    cellsFieldParams.cellHeight = 25;
    cellsFieldParams.cellWidth = 25;

    TCellsFieldParams cellsInfoFieldParams;
    cellsInfoFieldParams.xCellsCount = 4;
    cellsInfoFieldParams.yCellsCount = 4;
    cellsInfoFieldParams.cellHeight = 15;
    cellsInfoFieldParams.cellWidth = 15;

    //! Area for main field
    TRectangleDescription activeGameField;
    activeGameField.xStart = xStartBias;
    activeGameField.yStart = yStartBias;
    activeGameField.width = activeGameFieldWidth;
    activeGameField.height = activeGameFieldHeight;
    activeGameField.color = { 0xA0, 0xA0, 0xA0, 0xFF };
    activeGameField.isFilled = true;

    //! Area for main field
    TRectangleDescription gameInfoField;
    gameInfoField.xStart = activeGameField.xStart + ( cellsFieldParams.xCellsCount + 1 ) * cellsFieldParams.cellWidth + 20;
    gameInfoField.yStart = 120;
    gameInfoField.width = cellsInfoFieldParams.xCellsCount * cellsInfoFieldParams.cellWidth;
    gameInfoField.height = cellsInfoFieldParams.yCellsCount * cellsInfoFieldParams.cellHeight;

    //! Border around the main area
    TRectangleDescription gameFieldBound;
    gameFieldBound.xStart = activeGameField.xStart - 1;
    gameFieldBound.yStart = activeGameField.yStart - 1;
    gameFieldBound.width = cellsFieldParams.xCellsCount * cellsFieldParams.cellWidth + 2;
    gameFieldBound.height = cellsFieldParams.yCellsCount * cellsFieldParams.cellHeight + 2;
    gameFieldBound.isFilled = false;
    gameFieldBound.color = { 0xFF, 0xFF, 0xFF, 0xFF };

    //! Border around the info area
    TRectangleDescription infoFieldBound;
    infoFieldBound.xStart = gameInfoField.xStart - 1;
    infoFieldBound.yStart = gameInfoField.yStart - 1;
    infoFieldBound.width = cellsInfoFieldParams.xCellsCount * cellsInfoFieldParams.cellWidth + 2;
    infoFieldBound.height = cellsInfoFieldParams.yCellsCount * cellsInfoFieldParams.cellHeight + 2;
    infoFieldBound.isFilled = false;
    infoFieldBound.color = { 0xFF, 0xFF, 0xFF, 0xFF };

    //! Calc cells coordinates
    //! Mapping (xCell, yCell) to (xWindow, yWindow)
    //! where [x|y]Cell - cells coordinate in 2d array of cells
    //! [x|y]Window - cells coordinate in pixels for corresponding window
    TCellRectangles mainFieldCellsGrid;
    mainFieldCellsGrid.setCellsFieldParams( activeGameField, cellsFieldParams );
    mainFieldCellsGrid.calcGrid();

    TCellRectangles infoFieldCellsGrid;
    infoFieldCellsGrid.setCellsFieldParams( gameInfoField, cellsInfoFieldParams );
    infoFieldCellsGrid.calcGrid();

    //! Game backend
    /*TSnakeGame snakeGame( { cellsFieldParams.yCellsCount, cellsFieldParams.xCellsCount }, 5 );

    TFieldDrawer snakeDrawer( snakeGame.gameField, renderer, cellRectangles );
    snakeDrawer.addStaticPrimitiveLast( gameFieldBound );

    snakeGame.mainFieldDrawer = &snakeDrawer;

    thread mainThr( &TSnakeGame::gameThread, &snakeGame );

    snakeGame.ioThread();

    mainThr.join();*/

    TTetrisGame tetris( { cellsFieldParams.yCellsCount, cellsFieldParams.xCellsCount } );

    TFieldDrawer tetrisDrawer( tetris.tetrisBackend.gameField, renderer, mainFieldCellsGrid );
    tetrisDrawer.cellsMapper = tetrisCellsMapper;
    tetrisDrawer.addStaticPrimitiveLast( gameFieldBound );

    TFieldDrawer infoFieldDrawer( tetris.tetrisBackend.nextFigureField, renderer, infoFieldCellsGrid );
    infoFieldDrawer.cellsMapper = tetrisCellsMapper;
    infoFieldDrawer.addStaticPrimitiveLast( infoFieldBound );

    TFontTTF ttfScoreTextPrinter( renderer, fontFile, fontSize );
    auto point = make_pair( activeGameField.xStart + ( cellsFieldParams.xCellsCount + 1 ) * cellsFieldParams.cellWidth + 20, gameFieldBound.yStart );
    ttfScoreTextPrinter.setPoint( point );
    TFontDrawer scoreTextDrawer( ttfScoreTextPrinter );

    TFontTTF ttfTitleTextPrinter( renderer, fontFile, fontSize + 6 );
    ttfTitleTextPrinter.setPoint( { wholeWidth / 2 - 50, 10 } );
    TFontDrawer titleTextDrawer( ttfTitleTextPrinter );
    titleTextDrawer.setText( "Tetris" );

    TDrawer mainDrawer( renderer );
    mainDrawer.addPrimitive( background );
    mainDrawer.addPrimitive( gameFieldBound );
    mainDrawer.addPrimitive( infoFieldBound );
    mainDrawer.addField( &tetrisDrawer );
    mainDrawer.addField( &infoFieldDrawer );
    mainDrawer.addText( &scoreTextDrawer );
    mainDrawer.addText( &titleTextDrawer );

    tetris.mainDrawer = &mainDrawer;
    tetris.scorePrinter = &scoreTextDrawer;

    thread mainThr2( &TTetrisGame::gameThread, &tetris );

    tetris.ioThread();

    mainThr2.join();

    TSdlWrapper::deteteInstance();

    return 0;
};
