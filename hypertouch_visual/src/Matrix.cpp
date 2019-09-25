#include "Matrix.hpp"

float** vecToMatrix(ofVec4f v){
    float** m = new float*[4];
    for (int h = 0; h < 4; h++)
    {
        m[h] = new float[1];
    }
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = v.w;
    return m;
}

ofVec3f matrixToVec(float** m) {
    float a = m[0][0];
    float b = m[1][0];
    float c = m[2][0];
    for (int i = 0; i < 3; i++)
    {
        delete [] m[i];
    }
    delete [] m;
    return ofVec3f(a, b, c);
}

ofVec4f matrixToVec4(float** m) {
    float a = m[0][0];
    float b = m[1][0];
    float c = m[2][0];
    float d = m[3][0];
    for (int i = 0; i < 4; i++)
    {
        delete [] m[i];
    }
    delete [] m;
    return ofVec4f(a,b,c,d);
}


void logMatrix(float** m, int c, int r) {
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}


ofVec3f matmul(float** a, int cA, int rA, ofVec4f b) {
    float** m = vecToMatrix(b);
    return matrixToVec(matmul(a, m, cA, rA, 1, 4));
}

ofVec4f matmul(float** a, int cA, int rA, ofVec4f b, bool fourth) {
    float** m = vecToMatrix(b);
    return matrixToVec4(matmul(a, m, cA, rA, 1, 4));
}

float** matmul(float** a, float** b, int cA, int rA, int cB, int rB) {
    int colsA = cA;
    int rowsA = rA;
    int colsB = cB;
    int rowsB = rB;
    
    if (colsA != rowsB) {
        cout << "Columns of A must match rows of B" << endl;
        return NULL;
    }
    float** result = new float*[rowsA];
    for (int i = 0; i < rowsA; i++)
    {
        result[i] = new float[colsB];
        for (int j = 0; j < colsB; j++) {
            float sum = 0;
            for (int k = 0; k < colsA; k++) {
                sum += a[i][k] * b[k][j];
            }
            result[i][j] = sum;
        }
        
    }
    for (int i = 0; i < 4; i++)
    {
        delete [] b[i];
    }
    delete [] b;
    
    return result;
}
