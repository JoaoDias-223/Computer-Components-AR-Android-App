//
// Created by joao_vitor on 24/01/2020.
//

#ifndef GLES3JNI_PATHFINDER_H
#define GLES3JNI_PATHFINDER_H

#include "Config.h"
#include "Percurso.h"

#include <math.h>
#include <vector>

//A
double ABSDist(Point2f P1, Point2f P2, Point2f P3, Point2f P4);

//B

//C
void CannySegmentation(Mat src_gray0, Mat* dst_bw);
void compareFramesForNewOrientation(Mat old_very_small_frame, Mat new_very_small_frame);
void computeDirectionLINE();
void computeDirectionLINE2();
void computeEL(Mat new_frame, Point2f P1, Point2f P2, vector<Point2f>& EL);

//D
//void DesenhaTest(void);
//void DesenhaComTextura(void);
//void drawObjet();

//E
double entropySceneDirProb(Mat HL, Mat HR);

//F
double fitAngle(double DirX, double DirY);
double fitAngle0To180(double tang1, double tang2);
void frameSetting(Mat ParamCameraFrame);
void fitMaskAroundP(Mat frame, Mat& Pmask, Point P);

//G
double getAngle(Vec4i P);
double getMainAngle0To180(Point P1, Point P2);
double getqEntropy(Mat src_gray_small_2, Mat mask);
double getqEntropyMaskOptmizer(Mat src_gray_small_2, Mat mask);
void getDirections(Mat main_street);
void getSobelGradient(Mat src0, Mat* dst0);
void getqEntropyImage(Mat src, Mat* dst0);

//H
void HoughSegmentation(Mat src_gray0, Mat& dst_bw);

//I
//int InitilizeApplication();
//void InicializaTextura(void);
void InicializaRecognitionFromWebCam(Mat ParamCameraFrame);
void InicializaRAStreet();
void InitialStreetRecognition(Mat im0);
//void InicializaLuz(void);

//J
//K

//L
void LocalOpticalFlow(std::vector <ImageAtributs>& ImAts, Mat new_frame);

//M
void ModelRegistration(void);
void makeTextImage(Mat im0);

//N
void newmainlines(Mat new_frame);
void newMarginalLines(vector<Point> approxContourL, std::vector <ImageAtributs>& ImAts);

//O

//P
double PolarDist(Point2f P1, Point2f P2, Point2f P3, Point2f P4);
double ProbSceneDirBasedOnLine(vector<Point> splitLine);
void psrFocusDetectionBasedOnFeatureExpansion(Mat src_gray_small, Mat reg_bw_small, std::vector<Point> approxContourL, ImageAtributs* ImAl);
void psrSquareImage(Mat src0, Mat* dst2);
void psrGradSobelqEntropySegment(Mat src_gray, Mat* dst_seg);
void psrSobelSegmentation(Mat src0, Mat* dst2);

//Q

//R
void roiAroundPoint(Point P, Mat frame, Mat& region, int BoundingX, int BoundingY, Rect& r);
void RhoTheta(Point2f P1, Point2f P2, double* rho, double* theta);

//S
double SkyProb(Mat RegionMask);
void SetInitialPath(void);

//T
void Timer(Mat ParamCameraFrame);

//U
//V
//W

//X
void xprojection();

//Y
void yprojection();

//Z
void zprojection();

#endif //GLES3JNI_PATHFINDER_H
