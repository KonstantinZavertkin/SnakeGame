#ifndef _TFIELD_H_
#define _TFIELD_H_

#include <vector>
#include <deque>
#include "TCell.h"

namespace game_backend
{
    using namespace std;

    class TGameField
    {
        public:
            TGameField( size_t linesCount, size_t rowsCount );
            ~TGameField();

            void debugPrint();

            void scrollField( pair<int, int> );

            vector<vector<TCell>> field;     //! 2d field representation
            pair<size_t, size_t> fieldSize;
    };
};

#endif
