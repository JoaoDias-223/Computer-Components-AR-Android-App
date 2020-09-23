//
// Created by joao_vitor on 24/01/2020.
//

#include "Percurso.h"

void FitDimentions()
{

    Point P1, P2;

    P1.x = PATH_X[0]; 	     P1.y = PATH_Y[0];
    P2.x = PATH_X[LENPATH];  P2.y = PATH_Y[LENPATH];

    double DESLX = 0;
    double DESLY = (src_color_small.cols - src_color_small.rows) / 2.0;

    for (int i = 0; i <= LENPATH; i++)
    {
        PATH_X[i] = (PATH_X[i] + DESLX) * LAR / src_color_small.cols;
        PATH_Y[i] = (PATH_Y[i] + DESLY) * ALT / src_color_small.cols;
        ORTHO_LEN[i] = (ORTHO_LEN[i] * LAR) / src_color_small.cols;
    }


    itl_iats = ImAts.begin();

    Point Pi, Pf, Pf2;

    Pi.x = (int) PATH_X[0];
    Pi.y = (int) PATH_Y[0];

    Pf.x = (int) PATH_X[LENPATH];
    Pf.y = (int) PATH_Y[LENPATH];

    Point PD1, PD2, PE1, PE2;

    PD1 = itl_iats->margLINES[0].margP1;
    PD2 = itl_iats->margLINES[0].margP2;

    PE1 = itl_iats->margLINES[1].margP1;
    PE2 = itl_iats->margLINES[1].margP2;

    /*PD1.x = itl_iats->AE[0].x;
    PD1.y = itl_iats->AE[0].y;
    PD2.x = itl_iats->AE[1].x;
    PD2.y = itl_iats->AE[1].y;

    PE1.x = itl_iats->AE[2].x;
    PE1.y = itl_iats->AE[2].y;
    PE2.x = itl_iats->AE[3].x;
    PE2.y = itl_iats->AE[3].y;
    */

    int angle1 = (int) floor(getMainAngle0To1802(PD1, PD2) + 0.5);
    int  angle = (int) floor(getMainAngle0To1802(Pi, Pf)   + 0.5);
    //int angle2 = (int) floor(getMainAngle0To1802(PE1, PE2) + 0.5);

    //printf("angle1 = %d\n",angle1);


    double alpha_x = PROJECTIONS.at<float>(0, angle1);
    double alpha_y = PROJECTIONS.at<float>(1, angle);
    double alpha_z = 0.0; // itl_iats->MainAngle + 180;

    PROJ[0] = (float)  alpha_x;
    PROJ[1] = (float) -alpha_y;
    PROJ[2] = (float)  alpha_z;

}


