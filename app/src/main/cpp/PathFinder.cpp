#include "PathFinder.h"
#include <typeinfo>
#include <android/log.h>

#define  LOG_TAG    "PathFinder"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


//A
double ABSDist(Point2f P1, Point2f P2, Point2f P3, Point2f P4)
{

    double d1, d2, d12;
    double d3, d4, d34;
    double DIST;

    d1 = sqrt((P1.x - P3.x) * (P1.x - P3.x) + (P1.y - P3.y) * (P1.y - P3.y));
    d2 = sqrt((P2.x - P4.x) * (P2.x - P4.x) + (P2.y - P4.y) * (P2.y - P4.y));
    d12 = ((d1 + d2) / 2.0);

    d3 = sqrt((P1.x - P4.x) * (P1.x - P4.x) + (P1.y - P4.y) * (P1.y - P4.y));
    d4 = sqrt((P2.x - P3.x) * (P2.x - P3.x) + (P2.y - P3.y) * (P2.y - P3.y));
    d34 = ((d3 + d4) / 2.0);

    if (d12 < d34) DIST = d12;
    else           DIST = d34;

    return DIST;
}

//B

//C
void compareFramesForNewOrientation(Mat old_very_small_frame, Mat new_very_small_frame){

    vector<Point2f> ERL_temp1;
    vector<Point2f> ERL_temp2;

    vector<Point2f>::iterator it_erl;
    vector<Point2f>::iterator it_ell;

    int DES = LOW_RES_AROUND_LINE;

    double DIF;
    double DIFmin = 100000.0;
    int L1, L2, x, y, ndx, ndy;

    for (int dy = -DES; dy <= DES; dy++)
    {
        for (int dx = -DES; dx <= DES; dx++)
        {
            DIF = 0;
            for (it_erl = ERL_old.begin(); it_erl != ERL_old.end(); ++it_erl)
            {
                x = (int)it_erl->x + dx;
                y = (int)it_erl->y + dy;

                if ((x >= 0) && (x < old_very_small_frame.cols) && (y >= 0) && (y < old_very_small_frame.rows)) {

                    L1 = old_very_small_frame.at<uchar>(y, x);
                    L2 = new_very_small_frame.at<uchar>(y, x);

                    ERL_temp1.push_back(Point2f(x, y));

                    DIF = DIF + fabs(L1 - L2);
                }
            }

            if (DIF < DIFmin)
            {
                DIFmin = DIF;
                ndx = dx;
                ndy = dy;
                ERL_temp2.assign(ERL_temp1.begin(), ERL_temp1.end());
                //printf("DIF = %.3f ndx = %d ndy = %d\n",DIF,ndx,ndy);
            }
            ERL_temp1.clear();
        }
    }

    itl_iats = ImAts.begin();
}
void CannySegmentation(Mat src_gray0, Mat* dst_bw){
    //LOGI("called CannySegmentation()");

    Mat canny;
    Canny(src_gray0, canny, 50, 200, 3);
    dilate(canny, canny, Mat(), Point(-1, -1), 2, 1, 1);
    adaptiveThreshold(canny, canny, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 11, -1);
    canny.copyTo((*dst_bw));
}
void computeDirectionLINE() {
    //LOGI("called computeDirectionLine()");

    itl_iats = ImAts.begin();

    Point2f PL1, PL2, PR1, PR2;

    int Lins = src_gray_small.rows;
    int Cols = src_gray_small.cols;

    // extreme lines
    PL1 = itl_iats->PL1;
    PL2 = itl_iats->PL2;

    PR1 = itl_iats->PR1;
    PR2 = itl_iats->PR2;

    double dxL, dyL, dxR, dyR;
    double tanL, tanR, bL, bR;
    double xD, yD;
    //double angD;

    Point MPE;

    dxL = PL2.x - PL1.x;  if (dxL == 0) dxL = 0.01;
    dxR = PR2.x - PR1.x;  if (dxR == 0) dxR = 0.01;

    dyL = PL2.y - PL1.y;
    dyR = PR2.y - PR1.y;

    tanL = dyL / dxL;
    bL = PL1.y - tanL * PL1.x;

    tanR = dyR / dxR;
    bR = PR1.y - tanR * PR1.x;

    double distP1, distP2;
    if (tanL != tanR) {

        xD = (bR - bL) / (tanL - tanR);
        yD = tanL * xD + bL;
        angD = ((atan(tanL) * 180.0 / CV_PI) + (atan(tanR) * 180 / CV_PI)) / 2.0;
        tanD = angD * (CV_PI / 180.0);
        bD = yD - tanD * xD;

        MPE.x = (int) ((bR - bL) / (tanL - tanR));
        MPE.y = (int) (tanD * xD + bD);

        Point PT;
        vector<Point2f> P12;

        PT.x = 0;
        PT.y = (int) (tanD * PT.x + bD);

        if ((PT.y >= 0) && (PT.y < Lins)) {
            P12.push_back(PT);
        }

        PT.x = Cols - 1;
        PT.y = (int) (tanD * PT.x + bD);

        if ((PT.y >= 0) && (PT.y < Lins)) {
            P12.push_back(PT);
        }

        PT.y = 0;
        PT.x = (int) ((PT.y - bD) / tanD);

        if ((PT.x >= 0) && (PT.x < Cols)) {
            P12.push_back(PT);
        }

        PT.y = Lins - 1;
        PT.x = (int) ((PT.y - bD) / tanD);

        if ((PT.x >= 0) && (PT.x < Cols)) {
            P12.push_back(PT);
        }

        if(P12.size() >= 2) {
            distP1 = sqrt((P12[0].x - MPE.x) * (P12[0].x - MPE.x) +
                          (P12[0].y - MPE.y) * (P12[0].y - MPE.y));
            distP2 = sqrt((P12[1].x - MPE.x) * (P12[1].x - MPE.x) +
                          (P12[1].y - MPE.y) * (P12[1].y - MPE.y));

            if (distP1 < distP2) {

                itl_iats->PD1 = P12[0];
                itl_iats->PD2 = P12[1];
            } else {
                itl_iats->PD1 = P12[1];
                itl_iats->PD2 = P12[0];
            }
        }

    }
    else {
        // retas paralelas
        Point2f MDP;
        MDP.x = (int) ((PL1.x + PL2.x + PR1.x + PR2.x) / 4.0);
        MDP.y = (int) ((PL1.y + PL2.y + PR1.y + PR2.y) / 4.0);

        angD = ((atan(tanL) * 180.0 / CV_PI) + (atan(tanR) * 180 / CV_PI)) / 2.0;
        tanD = angD * (CV_PI / 180.0);
        bD = MDP.y - tanD * MDP.x;

        Point PT;
        vector<Point2f> P12;

        PT.x = 0;
        PT.y = (int) (tanD * PT.x + bD);

        if ((PT.y >= 0) && (PT.y < Lins)) {
            P12.push_back(PT);
        }

        PT.x = Cols - 1;
        PT.y = (int) (tanD * PT.x + bD);

        if ((PT.y >= 0) && (PT.y < Lins)) {
            P12.push_back(PT);
        }

        PT.y = 0;
        PT.x = (int) ((PT.y - bD) / tanD);

        if ((PT.x >= 0) && (PT.x < Cols)) {
            P12.push_back(PT);
        }

        PT.y = Lins - 1;
        PT.x = (int) ((PT.y - bD) / tanD);

        if ((PT.x >= 0) && (PT.x < Cols)) {
            P12.push_back(PT);
        }

        if(P12.size() >= 2) {
            distP1 = sqrt((P12[0].x - MPE.x) * (P12[0].x - MPE.x) +
                          (P12[0].y - MPE.y) * (P12[0].y - MPE.y));
            distP2 = sqrt((P12[1].x - MPE.x) * (P12[1].x - MPE.x) +
                          (P12[1].y - MPE.y) * (P12[1].y - MPE.y));


            if (distP1 < distP2) {
                itl_iats->PD1 = P12[0];
                itl_iats->PD2 = P12[1];
            } else {
                itl_iats->PD1 = P12[1];
                itl_iats->PD2 = P12[0];
            }
        }
    }
}
void computeDirectionLINE2()
{

    itl_iats = ImAts.begin();

    Point2f PD1, PD2;
    Point2f PL1, PL2, PR1, PR2;

    // extreme lines
    PL1 = itl_iats->PL1;   //itl_iats->margLINES[0].margP1;
    PL2 = itl_iats->PL2;   //itl_iats->margLINES[0].margP2;

    PR1 = itl_iats->PR1;  // itl_iats->margLINES[1].margP1;
    PR2 = itl_iats->PR2;  // itl_iats->margLINES[1].margP2;


    // directioal points
    PD1.x = (PL2.x + PR1.x) / 2.0f;
    PD1.y = (PL2.y + PR1.y) / 2.0f;

    PD2.x = (PL1.x + PR2.x) / 2.0f;
    PD2.y = (PL1.y + PR2.y) / 2.0f;

    // save directional points
    itl_iats->PD1 = PD1;
    itl_iats->PD2 = PD2;

}
void computeEL(Mat new_frame, Point2f P1, Point2f P2, vector<Point2f>& EL)
{

    int DS = LOW_RES_AROUND_LINE;

    int new_cols = new_frame.cols;
    int new_rows = new_frame.rows;

    Point2f P3;

    double dx = (double)(P2.x - P1.x); if (dx == 0) dx = 0.05;
    double dy = (double)(P2.y - P1.y);

    double TAN1 = dy / dx; if (TAN1 == 0) TAN1 = 0.05;
    double B1 = P1.y - TAN1 * P1.x;
    double X1, Y1, X3, Y3;

    double TAN3 = -1.0 / TAN1;
    double B3;
    if (fabs(dx) > fabs(dy)) {

        if (P2.x > P1.x)
        {
            for (int i = (int)P1.x; i <= (int)P2.x; i++)
            {
                X1 = (double)i;
                Y1 = TAN1 * X1 + B1;
                B3 = Y1 - TAN3 * X1;
                for (int j = (int)(Y1 - DS); j <= (int)(Y1 + DS); j++)
                {
                    Y3 = (double)j;
                    X3 = (Y3 - B3) / TAN3;
                    P3.x = (float) X3;
                    P3.y = (float) Y3;
                    if ((X3 >= 0) && (Y3 >= 0) && (X3 < new_cols) && (Y3 < new_rows))
                        EL.push_back(Point2f((float) X3, (float) Y3));
                }
            }
        }
        else
        {
            for (int i = (int)P2.x; i <= (int)P1.x; i++)
            {
                X1 = (double)i;
                Y1 = TAN1 * X1 + B1;
                B3 = Y1 - TAN3 * X1;
                for (int j = (int)(Y1 - DS); j <= (int)(Y1 + DS); j++)
                {
                    Y3 = (double)j;
                    X3 = (Y3 - B3) / TAN3;
                    if ((X3 >= 0) && (Y3 >= 0) && (X3 < new_cols) && (Y3 < new_rows))
                        EL.push_back(Point2f((float) X3, (float) Y3));
                }
            }
            //printf("2\n");
        }
    }
    else {
        if (P2.y > P1.y)
        {
            for (int i = (int)P1.y; i <= (int)P2.y; i++)
            {
                Y1 = (double)i;
                X1 = (Y1 - B1) / TAN1;
                B3 = Y1 - TAN3 * X1;
                for (int j = (int) (X1 - DS); j <= (X1 + DS); ++j){

                    X3 = (double)j;
                    Y3 = TAN3 * X3 + B3;
                    if ((X3 >= 0) && (Y3 >= 0) && (X3 < new_cols) && (Y3 < new_rows))
                        EL.push_back(Point2f((float) X3, (float) Y3));
                }
            }
        }
        else
        {
            for (int i = (int)P2.y; i <= (int)P1.y; i++)
            {
                Y1 = (double)i;
                X1 = (Y1 - B1) / TAN1;
                B3 = Y1 - TAN3 * X1;
                for (int j = (int) (X1 - DS); j <= X1 + DS; j++)
                {
                    X3 = (double)j;
                    Y3 = TAN3 * X3 + B3;
                    P3.x = (float) X3;
                    P3.y = (float) Y3;
                    if ((X3 >= 0) && (Y3 >= 0) && (X3 < new_cols) && (Y3 < new_rows))
                        EL.push_back(Point2f((float) X3, (float) Y3));
                }
            }
        }
    }
}

