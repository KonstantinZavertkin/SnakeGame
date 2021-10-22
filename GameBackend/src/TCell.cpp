
#include "TCell.h"

namespace game_backend 
{
    const std::string TCellStates::eatStateKey = "eat";
    const std::string TCellStates::backgroundStateKey = "background";
    const std::string TCellStates::snakeHeadStateKey = "snakeHead";
    const std::string TCellStates::snakeBodyStateKey = "snakeBody";
    const std::string TCellStates::snakeTailStateKey = "snakeTail";
    const std::string TCellStates::newSectionStateKey = "snakeNewSection";

    const std::string TCellStates::redColorStateKey = "redColor";
    const std::string TCellStates::greenColorStateKey = "greenColor";
    const std::string TCellStates::blueColorStateKey = "blueColor";

    TCell::TCell() : currentState( TCellStates::backgroundStateKey ){};

    TCell::~TCell(){};
};
