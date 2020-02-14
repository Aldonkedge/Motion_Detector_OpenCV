#ifndef SAURONS_EYE_HEADER_H
#define SAURONS_EYE_HEADER_H

#include <opencv2/bioinspired/bioinspired.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace bioinspired;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

void init_retina(Ptr<Retina> Retina_eye);
void find_motion(Ptr<Retina> retina_eye, Mat *input_frame, vector<vector<Point>> *contours);

#endif //SAURONS_EYE_HEADER_H
