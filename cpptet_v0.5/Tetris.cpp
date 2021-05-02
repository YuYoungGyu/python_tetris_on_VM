#include "Tetris.h"
#include <cmath>

Tetris::Tetris(int dy, int dx){
    iScreenDy = dy;
    iScreenDx = dx;
    idxBlockDegree = 0;
    
    int arrayBlk_maxSize = 0;
    for(int i = 0; i < nBlockTypes; i++){
        int n = setOfBlockObjects[i][0]->get_dx();
        if(arrayBlk_maxSize < n) arrayBlk_maxSize = n;
    }
    iScreenDw = arrayBlk_maxSize;

    arrayScreen = createArrayScreen();
    Matrix *ismat = new Matrix(arrayScreen,iScreenDy+iScreenDw,iScreenDx+iScreenDw*2);
    iScreen = ismat;
    Matrix *osmat = new Matrix(iScreen);
    oScreen = osmat;
    justStarted = true;
}

Tetris::~Tetris(){
    cout << "delete setOfBlockObjects" << endl;
    while(!setOfBlockObjects.empty()){
        vector<Matrix*> tmp_layer = setOfBlockObjects.back();
        setOfBlockObjects.pop_back();
        while(!tmp_layer.empty()){
            Matrix* tmp = tmp_layer.back();
            tmp_layer.pop_back();
            delete tmp;
        }
    }

    cout << "delete arrayScreen" << endl;
    delete arrayScreen;
    cout << "delete iScreen" << endl;
    delete iScreen;
    cout << "delete oScreen" << endl;
    delete oScreen;
    cout << "delete currBlk" << endl;
    delete currBlk;
    cout << "delete tempBlk" << endl;
    delete tempBlk;
}

void Tetris::init(int *setOfCBlockArrays[], int blk_type, int blk_degree){
    nBlockTypes = blk_type;
    nBlockDegrees = blk_degree;

    for (int i = 0; i < blk_type; i++){
        vector<Matrix*> setOfOneBlock;

        int m = 0;
        while(1){
            if (*(setOfCBlockArrays[i*blk_degree]+m) == -1) break;
            m++;
        }
        int n = sqrt(m);

        for(int j = 0; j < blk_degree; j++) {
            setOfOneBlock.push_back(new Matrix(setOfCBlockArrays[i*blk_degree+j],n,n));
        }
        setOfBlockObjects.push_back(setOfOneBlock);
    }
}

int* Tetris::createArrayScreen(){
    int arrayScreenDx = iScreenDw * 2 + iScreenDx;
    int arrayScreenDy = iScreenDy + iScreenDw;
    int* arrayScreen = (int*)malloc(sizeof(int)*(arrayScreenDy*arrayScreenDx));

    for(int y = 0; y < arrayScreenDy; y++){
        for (int x = 0; x < iScreenDw; x++)
            arrayScreen[y*arrayScreenDx + x] = 1;
        for (int x = 0; x < iScreenDx; x++)
            arrayScreen[y*arrayScreenDx+iScreenDw + x] = 0;
        for (int x = 0; x < iScreenDw; x++)
            arrayScreen[y*arrayScreenDx+iScreenDw + iScreenDx + x] = 1;
    }

    for (int y = 0; y < iScreenDw; y++){
        for (int x = 0; x < arrayScreenDx; x++)
            arrayScreen[(iScreenDy + y)*arrayScreenDx + x] = 1;
    }
    return arrayScreen;
}

TetrisState Tetris::accept(char key){
    /*새로운 동적할당을 하기 전 수시로 delete를 해주도록 하였음*/
    state = Running;

    if (key >= '0' and key <= '6'){
        if (justStarted == false)
            deleteFullLines();

        idxBlockType = key - '0';
        idxBlockDegree = 0;

        Matrix *ismat = new Matrix(oScreen);
        iScreen = ismat;

        currBlk = setOfBlockObjects[idxBlockType][idxBlockDegree];

        top = 0;
        left = iScreenDw + iScreenDx/2 - currBlk->get_dx()/2;

        tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
        tempBlk = tempBlk->add(currBlk);

        justStarted = false;
        cout << endl;

        if (tempBlk->anyGreaterThan(1))
            state = Finished;

        Matrix *osmat = new Matrix(iScreen);
        oScreen = osmat;
        oScreen->paste(tempBlk, top, left);

        return state;
    }
    else if (key == 'q');

    else if (key == 'a') // move left
        left -= 1;
    else if (key == 'd') // move right
        left += 1;
    else if (key == 's') // move down
        top += 1;
    else if (key == 'w'){ // rotate the block clockwise
        idxBlockDegree = (idxBlockDegree + 1) % nBlockDegrees;
        currBlk = setOfBlockObjects[idxBlockType][idxBlockDegree];
    }
    else if (key == ' '){ // drop the block
        while (!tempBlk->anyGreaterThan(1)){
            top += 1;
            tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
            tempBlk = tempBlk->add(currBlk);
        }
    }
    else
        cout << "Wrong key!!!\n";
    
    tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
    tempBlk = tempBlk->add(currBlk);

    if (tempBlk->anyGreaterThan(1)){   // 벽 충돌시 undo 수행
        if (key == 'a') // undo: move right
            left += 1;
        else if (key == 'd') // undo: move left
            left -= 1;
        else if (key == 's'){ // undo: move up
            top -= 1;
            state = NewBlock;
        }
        else if (key == 'w'){ // undo: rotate the block counter-clockwise
            idxBlockDegree = (idxBlockDegree - 1) % nBlockDegrees;
            currBlk = setOfBlockObjects[idxBlockType][idxBlockDegree];
        }
        else if (key == ' '){ // undo: move up
            top -= 1;
            state = NewBlock;
        }
        
        tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
        tempBlk = tempBlk->add(currBlk);
    }

    Matrix *osmat = new Matrix(iScreen);
    oScreen = osmat;
    oScreen->paste(tempBlk, top, left);

    return state;
}

void Tetris::deleteFullLines(){
    int **array = oScreen->get_array();
    for(int y = 0; y<iScreenDy; y++){
        bool fullLine = true;
        for (int x =0; x < iScreenDx; x++){
            if (array[y][iScreenDw + x] != 1){
                fullLine = false;
                break;
            }
        }
        if (fullLine == true){
            for (int row = y; row > 0; row--){
                array[row] = array[row-1];
            }
            for (int x=0; x< iScreenDw; x++){
                array[0][x] = 1;
            }
            for (int x=0; x<iScreenDx; x++){
                array[0][iScreenDw + x] = 0;
            }
            for (int x=0; x< iScreenDw; x++){
                array[0][iScreenDw + iScreenDx + x] = 1;
            }
        }
    }

    iScreen = new Matrix(*array, iScreenDy, iScreenDx);
    iScreen->paste(tempBlk, top, left);
}