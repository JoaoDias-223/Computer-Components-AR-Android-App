//
// Created by joao_vitor on 24/01/2020.
//

#ifndef GLES3JNI_CONFIG_H
#define GLES3JNI_CONFIG_H

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
#include <set>

#define LAR 400
#define ALT 400
#define PI 3.14159265
#define RHOS 284
#define THETAS 91
#define MAIN_ANGLE_DIR MainAngleCP
#define DT 1000
#define DXT 0
#define DYT 0
#define AROUND_EXT_TRACK 10
#define LOW_RES_AROUND_LINE 3
#define LOW_RES_FRAME 1.0

using namespace std;
using namespace cv;

extern Mat frame;
extern Mat src_color;
extern Mat src_color_small;
extern Mat src_color_small_square;
extern Mat src_gray;
extern Mat src_gray_small;
extern Mat src_gray_small_square;
extern Mat seg;
extern Mat seg_small;
extern Mat seg_small_square;
extern Mat main_contours;
extern Mat image;
extern Mat main_street_small;
extern Mat main_street;
extern Mat per_frame;
extern Mat web_frame;
extern Mat new_frame, old_frame, old_gray, new_gray;
extern Mat old_reg_p1, old_reg_p2, old_reg_p3, old_reg_p4;
extern Mat old_reg_pc;
extern Mat PLmask_old, PRmask_old;
extern Mat PLmask_new, PRmask_new;
extern Mat PROJECTIONS;
extern Mat mask;
extern Mat entrMap; //= -Mat::ones(src_gray_small.rows, src_gray_small.cols, CV_32F);
extern Mat old_very_small_frame;
extern Mat mask_optflow;

static GLubyte checkImage[LAR][ALT][4];
static GLubyte MTextImg[LAR][ALT][4];
static GLuint texName;

extern vector<vector<Point>> contours;
extern vector<Vec4i> hierarchy;
extern Vec4i l;

extern vector<vector<Point>> hull;
extern vector<vector<Point>> hull2;

class SegLineANDLenth {
public:
    vector<Point> SegLine;
    double LENLINE;
};

class CompL {
public:
    bool operator()(SegLineANDLenth L1, SegLineANDLenth L2) const
    {
        if (L1.LENLINE > L2.LENLINE)
            return true;
        else     return false;
    };
};

struct margLINE
{
    Point2f margP1;
    Point2f margP2;
    bool valid;
};

struct ImageAtributs {
public:

    Mat RegionMask;  // region defined by a binary mask
    Mat RegionHull;  // region defined by a convex-hull mask

    double Area;    // regio area
    double Entropy; // q-convolutional entropy of the candidate region
    double PStreet; // probability of this region be the required street
    double Pfocus;  // focus probability around this region
    double PERM;    // entropy probability of expandend region hull
    double PCP;     // central point probability, acounts how much CP is far from image center

    vector<margLINE> margLINES; // the four extreme points of the two main hull arests (marginal Lines) as their extreme points:
    // (P1,P2) = (AE[0], AE[1]), left arest; (P3,P4) = (AE[2], AE[3]), right arest

    //vector<Point2f> dirLINE;  // direction line
    Point2f PD1;  // P1 of direction line
    Point2f PD2;

    Point2f PL1;
    Point2f PL2;
    bool validl;

    Point2f PR1;
    Point2f PR2;
    bool validr;

    // these family of points manage the directions
    Point MP; // main point (focus) of the two main hulls arests
    Point CP; // central point of expansed region
    Point PF; // final point of path from MP
    Point PL; // final point of left from P1-P2 in AE attribute above
    Point PR; // final point of right from P3-P4 in AE attrubute above

    double MainAngle; // main angle of the line (median) from MP to PF
    double MainAngleCP; // main angle of the line from MP to CP

    int indx;  // index of candidata initial region

};

extern bool CompE(const ImageAtributs A, const ImageAtributs B);

/** area comparison **/
extern bool CompA(const ImageAtributs A, const ImageAtributs B);

/** street comparison **/
extern bool CompS(const ImageAtributs A, const ImageAtributs B);

/** funcao de comparacao decrecente de area de contornos **/
extern bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2);

extern std::set <SegLineANDLenth, CompL> MainLinSegs;
extern std::set <SegLineANDLenth, CompL>::iterator itl_mls;

extern std::vector <ImageAtributs> ImAts;
extern std::vector <ImageAtributs>::iterator itl_iats;

extern vector<Vec4i> MargLines;
extern vector<Vec4i> lines;

extern vector<Point2f> good_new_pl;
extern vector<Point2f> ERL_old;
extern vector<Point2f> ELL_old;
extern vector<Point2f> good_new_pr;
extern vector<Point2f> p0_l1,  p0_l2,  p0_r1,  p0_r2;
extern vector<Point2f> p0_pc;

extern vector<Point> approxContour;

extern vector<Scalar> colors;

extern RNG rng;

extern vector<uchar> status_pr;
extern vector<uchar> status_pl;
extern vector<float> err_pr;
extern vector<float> err_pl;

extern Point2f MODEL_POS;
extern Point2f p0_l1_, p0_l2_, p0_r1_, p0_r2_;
extern Point2f Avera_des_pc_geral;
extern Point2f Avera_des0;
extern Point2f Avera_des1;
extern Point2f Avera_des_pc0;
extern Point2f Avera_des_pc1;

extern time_t time0;   // create timers.
extern time_t time1;
extern time_t time2;

extern bool INITIAL_STATUS;
extern const bool OPTICALFLOWVIEW;

extern double range;
extern double DESL_X, DESL_Y;
extern double xc1;
extern double xc2;
extern double yc1;
extern double yc2;
extern double X0, Y0, XA, YA, XB, YB, XC, YC;
extern double LENGTH_X;
extern double LENGTH_Y;
extern double tanD, angD, bD;
extern double  PATH_Y[ALT];
extern double  PATH_X[LAR];
extern double SIZE_OBJ;
extern double OBJ_VAR[180];
extern double tempo;
extern double Total_des_pc0;
extern double Total_des_pc1;
extern double Total_desl;

extern int LENPATH;
extern int POS;
extern int fm;
extern int MAX_NLINES;

extern GLint DX, DY, DZ;

extern GLfloat ORTHO_LEN[400];
extern GLfloat PROJ[3];
extern GLfloat Rz;
extern GLfloat angle, fAspect, rot;

#endif //GLES3JNI_CONFIG_H
