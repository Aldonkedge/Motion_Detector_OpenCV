#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <stdlib.h>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	// open the first webcam plugged in the computer
	VideoCapture web_camera(0);
	// this will contain the image from the webcam
	Mat prev_wc_frame;

	char key;

	if (!web_camera.isOpened())
	{
		cerr << "ERROR: Could not open web_camera" << endl;
		return 1;
	}

	// create a window to display the images from the webcam
	namedWindow("Saurons eye", WINDOW_AUTOSIZE);



	// display the frame until you press a key
	while (1)
	{
		// capture the  frame from the webcam
		web_camera >> prev_wc_frame;
		// show the image on the window
		imshow("Saurons eye", prev_wc_frame);

		key = waitKey(10);

		if (key == 'q' || key == 'Q')
			break;
	}
	return 0;
}