//D

//E
double entropySceneDirProb(Mat HL, Mat HR){
    //LOGI("called entropySceneDirProb()");

    double SL = 0;
    for (int i = 0; i < 180; i++)  SL = SL + HL.at<float>(i);
    if (SL != 0)  for (int i = 0; i < 180; i++)  HL.at<float>(i) = (float) (HL.at<float>(i) / SL);

    double SR = 0;
    for (int i = 0; i < 180; i++)  SR = SR + HR.at<float>(i);
    if (SR != 0)  for (int i = 0; i < 180; i++)  HR.at<float>(i) = (float) (HR.at<float>(i) / SR);

    double EL = 0;
    for (int i = 0; i < 180; i++)
    {
        if (HL.at<float>(i) != 0)
            EL = EL - HL.at<float>(i) * log2(HL.at<float>(i));
    }

    double ER = 0;
    for (int i = 0; i < 180; i++)
    {
        if (HR.at<float>(i) != 0)
            ER = ER - HR.at<float>(i) * log2(HR.at<float>(i));
    }

    if ((EL == 0) && (ER > 0)) EL = ER;
    if ((ER == 0) && (EL > 0)) ER = EL;

    double Emax = log2(180);

    double PED, PL, PR;

    PL = 1 - (EL / Emax);
    PR = 1 - (ER / Emax);

    PED = PL * PR;

    return PED;
}

//F
double fitAngle(double DirX, double DirY)
{
    double angle;

    //printf("P0 = %f P1 = %f\n", DirX, DirY);

    if (DirX == 0.00)  angle = 90.0;
    else{
        angle = (atan(DirY / DirX)) * 180.0 / CV_PI;

        if (angle > 180.0){
            if (angle <= 270.0){
                angle = angle - 180.0;
            }
            else{
                if (angle == 360.0){
                    angle = 0.0;
                }
                else{
                    angle = angle - 270.0;
                }
            }
        }
        else{
            if (angle == 180.0){
                angle = 0.0;
            }
        }

        if (angle < 0.0){
            if (angle >= -180.0){
                angle = 180 + angle;
            }
            else{
                angle = 360.0 + angle;
            }
        }
    }

    return angle;
}
double fitAngle0To180(double tang1, double tang2)
{

    double ang1 = atan(tang1) * 180.0 / CV_PI;
    double ang2 = atan(tang2) * 180.0 / CV_PI;

    if (ang1 < 0) ang1 = 180 + ang1;
    if (ang2 < 0) ang2 = 180 + ang2;

    //printf("ang1 = %.3f ang2 = %.3f\n",ang1,ang2);

    double tan_degree = (ang1 + ang2) / 2.0;

    if (tan_degree > 180.0) tan_degree = tan_degree - 180.0;

    if ((tan_degree < 0.0) && (tan_degree > -180.0)) tan_degree = 180 + tan_degree;

    if ((tan_degree < -180.0) && (tan_degree > -360.0)) tan_degree = 360 + tan_degree;

    if (tan_degree == 0.0) tan_degree = 0.005;
    if ((tan_degree == 180.0) || (tan_degree == 90.0)) tan_degree = tan_degree - 0.005;

    return tan_degree;
}
void frameSetting(Mat ParamCameraFrame){
    //LOGI("called frameSetting()");

    ParamCameraFrame.copyTo(src_color);

    int old_rows = src_color.rows;
    int old_cols = src_color.cols;
    int new_cols = old_cols, new_rows = old_rows;

    // diminui a resolução
    if (src_color.cols > LAR)
    {
        double tx = (double)LAR / (double)old_cols;
        new_cols = (int)(old_cols * tx);
        new_rows = (int)(old_rows * tx);
    }
    resize(src_color, src_color_small, cv::Size(new_cols, new_rows));

    cvtColor(src_color_small, src_gray_small, CV_BGR2GRAY);

    entrMap = -Mat::ones(src_gray_small.rows, src_gray_small.cols, CV_32F);

    psrSquareImage(src_color_small, &src_color_small_square);

    frame = 0;

}
void fitMaskAroundP(Mat frame, Mat& Pmask, Point P)
{

    int left, top, width, height;

    int Lins = frame.rows;
    int Cols = frame.cols;

    // Busca regiao around P
    itl_iats = ImAts.begin();

    left = P.x - AROUND_EXT_TRACK;
    if (left < 0) {
        left = 0;
    }

    top = P.y - AROUND_EXT_TRACK;
    if (top < 0) {
        top = 0;
    }

    width = 2 * AROUND_EXT_TRACK;
    if ((left + width) > (Cols - 1)) {
        width = Cols - left - 1;
    }

    height = 2 * AROUND_EXT_TRACK;
    if ((top + height) > (Lins - 1)) {
        height = Lins - top - 1;
    }

    frame(Rect(left, top, width, height)).copyTo(Pmask);

}

