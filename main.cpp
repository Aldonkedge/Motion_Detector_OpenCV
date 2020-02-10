#include <opencv2/bioinspired.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using std::cout;
using std::cerr;
using std::endl;

int main(int, char**)
{
	Mat input_frame;
	int key;

	cout << "Opening camera..." << endl;
	VideoCapture capture(0);
	if (!capture.isOpened())
	{
		cerr << "ERROR: Can't initialize camera capture" << endl;
		return 1;
	}

	cout << "Frame width: " << capture.get(CAP_PROP_FRAME_WIDTH) << endl;
	cout << "     height: " << capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
	cout << endl << "Press 'ESC' to quit" << endl;
	for (;;)
	{
		capture >> input_frame;
		if (input_frame.empty())
		{
			cerr << "ERROR: Can't grab camera input_frame." << endl;
			break;
		}
			imshow("Frame", input_frame);
		key = waitKey(1);
		if (key == 27/*ESC*/)
			break;
	}

	return 0;
}
