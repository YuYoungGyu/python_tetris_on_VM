#include "CTetris.h"
#include <cmath>

CTetris::CTetris(int dy, int dx) : Tetris(dy, dx){
    iScreenDy = dy;
    iScreenDx = dx;
    idxBlockDegree = 0;

    int arrayBlk_maxSize = 0;
    for(int i = 0; i < nBlockTypes; i++){
        int n = setOfBlockObjects[i][0]->get_dx();
        if(arrayBlk_maxSize < n) arrayBlk_maxSize = n;
    }
    iScreenDw = arrayBlk_maxSize;


    arrayCScreen = createArrayScreen();
    Matrix *icsmat = new Matrix(arrayCScreen,iScreenDy+iScreenDw,iScreenDx+iScreenDw*2);
    iCScreen = icsmat;
    Matrix *ocsmat = new Matrix(iCScreen);
    oCScreen = ocsmat;

    arrayScreen = createArrayScreen();
    Matrix *ismat = new Matrix(arrayScreen,iScreenDy+iScreenDw,iScreenDx+iScreenDw*2);
    iScreen = ismat;
    Matrix *osmat = new Matrix(iScreen);
    oScreen = osmat;

    justStarted = true;
}

CTetris::~CTetris(){
    cout << "delete setOfColorBlockObjects" << endl;
    while(!setOfColorBlockObjects.empty()){
        vector<Matrix*> tmp_layer = setOfColorBlockObjects.back();
        setOfColorBlockObjects.pop_back();
        while(!tmp_layer.empty()){
            Matrix* tmp = tmp_layer.back();
            tmp_layer.pop_back();
            delete tmp;
        }
    }

    cout << "delete arrayCScreen" << endl;
    delete arrayCScreen;
    cout << "delete iCScreen" << endl;
    delete iCScreen;
    cout << "delete oCScreen" << endl;
    delete oCScreen;
    cout << "delete currColorBlk" << endl;
    delete currColorBlk;
    cout << "delete tempColorBlk" << endl;
    delete tempColorBlk;
}

void CTetris::init(int *setOfCBlockArrays[], int blk_type, int blk_degree){
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

    for(int j =0; j < nBlockTypes; j++){
        vector<Matrix*> setOfOneColorBlock;
        for(int i =0; i < nBlockDegrees; i++){
            Matrix *mat = new Matrix(setOfBlockObjects[j][i]);
            mat->mulc(j+2);
            setOfOneColorBlock.push_back(mat);
        }
        setOfColorBlockObjects.push_back(setOfOneColorBlock);
    }

}

TetrisState CTetris::accept(char key){
    state = Running;
    if (key >= '0' and key <= '6'){
        if (justStarted == false)
            deleteFullLines();
        
        idxBlockType = key - '0';
        idxBlockDegree = 0;

        Matrix *ismat = new Matrix(oScreen);
        iScreen = ismat;

        currBlk = setOfBlockObjects[idxBlockType][idxBlockDegree];
        
        Matrix *icsmat = new Matrix(oCScreen);
        iCScreen = icsmat;

        currColorBlk = setOfColorBlockObjects[idxBlockType][idxBlockDegree];

        top = 0;
        left = iScreenDw + iScreenDx/2 - currBlk->get_dx()/2;

        tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
        tempBlk = tempBlk->add(currBlk);

        tempColorBlk = iCScreen->clip(top, left, top+currColorBlk->get_dy(), left+currColorBlk->get_dx());
        tempColorBlk = tempColorBlk->add(currColorBlk);

        justStarted = false;
        cout << endl;

        if (tempBlk->anyGreaterThan(1))
            state = Finished;

        Matrix *osmat = new Matrix(iScreen);
        oScreen = osmat;
        oScreen->paste(tempBlk, top, left);
        
        Matrix *ocsmat = new Matrix(iCScreen);
        oCScreen = ocsmat;
        oCScreen->paste(tempColorBlk, top, left);

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
        currColorBlk = setOfColorBlockObjects[idxBlockType][idxBlockDegree];
    }
    else if (key == ' '){ // drop the block
        while (!tempBlk->anyGreaterThan(1)){
            top += 1;
            tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
            tempBlk = tempBlk->add(currBlk);
            tempColorBlk = iCScreen->clip(top, left, top+currColorBlk->get_dy(), left+currColorBlk->get_dx());
            tempColorBlk = tempColorBlk->add(currColorBlk);
        }
    }
    else
        cout << "Wrong key!!!\n";

    //delete tempBlk;
    tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
    tempBlk = tempBlk->add(currBlk);    

    //delete tempColorBlk;
    tempColorBlk = iCScreen->clip(top, left, top+currColorBlk->get_dy(), left+currColorBlk->get_dx());
    tempColorBlk = tempColorBlk->add(currColorBlk);

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
            //delete currBlk;
            currBlk = setOfBlockObjects[idxBlockType][idxBlockDegree];
            //delete currColorBlk;
            currColorBlk = setOfColorBlockObjects[idxBlockType][idxBlockDegree];
        }
        else if (key == ' '){ // undo: move up
            top -= 1;
            state = NewBlock;
        }

        tempBlk = iScreen->clip(top, left, top+currBlk->get_dy(), left+currBlk->get_dx());
        tempBlk = tempBlk->add(currBlk);

        tempColorBlk = iCScreen->clip(top, left, top+currColorBlk->get_dy(), left+currColorBlk->get_dx());
        tempColorBlk = tempColorBlk->add(currColorBlk);
    }

    Matrix *osmat = new Matrix(iScreen);
    oScreen = osmat;
    oScreen->paste(tempBlk, top, left);
    
    Matrix *ocsmat = new Matrix(iCScreen);
    oCScreen = ocsmat;
    oCScreen->paste(tempColorBlk, top, left);

    return state;
}

int* CTetris::createArrayScreen(){
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

void CTetris::deleteFullLines(){
    int **array = oScreen->get_array();
    int **colorArray = oCScreen->get_array();
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
                colorArray[row] = colorArray[row-1];
            }
            for (int x=0; x< iScreenDw; x++){
                array[0][x] = 1;
                colorArray[0][x] = 1;
            }
            for (int x=0; x<iScreenDx; x++){
                array[0][iScreenDw + x] = 0;
                colorArray[0][iScreenDw + x] = 0;
            }
            for (int x=0; x< iScreenDw; x++){
                array[0][iScreenDw + iScreenDx + x] = 1;
                colorArray[0][iScreenDw + iScreenDx + x] = 1;
            }
        }
    }

    iScreen = new Matrix(*array, iScreenDy, iScreenDx);
    iScreen->paste(tempBlk, top, left);

    iCScreen = new Matrix(*colorArray, iScreenDy, iScreenDx);
    iCScreen->paste(tempColorBlk, top, left);
}