void Orientation(std::vector <ImageAtributs> ImAts)
{

    /// COMPUTE THE MAIN ORIENTATION PATH

    double mdA, mrA;
    Point Pi, Pf;

    itl_iats = ImAts.begin();
    mdA = itl_iats->MAIN_ANGLE_DIR;

    Pi = itl_iats->MP;
    Pf = itl_iats->PF;

    // converto to radians
    if (mdA == 90.0) {
        mdA = 90.005;
    }

    mrA = mdA* CV_PI / 180.0;

    // y = tan * x + b
    double mtan = tan(mrA);
    double b = (double)Pi.y - mtan * (double)Pi.x;

    int x,y;
    //int k = 0;
    if (abs(Pi.x-Pf.x) > abs(Pi.y - Pf.y)) {// running on x
        if (Pi.x < Pf.x){
            for (x = Pi.x; x <= Pf.x; x++){
                y = mtan * x + b;
                PATH_X[LENPATH] = x;
                PATH_Y[LENPATH] = y;
                LENPATH++;
            }
        }
        else{
            for (x = Pi.x; x >= Pf.x; x--) {
                y = mtan * x + b;
                PATH_X[LENPATH] = (double) x;
                PATH_Y[LENPATH] = (double) y;
                LENPATH++;
            }
        }
    }
    else {// running on y
        if (Pi.y < Pf.y) {
            for (y = Pi.y; y <= Pf.y; y++) {
                x = (y-b) / mtan;
                PATH_X[LENPATH] = x;
                PATH_Y[LENPATH] = y;
                LENPATH++;
            }
        }
        else {
            for (y = Pi.y; y >= Pf.y; y--) {
                x = (y - b) / mtan;
                PATH_X[LENPATH] = x;
                PATH_Y[LENPATH] = y;
                LENPATH++;
            }
        }
    }
    LENPATH--;

    //COMPUTE THE ORTHOGONAL LENGTH AND ORIENTATION PATH
    double dx1, dy1, dx2, dy2, dx3, dy3;
    double b1, b2, b3;
    double tan1, tan2, tan3;
    Point P1, P2, P3, P4;

    P1 = itl_iats->margLINES[0].margP1;
    P2 = itl_iats->margLINES[0].margP2;

    P3 = itl_iats->margLINES[1].margP1;
    P4 = itl_iats->margLINES[1].margP2;


    /*P1.x = itl_iats->AE[0].x;
    P1.y = itl_iats->AE[0].y;

    P2.x = itl_iats->AE[1].x;
    P2.y = itl_iats->AE[1].y;

    P3.x = itl_iats->AE[2].x;
    P3.y = itl_iats->AE[2].y;

    P4.x = itl_iats->AE[3].x;
    P4.y = itl_iats->AE[3].y;
    */

    dx1 = (double)(P1.x - P2.x);  if (dx1 == 0) dx1 = dx1 + 0.005;
    dx2 = (double)(P4.x - P3.x);  if (dx2 == 0) dx2 = dx2 + 0.005;
    dx3 = (double)(Pf.x - Pi.x);  if (dx3 == 0) dx3 = dx3 + 0.005;

    dy1 = (double)(P1.y - P2.y);
    dy2 = (double)(P4.y - P3.y);
    dy3 = (double)(Pf.y - Pi.y);

    tan1 = dy1 / dx1;  if (tan1 == 0) tan1 = tan1 + 0.005;
    tan2 = dy2 / dx2;  if (tan2 == 0) tan2 = tan2 + 0.005;
    tan3 = dy3 / dx3;  if (tan3 == 0) tan3 = tan3 + 0.005;

    b1 = (double)P1.y - tan1 * P1.x;
    b2 = (double)P3.y - tan2 * P3.x;
    b3 = (double)Pi.y - tan3 * Pi.x;

    double XA, YA, XB, YB, tan3_, b3_;
    tan3_ = -1.0 / tan3;

    Point PA, PB, PC;

    for (int i = 0; i <= LENPATH; i++)
    {
        b3_ = PATH_Y[i] - tan3_ * PATH_X[i];

        XA = (b3_ - b1) / (tan1 - tan3_);
        YA = tan3_ * XA + b3_;

        XB = (b3_ - b2) / (tan2 - tan3_);
        YB = tan3_ * XB + b3_;

        PA.x = (int)XA;
        PA.y = (int)YA;
        PB.x = (int)XB;
        PB.y = (int)YB;
        PC.x = (int)PATH_X[i];
        PC.y = (int)PATH_Y[i];

        ORTHO_LEN[i] = SIZE_OBJ * ((float)(1 + OBJ_VAR[(int)mdA]));
    }
}


