//
// Created by joao_vitor on 24/01/2020.
//

#include "Config.h"

bool CompE(const ImageAtributs A, const ImageAtributs B) {
    return A.Entropy < B.Entropy;
}

/** area comparison **/
bool CompA(const ImageAtributs A, const ImageAtributs B) {
    return A.Area < B.Area;
}

/** street comparison **/
bool CompS(const ImageAtributs A, const ImageAtributs B) {
    return A.PStreet > B.PStreet;
}

/** funcao de comparacao decrecente de area de contornos **/
bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2) {
    double i = fabs(contourArea(cv::Mat(contour1)));
    double j = fabs(contourArea(cv::Mat(contour2)));
    return (i > j);
}

Mat frame;
Mat src_color;
Mat src_color_small;
Mat src_color_small_square;
Mat src_gray;
Mat src_gray_small;
Mat src_gray_small_square;
Mat seg;
Mat seg_small;
Mat seg_small_square;
Mat main_contours;
Mat image;
Mat main_street_small;
Mat main_street;
Mat per_frame;
Mat web_frame;
Mat new_frame, old_frame, old_gray, new_gray;
Mat old_reg_p1, old_reg_p2, old_reg_p3, old_reg_p4;
Mat old_reg_pc;
Mat PLmask_old, PRmask_old;
Mat PLmask_new, PRmask_new;
Mat PROJECTIONS = Mat::zeros(3,180,CV_32F);
Mat mask = Mat::zeros(226, 400, CV_8UC3);
Mat entrMap; //= -Mat::ones(src_gray_small.rows, src_gray_small.cols, CV_32F);
Mat old_very_small_frame;
Mat mask_optflow;

vector<vector<Point>> contours;
vector<Vec4i> hierarchy;
Vec4i l;

vector<vector<Point>> hull(1000);
vector<vector<Point>> hull2(1000);

std::set <SegLineANDLenth, CompL> MainLinSegs;
std::set <SegLineANDLenth, CompL>::iterator itl_mls;

std::vector <ImageAtributs> ImAts;
std::vector <ImageAtributs>::iterator itl_iats;

vector<Vec4i> MargLines;
vector<Vec4i> lines;

vector<Point2f> good_new_pl;
vector<Point2f> ERL_old;
vector<Point2f> ELL_old;
vector<Point2f> good_new_pr;
vector<Point2f> p0_l1,  p0_l2,  p0_r1,  p0_r2;
vector<Point2f> p0_pc;

vector<Point> approxContour;

vector<Scalar> colors;

RNG rng;

vector<uchar> status_pr;
vector<uchar> status_pl;
vector<float> err_pr;
vector<float> err_pl;

Point2f MODEL_POS(0.0f, 0.0f);
Point2f p0_l1_, p0_l2_, p0_r1_, p0_r2_;
Point2f Avera_des_pc_geral;
Point2f Avera_des0;
Point2f Avera_des1;
Point2f Avera_des_pc0;
Point2f Avera_des_pc1;

time_t time0;   // create timers.
time_t time1;
time_t time2;

bool INITIAL_STATUS = 1;
const bool OPTICALFLOWVIEW = 0;

double range = 0.2;
double DESL_X, DESL_Y;
double xc1 = 0;
double xc2 = 0;
double yc1 = 0;
double yc2 = 0;
double X0, Y0, XA, YA, XB, YB, XC, YC;
double LENGTH_X;
double LENGTH_Y;
double tanD, angD, bD;
double PATH_Y[ALT];
double PATH_X[LAR];
double SIZE_OBJ = 10;
double OBJ_VAR[180];
double tempo = 0;
double Total_des_pc0;
double Total_des_pc1;
double Total_desl;

int LENPATH = 0;
int POS = -1;
int fm = 0;
int MAX_NLINES = 0;

GLint DX = -100, DY = -100, DZ = 0;

GLfloat ORTHO_LEN[400];
GLfloat PROJ[3] = {0,0,0};
GLfloat Rz = 0;
GLfloat angle = 0, fAspect = 0, rot = 0;