//G
double getAngle(Vec4i P){
    //LOGI("called getAngle()");

    double thetaD, thetaR;

    double DX = P[2] - P[0];
    double DY = P[3] - P[1];

    if (DX == 0)  thetaD = 90.0;
    else{
        thetaR = atan(DY / DX);
        thetaD = thetaR * 180.0 / CV_PI;

        if (thetaD > 180.0){
            if (thetaD <= 270){
                thetaD = thetaD - 180.0;
            }
            else{
                if (thetaD == 360.0){
                    thetaD = 0;
                }
                else{
                    thetaD = thetaD - 270.0;
                }
            }
        }
        else{
            if (thetaD == 180.0){
                thetaD = 0.0;
            }
        }

        if (thetaD < 0){
            if (thetaD >= -180.0){
                thetaD = 180 + thetaD;
            }
            else{
                thetaD = 360 + thetaD;
            }
        }
    }

    return thetaD;
}
double getMainAngle0To180(Point P1, Point P2){
    //LOGI("called getMainAngle0To180()");

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
double getqEntropyMaskOptmizer(Mat src_gray_small_2, Mat mask){
    //LOGI("called getqEntropyMaskOptmizer()");

    int intensity;

    int Lin = src_gray_small_2.rows;
    int Col = src_gray_small_2.cols;

    Mat Histgray = Mat::zeros(1, 256, CV_32F);

    double EntropyRegion = 0;
    double TL = 0;
    double TT = 0;

    int d = 5;
    int r, c;
    for (int y = d; y < Lin - d; y++)
        for (int x = d; x < Col - d; x++)
        {
            //printf("%d ", mask.at<uchar>(y, x));

            if (mask.at<uchar>(y, x) != 0)
            {
                if (entrMap.at<float>(y, x) >= 0)
                {
                    EntropyRegion = EntropyRegion + entrMap.at<float>(y, x);
                }
                else
                {
                    Histgray = 0;
                    TL = 0;
                    for (r = y - d; r <= y + d; r++)
                        for (c = x - d; c <= x + d; c++)
                        {
                            intensity = (int)src_gray_small_2.at<uchar>(r, c);
                            Histgray.at<float>(intensity)++;
                            TL = TL + 1;
                        }

                    // calcula a entropia
                    double SL = 0;
                    for (int L = 0; L < 256; L++)
                    {
                        double Pintesity = Histgray.at<float>(L) / TL;
                        if (Pintesity != 0)
                            SL = SL + Pintesity * log2(Pintesity);
                    }
                    entrMap.at<float>(y, x) = (float) (-SL);
                    EntropyRegion = EntropyRegion + entrMap.at<float>(y, x);
                }
                TT = TT + 1.0;
            }
        }

    if (TT != 0) {
        EntropyRegion = EntropyRegion / Lin * Col / 8000;
    }
    else {
        EntropyRegion = log2(256) * Lin * Col * 1000;
    }

    return EntropyRegion;
}
double getqEntropy(Mat src_gray_small_2, Mat mask){
    int intensity;

    int Lin = src_gray_small_2.rows;
    int Col = src_gray_small_2.cols;

    double EntropyRegion = 0;
    double Total = 0;
    double T = 0;

    int d = 5;
    int r, c;
    for (int y = d; y < Lin - d; y++)
        for (int x = d; x < Col - d; x++){
            if (mask.at<uchar>(y, x) != 0){
                Mat Histgray = Mat::zeros(1, 256, CV_32F);
                T = 0;
                for (r = y - d; r < y + d; r++)
                    for (c = x - d; c < x + d; c++){
                        intensity = (int)src_gray_small_2.at<uchar>(r, c);
                        Histgray.at<float>(intensity)++; // = Histgray.at<float>(intensity) + 1.0;
                        T = T + 1;
                    }

                // calcula a entropia
                double S = 0;
                for (int L = 0; L < 256; L++){
                    Histgray.at<float>(L) = (float) (Histgray.at<float>(L) / T);
                    double Pintesity = Histgray.at<float>(L);
                    if (Pintesity != 0)
                        S = S + Pintesity * log2(Pintesity);
                }
                Total = Total + 1.0;
                EntropyRegion = EntropyRegion - S;
            }
        }

    if (Total != 0) EntropyRegion = 1 - ((EntropyRegion / Total) / 8);
    else            EntropyRegion = log2(256) * Lin * Col;

    return EntropyRegion;
}
void getDirections(Mat main_street)
{
    Mat cannyImage;

    //cvtColor(src_color_small, src_gray_small, CV_BGR2GRAY);
    GaussianBlur(src_gray_small, cannyImage, Size(3, 3), 0, 0, BORDER_DEFAULT);
    Canny(cannyImage, cannyImage, 50, 200, 3);

    cv::HoughLinesP(cannyImage, lines, 1, CV_PI / 180, 10, 50, 10);
    for (size_t i = 0; i < lines.size(); i++)
    {
        Vec4i L = lines[i];
    }
    lines.clear();
}
void getqEntropyImage(Mat src, Mat* dst0){
    //LOGI("called getqEntropyImage()");

    int intensity;

    int Lin = src.rows;
    int Col = src.cols;

    double Total = 0;
    double T = 0;
    double q = 0.5;

    int d = 5;
    int r, c;

    Mat entropy_temp = Mat::zeros(Lin, Col, CV_32F);
    for (int y = d; y < Lin - d; y++)
        for (int x = d; x < Col - d; x++)
        {
            Mat Histgray = Mat::zeros(1, 256, CV_32F);
            T = 0;
            for (r = y - d; r < y + d; r++)
                for (c = x - d; c < x + d; c++)
                {
                    intensity = (int)src.at<uchar>(r, c);
                    Histgray.at<float>(intensity) = (float) (Histgray.at<float>(intensity) + 1.0);
                    T = T + 1;
                }

            // calcula a q-entropia
            double S = 0;
            for (int L = 0; L < 256; L++)
            {
                Histgray.at<float>(L) = (float) (Histgray.at<float>(L) / T);
                double Pintesity = Histgray.at<float>(L);
                if (Pintesity != 0)
                    S = S + pow(Pintesity, q);
            }

            S = (1 - S) / (q - 1);
            entropy_temp.at<float>(y, x) = (float) S;
            Total = Total + S;
            //printf("S = %.3f\n",S);
        }

    if (Total != 0) {
        for (int y = 0; y < Lin; y++)
            for (int x = 0; x < Col; x++)
                entropy_temp.at<float>(y, x) = (float) (entropy_temp.at<float>(y, x) / Total);
    }

    entropy_temp.copyTo((*dst0));

}
void getSobelGradient(Mat src0, Mat* dst0)
{
    //LOGI("called getSobelGradient");
    //int intensity;

    Mat dx, dy; // 1st derivative in x,y
    Mat src1;

    GaussianBlur(src0, src1, Size(3, 3), 0, 0, BORDER_DEFAULT);
    //cvtColor(src1, src1, CV_BGR2GRAY);

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;

    Mat grad_x, grad_y, gradImage;
    Mat abs_grad_x, abs_grad_y;
    Sobel(src1, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);

    Sobel(src1, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);

    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradImage);

    double Total = 0;
    for (int y = 0; y < src0.rows; y++)
        for (int x = 0; x < src0.cols; x++)
            Total = Total + gradImage.at<uchar>(y, x);

    Mat sobel_grad_Image = Mat::zeros(src0.rows, src0.cols, CV_32F);
    if (Total != 0)
    {
        for (int y = 0; y < src0.rows; y++)
            for (int x = 0; x < src0.cols; x++)
                sobel_grad_Image.at<float>(y, x) = (float) (gradImage.at<uchar>(y, x) / Total);
    }
    sobel_grad_Image.copyTo((*dst0));
}