void PathDatails(int Xini, int Yini, int Xfin, int Yfin, double tan)
{
    double b;

    b = Yini - tan * Xini;

    int DX = (int)fabs((double)(Xini - Xfin));
    int DY = (int)fabs((double)(Yini - Yfin));

    if (DX > DY)
    {
        if (Xini < Xfin)
        {
            double X, Y;
            LENPATH = -1;
            for (int x = Xini; x <= Xfin; x++)
            {
                X = (double)x;
                Y = tan * X + b;
                LENPATH++;
                PATH_X[LENPATH] = X;
                PATH_Y[LENPATH] = Y;
            }
        }
        else
        {
            double X, Y;
            LENPATH = -1;
            for (int x = Xfin; x <= Xini; x++)
            {
                X = (double) x;
                Y = tan * X + b;
                LENPATH++;
                PATH_X[LENPATH] = X;
                PATH_Y[LENPATH] = Y;
            }
        }
    }
    else
    {
        if (Yini < Yfin)
        {
            double X, Y;
            LENPATH = -1;
            for (int y = Yini; y <= Yfin; y++)
            {
                Y = (double) y;
                X = (Y - b) / tan;
                LENPATH++;
                PATH_X[LENPATH] = X;
                PATH_Y[LENPATH] = Y;
            }
        }
        else
        {
            double X, Y;
            LENPATH = -1;
            for (int y = Yfin; y <= Yini; y++)
            {
                Y = (double) y;
                X = (Y - b) / tan;
                LENPATH++;
                PATH_X[LENPATH] = X;
                PATH_Y[LENPATH] = Y;
            }
        }
    }
}


double getMainAngle0To1802(Point P1, Point P2)
{

    Point V1, V2;
    V2.x = 400;
    V2.y = 0;

    if (P1.y < P2.y) { //P1 sera a base
        V1.x = P2.x - P1.x;
        V1.y = P2.y - P1.y;
    }
    else { //P2 sera a base
        V1.x = P1.x - P2.x;
        V1.y = P1.y - P2.y;
    }

    double num = V1.x * V2.x + V1.y * V2.y;
    double den = sqrt(V1.x * V1.x + V1.y * V1.y) * sqrt(V2.x * V2.x + V2.y * V2.y);

    double coss_ang = acos(num / den) * (180.0 / CV_PI);

    if (coss_ang == 180.0) {
        coss_ang = 0;
    }

    return coss_ang;
}


void FitPath(std::vector <ImageAtributs>& ImAts)
{

    FitMainPath(ImAts);

    FitRightPath(ImAts);

    FitLeftPath(ImAts);

}


void FitMainPath(std::vector <ImageAtributs>& ImAts)
{

    itl_iats = ImAts.begin();

    int Lin = itl_iats->RegionMask.rows;
    int Col = itl_iats->RegionMask.cols;

    double Xi, Yi, Xc, Yc, Xf, Yf;
    Xi = 0; Yi = 0; Xc = 0; Yc = 0; Xf = 0; Yf = 0;

    Xi = (double)itl_iats->MP.x;
    Yi = (double)itl_iats->MP.y;

    Xc = (double)itl_iats->CP.x;
    Yc = (double)itl_iats->CP.y;

    // local para se decidir se a orientacao é em direcao ao CP ou média da pista
    double mdA = itl_iats->MAIN_ANGLE_DIR;

    if ((mdA == 180) || ((mdA == 90))) {
        mdA = mdA - 0.05;
    }

    // converte para radianos
    double mrA = mdA * CV_PI / 180;

    double TAN = tan(mrA);

    // y = tan x + b
    // x = (y - b) / tan;
    double B = Yi - TAN * Xi;
    double XT;
    //double YT;

    if (Yc > Yi) { // corta Ymax
        Yf = Lin;
    }
    else {// corta Y = 0
        Yf = 0;
    }

    XT = (Yf - B) / TAN;

    if ((XT >= 0) && (XT < Col)) {
        Xf = XT;
    }
    else {
        if (XT >= Col) {
            XT = Col - 1;
        }
        else {
            XT = 0;
        }
        Yf = TAN * XT + B;
    }

    itl_iats->PF.x = (int)Xf;
    itl_iats->PF.y = (int)Yf;

}


