#pragma once
#include "Tetris.h"

class CTetris : public Tetris {
    protected:
        int* arrayCScreen;
        Matrix *currColorBlk;
        Matrix *tempColorBlk;

    public:
        Matrix *iCScreen;
        Matrix *oCScreen;

        CTetris(int dy, int dx);
        ~CTetris(){}
        static void init(int *setOfCBlockArrays[], int blk_type, int blk_degree);
        TetrisState accept(char key);
        int* createArrayScreen();
        void deleteFullLines();
};

static vector<vector<Matrix*>> setOfColorBlockObjects;