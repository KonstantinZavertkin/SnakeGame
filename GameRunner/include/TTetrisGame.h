#ifndef _TTETRISGAME_H_
#define _TTETRISGAME_H_

#include <cstddef>
#include <utility>
#include <deque>
#include <mutex>

#include "TGameField.h"
#include "TCellsBlock.h"
#include "TFieldDrawer.h"

using namespace std;
using namespace game_backend;

class TTetrisGame
{
    public:
        TTetrisGame( pair<size_t, size_t> fieldSize );
        ~TTetrisGame();

        bool step();
        void turn( pair<int, int> rotateVector );

        void gameThread();
        void ioThread();

        TCellsBlock createFigure( size_t id );

        TGameField gameField;
        //TCellsBlock someBlock0;
        //TCellsBlock someBlock1;

        deque<TCellsBlock> allBlocks;

        TFieldDrawer* fDrawer;

        size_t clockCounter = 0;
        bool performStep = true;
        bool pauseGame = false;
        bool quit = false;

        deque<pair<int, int>> rotationsQueue;

        mutex syncPoint;
};

#endif
