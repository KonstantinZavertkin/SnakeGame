#ifndef _TSNAKEGAME_H_
#define _TSNAKEGAME_H_

#include <deque>
#include <mutex>

#include "TGameField.h"
#include "TCellsChain.h"
#include "TDrawer.h"
#include "TFieldDrawer.h"
#include "TMainMenu.h"
#include "TScoreStorage.h"

using namespace std;

//! Класс игры "змейка". Создает поле TGameField и связанный с ним объект TCellsChain
//! Выполняет дискретный шаг игры
class TSnakeGame
{
    public:
        TSnakeGame( TCoords fieldSize, size_t snakeLength );
        ~TSnakeGame();

        bool step();
        void turn( TCoords rotateVector );
        void checkFood();

        void gameThread();
        void ioThread();

        TGameField gameField;
        TCellsChain snake;

        TDrawer* mainDrawer = nullptr;
        TFontDrawer* scorePrinter = nullptr;
        TFontDrawer* bestScorePrinter = nullptr;
        TMainMenu* pauseMenu = nullptr;

        size_t initSnakeLength = 0;
        size_t clockCounter = 0;
        bool performStep = true;
        bool pauseGame = false;
        bool quit = false;

        deque<TCoords> rotationsQueue;

    private:
        mutex syncPoint;
        const string pathToBestScoreFile = "snake_best_score.txt";
        TScoreStorage bestScoreStorage;
        unsigned bestScore = 0;
        unsigned currentScore = 0;
};

#endif