void FitRightPath(std::vector <ImageAtributs>& ImAts)
{

    itl_iats = ImAts.begin();

    int Lin = itl_iats->RegionMask.rows;
    int Col = itl_iats->RegionMask.cols;

    double X1, Y1, X2, Y2, Xf, Yf;

    X1 = (double)itl_iats->margLINES[0].margP1.x;
    Y1 = (double)itl_iats->margLINES[0].margP1.y;

    X2 = (double)itl_iats->margLINES[0].margP2.x;
    Y2 = (double)itl_iats->margLINES[0].margP2.y;

    /*X1 = (double)itl_iats->AE[0].x;
    Y1 = (double)itl_iats->AE[0].y;

    X2 = (double)itl_iats->AE[1].x;
    Y2 = (double)itl_iats->AE[1].y;
    */

    //printf("X1 = %.3f Y1 = %.3f X2 = %.3f Y2 = %.3f\n",X1,Y1,X2,Y2);

    // local para se decidir se a orientacao é em direcao ao CP ou média da pista
    double dx1 = X2 - X1;
    double dy1 = Y2 - Y1;
    if (dx1 == 0) dx1 = dx1 + 0.05;
    double TAN = dy1 / dx1;


    // y = tan x + b
    // x = (y - b) / tan;
    double B = Y1 - TAN * X1;

    double XA, YA;
    //double XB, YB;

    vector<Point> P12;
    Point Pt;

    // test intersection Y = 0
    YA = 0;
    XA = (YA - B) / TAN;
    if ((XA >= 0) && (XA < Col)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("1 X = %d Y = %d\n", Pt.x, Pt.y);
    }

    // test intersection X = 0
    XA = 0;
    YA = TAN * XA + B;
    if ((YA >= 0) && (YA < Lin)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("2 X = %d Y = %d\n",Pt.x,Pt.y);
    }

    // test intersection Y = Lin - 1
    YA = Lin - 1;
    XA = (YA - B) / TAN;
    if ((XA >= 0) && (XA < Col)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("3 X = %d Y = %d\n", Pt.x, Pt.y);
    }

    // test intersection X = Col - 1
    XA = Col - 1;
    YA = TAN * XA + B;
    if ((YA >= 0) && (YA < Lin)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("4 X = %d Y = %d\n", Pt.x, Pt.y);
    }

    //printf("P12[0].x = %.3f P12[0].y = %.3f P12[1].x = %.3f P12[1].y = %.3f\n", P12[0].x, P12[0].y, P12[1].x, P12[1].y);

    if (P12[0].y > Y1)
    {
        Xf = P12[0].x;
        Yf = P12[0].y;
        //printf("1\n");
    }
    else
    {
        Xf = P12[1].x;
        Yf = P12[1].y;
        //printf("2\n");
    }

    //printf("1 X1 = %.3f Y1 = %.3f\n",X1,Y1);
    //printf("1 Xf = %.3f Yf = %.3f\n",Xf,Yf);
    //system("pause");

    itl_iats->PR.x = (int)Xf;
    itl_iats->PR.y = (int)Yf;

}


