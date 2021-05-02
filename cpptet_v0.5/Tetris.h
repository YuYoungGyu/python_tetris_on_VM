#pragma once
#include "Matrix.h"
#include <vector>

enum TetrisState { Running, NewBlock, Finished};

class Tetris {
    protected:   
        int iScreenDy;
        int iScreenDx;
        int idxBlockType;
        int idxBlockDegree;
        int* arrayScreen;
        bool justStarted;
        TetrisState state;

        Matrix *currBlk;
        Matrix *tempBlk;

        int top;
        int left;
    public:
        Matrix *iScreen;
        Matrix *oScreen;
        int iScreenDw;

        Tetris(int dy, int dx);
        ~Tetris();

        static void init(int *setOfCBlockArrays[], int blk_type, int blk_degree);
        int* createArrayScreen();
        TetrisState accept(char key);
        void deleteFullLines();
};

static int nBlockTypes = 0;
static int nBlockDegrees = 0;
static vector<vector<Matrix*>> setOfBlockObjects;