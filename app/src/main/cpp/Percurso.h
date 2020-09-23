//
// Created by joao_vitor on 24/01/2020.
//

#ifndef GLES3JNI_PERCURSO_H
#define GLES3JNI_PERCURSO_H

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/core/core.hpp>
#include <GLES3/gl3.h>
#include <math.h>
#include <set>
#include "Config.h"

extern double getMainAngle0To1802(Point P1, Point P2);
extern void PathDatails(int Xini, int Yini, int Xfin, int Yfin, double tan);
extern void FitMainPath(std::vector <ImageAtributs>& ImAts);
extern void FitRightPath(std::vector <ImageAtributs>& ImAts);
extern void FitLeftPath(std::vector <ImageAtributs>& ImAts);
extern void LineExtremePoints(Point P1, Point P2, Point* Pi, Point* Pf, int rows, int cols);
extern void Orientation(std::vector <ImageAtributs> ImAts);
extern void FitDimentions();
extern void FitPath(std::vector <ImageAtributs>& ImAts);

#endif //GLES3JNI_PERCURSO_H