void FitLeftPath(std::vector <ImageAtributs>& ImAts)
{

    itl_iats = ImAts.begin();

    int Lin = itl_iats->RegionMask.rows;
    int Col = itl_iats->RegionMask.cols;

    double X1, Y1, X2, Y2, Xf, Yf;

    X1 = (double)itl_iats->margLINES[1].margP1.x;
    Y1 = (double)itl_iats->margLINES[1].margP1.y;

    X2 = (double)itl_iats->margLINES[1].margP2.x;
    Y2 = (double)itl_iats->margLINES[1].margP2.y;


    /*X1 = (double)itl_iats->AE[3].x;
    Y1 = (double)itl_iats->AE[3].y;

    X2 = (double)itl_iats->AE[2].x;
    Y2 = (double)itl_iats->AE[2].y;
    */

    //printf("X1 = %.3f Y1 = %.3f X2 = %.3f Y2 = %.3f\n",X1,Y1,X2,Y2);

    // local para se decidir se a orientacao é em direcao ao CP ou média da pista
    double dx1 = X2 - X1;
    double dy1 = Y2 - Y1;
    if (dx1 == 0) dx1 = dx1 + 0.05;
    double TAN = dy1 / dx1;


    // y = tan x + b
    // x = (y - b) / tan;
    double B = Y1 - TAN * X1;

    double XA, YA;
    //double XB, YB;
    vector<Point> P12;
    Point Pt;

    // test intersection Y = 0
    YA = 0;
    XA = (YA - B) / TAN;
    if ((XA >= 0) && (XA < Col)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("1 X = %d Y = %d\n", Pt.x, Pt.y);
    }

    // test intersection X = 0
    XA = 0;
    YA = TAN * XA + B;
    if ((YA >= 0) && (YA < Lin)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("2 X = %d Y = %d\n",Pt.x,Pt.y);
    }

    // test intersection Y = Lin - 1
    YA = Lin - 1;
    XA = (YA - B) / TAN;
    if ((XA >= 0) && (XA < Col)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("3 X = %d Y = %d\n", Pt.x, Pt.y);
    }

    // test intersection X = Col - 1
    XA = Col - 1;
    YA = TAN * XA + B;
    if ((YA >= 0) && (YA < Lin)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("4 X = %d Y = %d\n", Pt.x, Pt.y);
    }

    //printf("P12[0].x = %.3f P12[0].y = %.3f P12[1].x = %.3f P12[1].y = %.3f\n", P12[0].x, P12[0].y, P12[1].x, P12[1].y);

    if (P12[0].y > Y1)
    {
        Xf = P12[0].x;
        Yf = P12[0].y;
        //printf("1\n");
    }
    else
    {
        Xf = P12[1].x;
        Yf = P12[1].y;
        //printf("2\n");
    }

    //printf("2 X1 = %.3f Y1 = %.3f\n", X1, Y1);
    //printf("2 Xf = %.3f Yf = %.3f\n", Xf, Yf);
    //system("pause");

    itl_iats->PL.x = (int)Xf;
    itl_iats->PL.y = (int)Yf;
}


void LineExtremePoints(Point P1, Point P2, Point* Pi, Point* Pf, int rows, int cols)
{

    double X1 = (double)P1.x;
    double Y1 = (double)P1.y;

    double X2 = (double)P2.x;
    double Y2 = (double)P2.y;

    double dx1 = X2 - X1;
    double dy1 = Y2 - Y1;
    if (dx1 == 0) dx1 = dx1 + 0.05;
    double TAN = dy1 / dx1;

    // y = tan x + b
    // x = (y - b) / tan;
    double B = Y1 - TAN * X1;
    double XA, YA;
    //double XB, YB;
    vector<Point> P12;
    Point Pt;


    //printf("\n\nFRAME = %d\n",fm);

    //printf("TAN = %.3f B = %.3f  X1 = %.3f Y1 = %.3f\n",TAN,B,X1,Y1);

    // test intersection Y = 0
    YA = 0;
    XA = (YA - B) / TAN;

    //printf("XA = %.3f YA = %.3f\n", XA, YA);

    if ((XA >= 0) && (XA < cols)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("1\n");
    }

    // test intersection X = 0
    XA = 0;
    YA = TAN * XA + B;

    //printf("XA = %.3f YA = %.3f\n",XA,YA);

    if ((YA >= 0) && (YA < rows)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("2\n");
    }

    // test intersection Y = Lin - 1
    YA = rows - 1;
    XA = (YA - B) / TAN;

    //printf("XA = %.3f YA = %.3f\n", XA, YA);

    if ((XA >= 0) && (XA < cols)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("3\n");
    }

    // test intersection X = Col - 1
    XA = cols - 1;
    YA = TAN * XA + B;

    //printf("XA = %.3f YA = %.3f\n", XA, YA);

    if ((YA >= 0) && (YA < rows)) {
        Pt.x = (int)XA;
        Pt.y = (int)YA;
        P12.push_back(Pt);
        //printf("4\n");
    }


    if (P12.size() >= 0) {

        //printf("P12.size = %d\n",P12.size());

        if (P12[0].x < P12[1].x) {

            (*Pi).x = P12[0].x;
            (*Pi).y = P12[0].y;
            (*Pf).x = P12[1].x;
            (*Pf).y = P12[1].y;
        }
        else {
            (*Pi).x = P12[1].x;
            (*Pi).y = P12[1].y;
            (*Pf).x = P12[0].x;
            (*Pf).y = P12[0].y;
        }
    }


}