//H
void HoughSegmentation(Mat src_gray0, Mat& dst_bw){

    Mat dst, cdst, canny, gray_lines;

    Canny(src_gray0, canny, 50, 200, 3);

    Mat hough_seg = Mat::zeros(src_gray0.size(), CV_8UC1);

    vector<Vec4i> lines_hough;
    HoughLinesP(canny, lines_hough, 1, CV_PI / 180, 20, 10, 5);
    for (size_t i = 0; i < lines_hough.size(); i++)
    {
        Vec4i l = lines_hough[i];
        line(hough_seg, Point(l[0], l[1]), Point(l[2], l[3]), 255, 2, CV_AA);
    }

    for (int y = 0; y < src_gray0.rows; y++)
        for (int x = 0; x < src_gray0.cols; x++)
        {
            if (hough_seg.at<uchar>(y, x) > 200)
                hough_seg.at<uchar>(y, x) = 0;
            else
                hough_seg.at<uchar>(y, x) = 255;
        }

    hough_seg.copyTo(dst_bw);
}

//I
void InicializaRecognitionFromWebCam(Mat ParamCameraFrame){
    ParamCameraFrame.copyTo(src_color);

    int old_rows = src_color.rows;
    int old_cols = src_color.cols;
    int new_cols = old_cols, new_rows = old_rows;
    if (src_color.cols > 400)
    {
        double tx = 400.0 / old_cols;
        new_cols = (int)(old_cols * tx);
        new_rows = (int)(old_rows * tx);
    }

    //printf("L = %d C = %d\n", new_rows, new_cols);  system("pause");

    resize(src_color, src_color_small, cv::Size(new_cols, new_rows));
    psrSquareImage(src_color_small, &src_color_small_square);

    // segmentaÃƒÂ§ÃƒÂ£o
    cvtColor(src_color_small, src_gray_small, CV_BGR2GRAY);
    psrGradSobelqEntropySegment(src_gray_small, &seg_small);
    psrSquareImage(seg_small, &seg_small_square);

}
void InicializaRAStreet(){

    double dp = 20;
    double fat = 1 / sqrt(2 * CV_PI * dp * dp);
    //double Ymax = 1.0 / (sqrt(2 * CV_PI * dp * dp));

    for (int i = 0; i < 90.0; i++)
    {
        double a = (-1.0 / 2.0) * pow(((double)i - 45.0) / dp, 2.0);
        //printf("a = %f\n",a);
        OBJ_VAR[i] = fat * exp(a) * 10.0;
        OBJ_VAR[i + 90] = OBJ_VAR[i] * 10.0;
    }

    xprojection();

    yprojection();

    zprojection();

    // temporario

    mask_optflow = Mat::zeros(226, 400, CV_8UC3);

    // temporario
    // Create some random colors
    for (int i = 0; i < 100; i++)
    {
        int r = rng.uniform(0, 256);
        int g = rng.uniform(0, 256);
        int b = rng.uniform(0, 256);
        colors.push_back(Scalar(r, g, b));
    }

    Avera_des0.x = 0;
    Avera_des0.y = 0;

    Avera_des1.x = 0;
    Avera_des1.y = 0;

    Avera_des_pc0.x = 0;
    Avera_des_pc0.y = 0;

    Total_desl = 0.0;

}
void InitialStreetRecognition(Mat im1){
    //LOGI("called InitialStreetRecognition()");

    Mat seg0_small;
    CannySegmentation(im1, &seg0_small);

    seg0_small.copyTo(seg_small);

    /// Find contours
    findContours(seg_small, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    LOGI("contours.size() = %d  contours.size().size() = %d", contours.size(), contours[0].size());

    // ordena pela maior Área
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    /// Find the convex hull object for the first 5 contour
    double TOT_AREA = 0, TOT_ENTR = 0;
    Mat tmp_street = Mat::zeros(src_gray_small.size(), CV_8UC1);

    for (int i = 0; i < 5; i++){
        ImageAtributs IA;

        //IA <- indx
        IA.indx = i;

        // desenha cada contorno em uma imagem small
        tmp_street = 0;
        drawContours(tmp_street, contours, i, 255, -1, 8, vector<Vec4i>(), 0, Point());
        dilate(tmp_street, tmp_street, Mat(), Point(-1, -1), 2, 1, 1);

        tmp_street.copyTo(IA.RegionMask);  // IA <- region mask

        // acha o convex hull de cada região entre as 5 maiores ordenadas por Área
        convexHull(contours[IA.indx], hull[IA.indx], false);

        // desenha o hull de cada reviao
        tmp_street = 0;
        drawContours(tmp_street, hull, IA.indx, 255, -1, 8, vector<Vec4i>(), 0, Point());
        tmp_street.copyTo(main_street_small);
        tmp_street.copyTo(IA.RegionHull); //IA <- region hull

        // calcula as arestas poligonais de cada hull
        cv::approxPolyDP(hull[IA.indx], approxContour, arcLength(Mat(hull[IA.indx]), true) * 0.005, true);

        psrFocusDetectionBasedOnFeatureExpansion(src_gray_small, IA.RegionHull, approxContour, &IA);

        IA.Area = contourArea(contours[i], false);
        IA.indx = i;
        ImAts.push_back(IA);

        TOT_AREA = TOT_AREA + IA.Area;
        TOT_ENTR = TOT_ENTR + IA.PERM;
    }

    //double b;
    double af, bt, gm;
    af = 0.15; bt = 0.7; gm = 0.15;
    for (itl_iats = ImAts.begin(); itl_iats != ImAts.end(); ++itl_iats)
    {
        double PS = SkyProb(itl_iats->RegionMask);
        if (PS <= 0.7)
        {
            itl_iats->Area = itl_iats->Area / TOT_AREA;
            itl_iats->PERM = 1 - (itl_iats->PERM / TOT_ENTR);
            itl_iats->PStreet = pow(itl_iats->Area, af) * pow(itl_iats->Pfocus, bt) * pow(itl_iats->PERM, gm);
        }
        else
            itl_iats->PStreet = 0.0;
    }

    // ordena pela maior chance de ser pista, por entropia e area
    // quanto maior e mais limpo -> maior Ã© a chance de ser pista
    std::sort(ImAts.begin(), ImAts.end(), CompS);

}

//J
//K

//L
void LocalOpticalFlow(std::vector <ImageAtributs>& ImAts, Mat new_frame) {
    //LOGI("called LocalOpticalFlow()");

    const int BOUNDINGX = 60;
    const int BOUNDINGY = 60;

    int Lins = new_frame.rows;
    int Cols = new_frame.cols;

    Point PC, P1, P2, P3, P4;
    //int top, left, width, height;
    Rect r_pc;

    itl_iats = ImAts.begin();

    PC.x = (int)(Cols / 2.0);
    PC.y = (int)(Lins / 2.0);

    Mat reg_around_PC;
    roiAroundPoint(PC, new_frame, reg_around_PC, BOUNDINGX, BOUNDINGY, r_pc);

    Total_des_pc0 = 0;
    Total_des_pc1 = 0;
    if (INITIAL_STATUS) {

        // atualiza os pontos fiduciais apenas
        goodFeaturesToTrack(reg_around_PC, p0_pc, 100, 0.3, 7, Mat(), 7, false, 0.04);

        for (int i = 0; i < p0_pc.size(); i++) {

            Avera_des_pc0.x = Avera_des_pc0.x + p0_pc[i].x;
            Avera_des_pc0.y = Avera_des_pc0.y + p0_pc[i].y;

            Total_des_pc0 = Total_des_pc0 + 1;
        }

        Avera_des_pc0.x = (float)((Avera_des_pc0.x / Total_des_pc0) + r_pc.x);
        Avera_des_pc0.y = (float)((Avera_des_pc0.y / Total_des_pc0) + r_pc.y);

        old_reg_pc = reg_around_PC.clone();


    }
    else {

        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);

        /////////////////////////////////////////////////////////////////////////////////////////////
        vector<Point2f> p1_pc;
        Point2f p1_pc_;
        vector<uchar> statuspc;
        vector<float> errpc;

        cv::calcOpticalFlowPyrLK(old_reg_pc, reg_around_PC, p0_pc, p1_pc, statuspc, errpc, Size(15, 15), 2, criteria);

        Avera_des_pc1.x = 0;
        Avera_des_pc1.y = 0;
        for (int i = 0; i < p1_pc.size(); i++)
        {
            Avera_des_pc1.x = Avera_des_pc1.x + p1_pc[i].x;
            Avera_des_pc1.y = Avera_des_pc1.y + p1_pc[i].y;
            Total_des_pc1 = Total_des_pc1 + 1;
        }
        Avera_des_pc1.x = (float) ((Avera_des_pc1.x / Total_des_pc1) + r_pc.x);
        Avera_des_pc1.y = (float) ((Avera_des_pc1.y / Total_des_pc1) + r_pc.y);

        // desenha 0 pra 1
        Mat img;

        Avera_des_pc0 = Avera_des_pc1;

        // guarda as informacoes
        old_reg_pc = reg_around_PC.clone();
    }
}

//M
void ModelRegistration(void){
    //LOGI("called ModelRegistration()");

    itl_iats = ImAts.begin();

    Point2f PL1, PL2, PR1, PR2;

    computeDirectionLINE();

    MODEL_POS.x = (float) (MODEL_POS.x - 0.1); // Avera_des0.x;
    MODEL_POS.y = (float) (tanD * MODEL_POS.x + bD + 87); // Avera_des0.y;
    LOGI("----------- MODEL_POS.x = %.4f  MODEL_POS.y = %.4f", MODEL_POS.x, MODEL_POS.y);

    PROJ[0] = 30;
    PROJ[1] = (float) -angD;

}
void makeTextImage(Mat im0){
    //LOGI("called makeTextImage()");

    Vec3b intensity;

    for (int y = 0; y < im0.rows; y++)
        for (int x = 0; x < im0.cols; x++)
        {
            intensity = im0.at<Vec3b>(x, y);
            MTextImg[y][x][2] = 255 - intensity.val[0];
            MTextImg[y][x][1] = 255 - intensity.val[1];
            MTextImg[y][x][0] = 255 - intensity.val[2];
            MTextImg[y][x][3] = 0;
        }
}

//N
void newMarginalLines(std::vector<Point> approxContourL, std::vector <ImageAtributs>& ImAts){
    //LOGI("called newMarginalLines()");

    std::vector<Point> LeftContours;
    std::vector<Point> RightContours;

    //const int PERIM_PISTA = 10;
    const int MARG = 10;

    Point PL1, PL2, PR1, PR2;
    Point2f PD1, PD2;

    int Lins = src_gray_small.rows;
    int Cols = src_gray_small.cols;

    //printf("L = %d C = %d\n",Lins, Cols);
    //system("pause");

    bool valid = 0;
    itl_iats = ImAts.begin();

    PL1 = itl_iats->PL1;      //itl_iats->margLINES[0].margP1;
    PL2 = itl_iats->PL2;      //itl_iats->margLINES[0].margP2;
    PR1 = itl_iats->PR1;      //itl_iats->margLINES[1].margP1;
    PR2 = itl_iats->PR2;      //itl_iats->margLINES[1].margP2;

    //bool validl = 0, validr = 0;
    Point PL1t, PL2t, PR1t, PR2t;
    for (int i = 0; i < approxContourL.size() - 1; i++) {

        valid = 0;
        Point P1, P2, P3, P4;

        P1.x = approxContourL[i].x;
        P1.y = approxContourL[i].y;

        P2.x = approxContourL[i + 1].x;
        P2.y = approxContourL[i + 1].y;

        if (((P1.x >= (Cols - MARG)) && (P2.x >= (Cols - MARG))) || ((P1.x <= MARG) && (P2.x <= MARG)) || ((P1.y <= MARG) && (P2.y <= MARG)) || ((P1.y >= (Lins - MARG)) && (P2.y >= (Lins - MARG)))) {
            valid = 0;
        }
        else {
            valid = 1;
        }

        if (valid) {
            if (P2.x > P1.x) {
                LeftContours.push_back(P1);
                LeftContours.push_back(P2);
                //printf("1\n");
            }
            else {
                if (P2.x < P1.x)
                {
                    RightContours.push_back(P1);
                    RightContours.push_back(P2);
                    //printf("2\n");
                }
                else
                {
                    if (P2.y > P1.y)
                    {
                        LeftContours.push_back(P1);
                        LeftContours.push_back(P2);
                    }
                    else {
                        RightContours.push_back(P1);
                        RightContours.push_back(P2);
                    }
                }
            }
        }
    }


    if (LeftContours.size() > 0) {

        int indt = 0;
        double d1, dmax = -10000;
        Point Pt1, Pt2;
        for (int i = 0; i < LeftContours.size() - 1; i++) {

            Pt1 = LeftContours[i];
            Pt2 = LeftContours[i + 1];

            d1 = sqrt((Pt1.x - Pt2.x) * (Pt1.x - Pt2.x) + (Pt1.y - Pt2.y) * (Pt1.y - Pt2.y));
            if (d1 > dmax) {
                dmax = d1;
                indt = i;
            }
        }
        itl_iats->PL1 = LeftContours[indt];
        itl_iats->PL2 = LeftContours[indt + 1];
    }
    else
    {
        itl_iats->PL1.x = 0;
        itl_iats->PL1.y = 0;
        itl_iats->PL2.x = Cols - 1;
        itl_iats->PL2.y = 0;
        itl_iats->validl = 0;
    }


    if (RightContours.size() > 0) {

        int indt=0;
        double d1, dmax = -10000;
        Point Pt1, Pt2;
        for (int i = 0; i < RightContours.size() - 1; i++) {

            Pt1 = RightContours[i];
            Pt2 = RightContours[i + 1];

            d1 = sqrt((Pt1.x - Pt2.x) * (Pt1.x - Pt2.x) + (Pt1.y - Pt2.y) * (Pt1.y - Pt2.y));
            if (d1 > dmax) {
                dmax = d1;
                indt = i;
            }
        }
        itl_iats->PR1 = RightContours[indt];
        itl_iats->PR2 = RightContours[indt + 1];
    }
    else
    {
        itl_iats->PR1.x = 0;
        itl_iats->PR1.y = Lins - 1;
        itl_iats->PR2.x = Cols - 1;
        itl_iats->PR2.y = Lins - 1;
        itl_iats->validr = 0;
    }

}
void newmainlines(Mat new_frame){
    //LOGI("called newmainlines()");

    itl_iats = ImAts.begin();

    Point2f PL1a, PL2a, PR1a, PR2a;

    PL1a = itl_iats->PL1; // margLINES[0].margP1;
    PL2a = itl_iats->PL2; // margLINES[0].margP2;
    PR1a = itl_iats->PR1; // margLINES[1].margP1;
    PR2a = itl_iats->PR2; // margLINES[1].margP2;

    Mat exp_hull_mask;

    exp_hull_mask = itl_iats->RegionHull.clone();

    dilate(exp_hull_mask, exp_hull_mask, Mat(), Point(-1, -1), 8, 1, 1);

    findContours(exp_hull_mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    Rect r = boundingRect(contours[0]);

    Mat new_frame_roi = new_frame(r);

    Mat seg;
    CannySegmentation(new_frame_roi, &seg);

    findContours(seg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    // ordena pela maior Ã¡rea
    std::sort(contours.begin(), contours.end(), compareContourAreas);

    seg = 0;
    drawContours(seg, contours, 0, 255, -1, 8, vector<Vec4i>(), 0, Point());

    //imshow("draw", seg);
    //waitKey(0);

    Scalar value;
    int borderType = BORDER_CONSTANT;
    value = Scalar(0, 0, 0);
    Mat seg_e;
    int top = r.y;
    int bottom = new_frame.rows - r.y - r.height;
    int left = r.x;
    int right = new_frame.cols - r.x - r.width;
    copyMakeBorder(seg, seg_e, top, bottom, left, right, borderType, value);
    itl_iats->RegionMask = seg_e.clone();

    Mat tmp1, tmp2;
    tmp1 = new_frame.clone();

    findContours(itl_iats->RegionMask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    convexHull(contours[0], hull2[0], false);

    Mat imhull(new_frame.rows, new_frame.cols, CV_8UC1);
    imhull = 0;
    drawContours(imhull, hull2, 0, 255, -1, 8, vector<Vec4i>(), 0, Point());
    itl_iats->RegionHull = imhull.clone();

    approxContour.clear();
    cv::approxPolyDP(hull2[0], approxContour, arcLength(Mat(hull[0]), true) * 0.005, true);

    Point PL1, PL2, PR1, PR2;

    approxContour.push_back(Point(approxContour[0].x, approxContour[0].y));

    newMarginalLines(approxContour, ImAts);

    PL1 = itl_iats->PL1;
    PL2 = itl_iats->PL2;
    Point Pli, Plf;
    LineExtremePoints(PL1, PL2, &Pli, &Plf, new_frame.rows, new_frame.cols);
    itl_iats->margLINES[0].margP1 = Pli;
    itl_iats->margLINES[0].margP2 = Plf;

    PR1 = itl_iats->PR1;
    PR2 = itl_iats->PR2;
    Point Pri, Prf;
    LineExtremePoints(PR1, PR2, &Pri, &Prf, new_frame.rows, new_frame.cols);
    itl_iats->margLINES[1].margP1 = Pri;
    itl_iats->margLINES[1].margP2 = Prf;

}

//O

//P
double PolarDist(Point2f P1, Point2f P2, Point2f P3, Point2f P4){

    double rho1, theta1;
    double rho2, theta2;

    RhoTheta(P1, P2, &rho1, &theta1);
    RhoTheta(P3, P4, &rho2, &theta2);

    double DIST = sqrt(((rho1 - rho2) * (rho1 - rho2)) + ((theta1 - theta2) * (theta1 - theta2)));

    return DIST;
}
double ProbSceneDirBasedOnLine(vector<Point> splitLine){
    //LOGI("called ProbSceneDirBasedOnLine()");

    Point P1, P2, P3;

    //double MainAngle;
    double MargAngle;

    Mat HL = Mat::zeros(1, 180, CV_32F);
    Mat HR = Mat::zeros(1, 180, CV_32F);

    vector<Point>::iterator itl;

    itl = splitLine.begin();
    P1.x = itl->x;
    P1.y = itl->y;

    ++itl;
    P2.x = itl->x;
    P2.x = itl->y;

    for (size_t i = 0; i < MargLines.size(); i++)
    {
        // get a line from scene
        Vec4i MargLine = MargLines[i];

        // central point of marginal line (CPML)
        P3.x = (int)((double)(MargLine[2] + MargLine[0]) / 2.0);
        P3.y = (int)((double)(MargLine[3] + MargLine[1]) / 2.0);

        // get angle of Marginal line
        MargAngle = getAngle(MargLine);

        // compute if CPML is on the leff or on the right of Main line
        int ED = (P2.x - P1.x) * (P3.y - P1.y) - (P3.x - P1.x) * (P2.y - P1.y);

        if (ED <= 0) //CPML is left of Main line
        {
            HL.at<float>((int)MargAngle)++;
        }
        else //CPML is right of Main line
        {
            HR.at<float>((int)MargAngle)++;
        }
    }

    // Probability Entropy Distribution
    double PED;
    PED = entropySceneDirProb(HL, HR);

    return PED;
}
void psrFocusDetectionBasedOnFeatureExpansion(Mat src_gray_small, Mat reg_bw_small, std::vector<Point> approxContourL, ImageAtributs* ImAl){
    //LOGI("called psrFocusDetectionBasedOnFeatureExpansion()");

    Point P1, P2, P3, P4, P5, PC, CP;
    int R1, R2, R3, R4;

    Point Pt1, Pt2, Pt3, Pt4;

    double MinEmpRegion = LONG_MAX;

    Mat mask = Mat::zeros(src_gray_small.rows, src_gray_small.cols, CV_8UC1);
    Mat gray;

    src_gray_small.copyTo(gray);
    for (int i = 0; i < 5; i++)
        GaussianBlur(gray, gray, Size(3, 3), 0, 0, BORDER_DEFAULT);

    int Cx = 0; int Cy = 0; int T = 0;
    for (int y = 0; y < src_gray_small.rows; y++)
        for (int x = 0; x < src_gray_small.cols; x++)
            if (reg_bw_small.at<uchar>(y, x) > 0)
            {
                Cx = Cx + x;
                Cy = Cy + y;
                T = T + 1;
            }

    PC.x = (int)(Cx / T);
    PC.y = (int)(Cy / T);

    P1.x = approxContourL[0].x;
    P1.y = approxContourL[0].y;
    approxContourL.push_back(P1);

    Mat tmp;

    P1.x = approxContourL[0].x;
    P1.y = approxContourL[0].y;
    for (int i = 1; i < approxContourL.size(); i++)
    {
        P2.x = approxContourL[i].x;
        P2.y = approxContourL[i].y;

        P3.x = P2.x;
        P3.y = P2.y;
        for (int j = i + 1; j < approxContourL.size(); j++)
        {
            P4.x = approxContourL[j].x;
            P4.y = approxContourL[j].y;

            double Xm = 0, Ym = 0, Tm = 0;
            for (int y = 0; y < src_gray_small.rows; y++)
            {
                for (int x = 0; x < src_gray_small.cols; x++)
                {
                    P5.x = x;
                    P5.y = y;
                    R1 = (P2.x - P1.x) * (P5.y - P1.y) - (P5.x - P1.x) * (P2.y - P1.y);
                    R2 = (P4.x - P3.x) * (P5.y - P3.y) - (P5.x - P3.x) * (P4.y - P3.y);

                    R3 = (P2.x - P1.x) * (PC.y - P1.y) - (PC.x - P1.x) * (P2.y - P1.y);
                    R4 = (P4.x - P3.x) * (PC.y - P3.y) - (PC.x - P3.x) * (P4.y - P3.y);

                    if (((R1 * R2) > 0) && ((R2 > 0) && (R4 > 0)))
                    {
                        mask.at<uchar>(y, x) = 255;
                        Xm = Xm + x;
                        Ym = Ym + y;
                        Tm = Tm + 1;
                    }
                }
            }
            Xm = Xm / Tm;
            Ym = Ym / Tm;

            double EMO = getqEntropyMaskOptmizer(gray, mask);

            if (EMO < MinEmpRegion)
            {
                MinEmpRegion = EMO;

                CP.x = (int)Xm;
                CP.y = (int)Ym;

                Pt1.x = P1.x;
                Pt1.y = P1.y;

                Pt2.x = P2.x;
                Pt2.y = P2.y;

                Pt3.x = P3.x;
                Pt3.y = P3.y;

                Pt4.x = P4.x;
                Pt4.y = P4.y;
            }

            P3.x = P4.x;
            P3.y = P4.y;

            mask = 0;
        }

        P1.x = P2.x;
        P1.y = P2.y;
    }

    (*ImAl).CP.x = CP.x;
    (*ImAl).CP.y = CP.y;

    double dx1 = Pt2.x - Pt1.x;  if (dx1 == 0) dx1 = dx1 + 0.005;
    double dy1 = Pt2.y - Pt1.y;

    double dx2 = Pt4.x - Pt3.x;  if (dx2 == 0) dx2 = dx2 + 0.005;
    double dy2 = Pt4.y - Pt3.y;

    double tan1 = dy1 / dx1; if (tan1 == 0) tan1 = tan1 + 0.005;
    double tan2 = dy2 / dx2; if (tan2 == 0) tan2 = tan2 + 0.005;

    double b1 = Pt1.y - tan1 * Pt1.x;
    double b2 = Pt3.y - tan2 * Pt3.x;

    double XmF, YmF;
    int Cols = src_gray_small.cols;
    int Lins = src_gray_small.rows;

    // specific situations
    XmF = (b2 - b1) / (tan1 - tan2);
    if (XmF < X0) XmF = 0;
    if (XmF >= Cols) XmF = Cols;

    YmF = tan1 * XmF + b1;
    if (YmF < 0)
    {
        YmF = 0;
        XmF = (YmF - b1) / tan1;
    }

    if (YmF >= Lins)
    {
        YmF = Lins;
        XmF = (YmF - b1) / tan1;
    }

    (*ImAl).MP.x = (int) XmF;
    (*ImAl).MP.y = (int) YmF;


    double mA1 = getMainAngle0To180(Pt1, Pt2);
    double mA2 = getMainAngle0To180(Pt3, Pt4);
    double mA = (mA1 + mA2) / 2.0;

    (*ImAl).MainAngle = mA;

    Point P1cp, P2cp;
    P1cp.x = (*ImAl).MP.x;
    P1cp.y = (*ImAl).MP.y;
    P2cp.x = (*ImAl).CP.x;
    P2cp.y = (*ImAl).CP.y;
    double mACP = getMainAngle0To180(P1cp, P2cp);

    (*ImAl).MAIN_ANGLE_DIR = mACP;

    (*ImAl).PL1 = Pt1;
    (*ImAl).PL2 = Pt2;
    (*ImAl).PR1 = Pt3;
    (*ImAl).PR2 = Pt4;

    if (((Pt1.x == (LAR - 1)) && (Pt2.x == (LAR - 1))) || ((Pt1.x == 0) && (Pt2.x == 0)) || ((Pt1.y == 0) && (Pt2.y == 0)) || ((Pt1.y == (ALT - 1)) && (Pt2.y == (ALT - 1)))) {
        (*ImAl).validl = 0;
    }
    else {
        (*ImAl).validl = 1;
    }

    if (((Pt3.x == (LAR - 1)) && (Pt4.x == (LAR - 1))) || ((Pt3.x == 0) && (Pt4.x == 0)) || ((Pt3.y == 0) && (Pt4.y == 0)) || ((Pt3.y == (ALT - 1)) && (Pt4.y == (ALT - 1)))) {
        (*ImAl).validr = 0;
    }
    else {
        (*ImAl).validr = 1;
    }

    margLINE margLL, margLR;
    Point Pi, Pf;
    LineExtremePoints(Pt1, Pt2, &Pi, &Pf, src_gray_small.rows, src_gray_small.cols);
    margLL.margP1 = Pi;
    margLL.margP2 = Pf;
    margLL.valid = 1;
    (*ImAl).margLINES.push_back(margLL);

    LineExtremePoints(Pt3, Pt4, &Pi, &Pf, src_gray_small.rows, src_gray_small.cols);
    margLR.margP1 = Pi;
    margLR.margP2 = Pf;
    margLR.valid = 1;
    (*ImAl).margLINES.push_back(margLR);

    (*ImAl).PERM = (double)MinEmpRegion;

    vector<Point> Line;
    Line.push_back(CP);
    Line.push_back((*ImAl).MP);

    (*ImAl).Pfocus = ProbSceneDirBasedOnLine(Line);

}
void psrSobelSegmentation(Mat src0, Mat* dst2)
{
    //int intensity;

    int Lin_ori = src0.rows;
    int Col_ori = src0.cols;

    Mat dx, dy; // 1st derivative in x,y
    Mat src1;
    GaussianBlur(src0, src1, Size(3, 3), 0, 0, BORDER_DEFAULT);
    cvtColor(src1, src1, CV_BGR2GRAY);

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;

    Mat grad_x, grad_y, gradImage;
    Mat abs_grad_x, abs_grad_y;
    Sobel(src1, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_x, abs_grad_x);

    Sobel(src1, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
    convertScaleAbs(grad_y, abs_grad_y);

    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, gradImage);

    int Lin_new = (int)(Lin_ori * 0.2);
    int Col_new = (int)(Col_ori * 0.2);

    resize(src1, src1, cv::Size(Col_new, Lin_new));

    int erosion_elem = 1;
    int erosion_size = 3;
    //int const max_elem = 2;
    //int const max_kernel_size = 21;
    int erosion_type = MORPH_RECT;
    if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
    else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
    else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement(erosion_type, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));


    for (int y = 0; y < Lin_ori; y++)
        for (int x = 0; x < Col_ori; x++)
        {
            if (gradImage.at<uchar>(y, x) < 30)
                gradImage.at<uchar>(y, x) = 255;
            else
                gradImage.at<uchar>(y, x) = 0;

            //double Dire = (double)gradImage.at<uchar>(y, x);
            //segImage.at<uchar>(y, x) = (uchar)Dire;
        }

    erode(gradImage, gradImage, element);

    gradImage.copyTo((*dst2));

}
void psrGradSobelqEntropySegment(Mat src_gray, Mat* dst_seg){
    //LOGI("called psrGradSobelqEntropySegment()");

    Mat seg_grd = Mat::zeros(src_gray.rows, src_gray.cols, CV_32F);
    Mat seg_ent = Mat::zeros(src_gray.rows, src_gray.cols, CV_32F);

    getqEntropyImage(src_gray, &seg_grd);
    getSobelGradient(src_gray, &seg_ent);

    Mat seg_grd_ent = Mat::zeros(src_gray.rows, src_gray.cols, CV_8UC1);

    double PE, PG;
    for (int y = 0; y < src_gray.rows; y++)
        for (int x = 0; x < src_gray.cols; x++)
        {
            PE = seg_grd.at<float>(y, x) * 10000.0;
            PG = seg_ent.at<float>(y, x) * 10000.0;

            //printf("PE = %f PG = %f\n",PE,PG);

            if (((PE + PG) / 2.0) < 0.1)
                seg_grd_ent.at<uchar>(y, x) = 255;
            else
                seg_grd_ent.at<uchar>(y, x) = 0;
        }

    seg_grd_ent.copyTo((*dst_seg));

}
void psrSquareImage(Mat src0, Mat* dst2){
    //LOGI("called psrSquareImage()");

    //src.rows = 438;
    int top = 0, bottom = 0, left = 0, right = 0;
    if (src0.rows < src0.cols)
    {
        if (!((src0.cols - src0.rows) % 2))
        {// ÃƒÂ© par
            top = (src0.cols - src0.rows) / 2; // (int)((src.rows - src.cols) / 2.0);
            bottom = (src0.cols - src0.rows) / 2; // top;
            left = 0;
            right = 0;
        }
        else
        {
            top = ((src0.cols - src0.rows) / 2) + 1; // (int)((src.rows - src.cols) / 2.0);
            bottom = (src0.cols - src0.rows) / 2; // top;
            left = 0;
            right = 0;
        }
    }
    else
    {
        if (src0.rows > src0.cols)
        {
            if (!((src0.rows - src0.cols) % 2))
            {// ÃƒÂ© par
                top = 0;
                bottom = 0;
                left = (src0.rows - src0.cols) / 2;
                right = (src0.rows - src0.cols) / 2;
            }
            else
            {
                top = 0;
                bottom = 0;
                left = (src0.rows - src0.cols) / 2;
                right = (src0.rows - src0.cols) / 2;
            }
        }
    }

    Scalar value;
    int borderType = BORDER_CONSTANT;
    value = Scalar(0, 0, 0);
    copyMakeBorder(src0, (*dst2), top, bottom, left, right, borderType, value);
}

//Q

//R
void RhoTheta(Point2f P1, Point2f P2, double* rho, double* theta){
    //LOGI("called RhoTheta()");

    double dx1, dy1;
    double rho1, theta1, theta2;
    double tan1, tan2;
    double b, x1, y1;

    dx1 = P2.x - P1.x;
    if (dx1 == 0) {
        //printf("P1x = %d\n",P1.x);
        theta1 = 90.0;
        theta2 = 0.0;
        rho1 = (double)P1.x;
    }
    else
    {
        dy1 = P2.y - P1.y;

        tan1 = dy1 / dx1;

        if (tan1 == 0) {
            theta1 = 0.0;
            theta2 = 90.0;
            rho1 = (double)P1.y;
        }
        else
        {
            theta1 = atan(tan1);
            tan2 = -1 / tan1;
            theta2 = atan(tan2) * 180 / CV_PI;
            b = (double)P1.y - tan1 * (double)P1.x;
            x1 = -b / (tan1 - tan2);
            y1 = tan1 * (double)P1.x + b;
            rho1 = sqrt((x1 * x1) + (y1 * y1));
        }
    }

    if (theta2 < 0) theta2 = 180.0 + theta2;

    (*rho) = rho1;
    (*theta) = theta2;

}
void roiAroundPoint(Point P, Mat frame, Mat& region, int BoundingX, int BoundingY, Rect& r){
    //LOGI("called roiAroundPoint()");

    int top, left, width, height;

    width = BoundingX;
    height = BoundingY;

    if (P.x < BoundingX) {
        top = 0;
    }
    else {
        if (P.x > (frame.cols - BoundingX)) {
            top = frame.cols - BoundingX;
        }
        else {
            top = P.x - (int)((int)BoundingX / 2.0);
        }
    }


    if (P.y < BoundingY) {
        left = 0;
    }
    else {
        if (P.y > (frame.rows - BoundingY)) {
            left = frame.rows - BoundingX;
        }
        else {
            left = P.y - (int)((int)BoundingY / 2.0);
        }
    }

    r.x = top;
    r.y = left;
    r.width = width;
    r.height = height;

    //printf("r.x = %d r.y = %d r.w = %d r.h = %d",r.x,r.y,r.width,r.height);
    //system("pause");

    region = frame(r);
}

//S
double SkyProb(Mat RegionMask){
    //LOGI("called SkyProb()");

    double mALT = 0; double TOTAL = 0;
    for (int y = 0; y < RegionMask.rows; y++)
        for (int x = 0; x < RegionMask.cols; x++)
        {
            if (RegionMask.at<uchar>(y, x) != 0)
            {
                mALT = mALT + (double)y / (double)RegionMask.rows;
                TOTAL = TOTAL + 1.0;
            }
        }

    return 1 - (mALT / TOTAL);

}
void SetInitialPath(void) {
    //LOGI("called SetInitialPath()");

    FitPath(ImAts);

    itl_iats = ImAts.begin();

    Orientation(ImAts);

    Mat first_frame;
    src_gray_small.copyTo(first_frame);

    itl_iats = ImAts.begin();

    Mat first_frame_lowres;
    int new_cols = (int)(first_frame.cols * LOW_RES_FRAME);
    int new_rows = (int)(first_frame.rows * LOW_RES_FRAME);
    resize(first_frame, first_frame_lowres, cv::Size(new_cols, new_rows));

    Point2f P1, P2, P3;
    P1.x = (float) (itl_iats->MP.x * LOW_RES_FRAME);
    P1.y = (float) (itl_iats->MP.y * LOW_RES_FRAME);

    P2.x = (float) (itl_iats->PR.x * LOW_RES_FRAME);
    P2.y = (float) (itl_iats->PR.y * LOW_RES_FRAME);

    P3.x = (float) (itl_iats->PL.x * LOW_RES_FRAME);
    P3.y = (float) (itl_iats->PL.y * LOW_RES_FRAME);

    Point PL1, PL2, PR1, PR2;
    PL1 = (Point)itl_iats->PL1;
    PL2 = (Point)itl_iats->PL2;
    PR1 = (Point)itl_iats->PR1;
    PR2 = (Point)itl_iats->PR2;

    margLINE margLL, margLR;
    Point Pi, Pf;
    LineExtremePoints(PL1, PL2, &Pi, &Pf, src_gray_small.rows, src_gray_small.cols);
    margLL.margP1 = (Point2f)Pi;
    margLL.margP2 = (Point2f)Pf;
    margLL.valid = 1;
    itl_iats->margLINES.push_back(margLL);

    LineExtremePoints(PR1, PR2, &Pi, &Pf, src_gray_small.rows, src_gray_small.cols);
    margLR.margP1 = (Point2f)Pi;
    margLR.margP2 = (Point2f)Pf;
    margLR.valid = 1;
    itl_iats->margLINES.push_back(margLR);

    old_very_small_frame = first_frame_lowres.clone();

    FitDimentions();
}

//T
void Timer(Mat ParamCameraFrame) {
    //LOGI("called Timer()");

    Mat im1, im2;

    frameSetting(ParamCameraFrame);  // prepara o frame capturado antes de obter deles informaÃ§Ãµes

    src_gray_small.copyTo(im2);

    if (!im2.empty()) {

        //int x, y;
        if (INITIAL_STATUS) {  // calcula todas as informações iniciais

            InitialStreetRecognition(im2);
            SetInitialPath();
            LocalOpticalFlow(ImAts, im2);

            // 555
            MODEL_POS = (Point2f)itl_iats->MP;

            //MODEL_POS.x - MODEL_POS.x + 87; // -Avera_des0.x;
            MODEL_POS.y = MODEL_POS.y + 87; // -Avera_des0.y - 10;

            printf("1: %.3f %.3f\n", MODEL_POS.x, MODEL_POS.y);

            INITIAL_STATUS = 0;
        }
        else { // atualiza novas informaÃ§Ãµes
            newmainlines(im2);
            LocalOpticalFlow(ImAts,im2);

            //rrr
            ModelRegistration();
        }
    }

    // ajusta a imagem colorida para projeÃ§Ã£o
    src_color_small_square.copyTo(im1);  // processa color small square
    makeTextImage(im1);

    POS++;
    if (POS >= LENPATH) POS = 0;
}

//U
//V
//W

//X
void xprojection() {
    //LOGI("called xprojection()");

    PROJECTIONS.at<float>(0, 23) = 0; //  23 + 40
    PROJECTIONS.at<float>(0, 34) = 0; //  34 + 40
    PROJECTIONS.at<float>(0, 39) = 0; //  39 + 35
    PROJECTIONS.at<float>(0, 40) = 0; //  40 + 35
    PROJECTIONS.at<float>(0, 43) = 0; //  43 + 35
    PROJECTIONS.at<float>(0, 61) = 61; //  61 + 18

    PROJECTIONS.at<float>(0, 90) = 0; //  90

    PROJECTIONS.at<float>(0, 108) = 31; // angle = 0
    PROJECTIONS.at<float>(0, 152) = 0; // angle = 0
    PROJECTIONS.at<float>(0, 153) = 0; // angle = 0
    PROJECTIONS.at<float>(0, 154) = 0; // angle = 0
    PROJECTIONS.at<float>(0, 155) = 0; // angle = 0
    PROJECTIONS.at<float>(0, 156) = 0; // anlge = 0
    PROJECTIONS.at<float>(0, 157) = 0; // anlge = 0
}

//Y
void yprojection() {
    //LOGI("called yprojection()");

    PROJECTIONS.at<float>(1, 23) = 73; //  23 + 40
    PROJECTIONS.at<float>(1, 34) = 74; //  34 + 40
    PROJECTIONS.at<float>(1, 39) = 74; //  39 + 35
    PROJECTIONS.at<float>(1, 40) = 75; //  40 + 35
    PROJECTIONS.at<float>(1, 43) = 78; //  43 + 35
    PROJECTIONS.at<float>(1, 61) = 77; //  61 + 16
    PROJECTIONS.at<float>(1, 90) = 90; //  90
    PROJECTIONS.at<float>(1, 140) = 110; // angle - 37
    PROJECTIONS.at<float>(1, 152) = 116; // angle - 37
    PROJECTIONS.at<float>(1, 153) = 116; // angle - 37
    PROJECTIONS.at<float>(1, 154) = 110; // angle - 44
    PROJECTIONS.at<float>(1, 155) = 117; // angle - 39
    PROJECTIONS.at<float>(1, 156) = 116; // anlge - 40
    PROJECTIONS.at<float>(1, 157) = 113; // anlge - 44
}

//Z
void zprojection() {
    //LOGI("called zprojection()");
}

