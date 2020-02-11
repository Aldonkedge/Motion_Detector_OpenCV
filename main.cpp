#include <opencv2/bioinspired/bioinspired.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace bioinspired;
using std::cout;
using std::cerr;
using std::endl;

/// Global variables

Mat input_frame, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char window_name[] = "Edge Map";

int main(int, char**)
{
	int key = 0, tmp = 1;
	// create a retina instance with default parameters setup, uncomment the initialisation you wanna test
	Ptr<Retina> myRetina;
	// declare retina output buffers
	Mat retinaOutput_parvo;
	Mat retinaOutput_magno;



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
	try {
		do
			{
				capture >> input_frame;
				if (input_frame.empty())
				{
					cerr << "ERROR: Can't grab camera input_frame." << endl;
					break;
				}
				else if (tmp)
				{
					myRetina = cv::bioinspired::Retina::create(input_frame.size());
					// save default retina parameters file
					myRetina->write("RetinaDefaultParameters.xml");
					tmp = 0;
					/// Create a matrix of the same type and size as src (for dst)
					dst.create( input_frame.size(), input_frame.type() );
					/// Create a window
					namedWindow( window_name, WINDOW_AUTOSIZE );
					/// Create a Trackbar for user to enter threshold
					createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold);

				}
				/// Convert the image to grayscale
				cvtColor( input_frame, src_gray, COLOR_BGR2GRAY );
				// run retina filter on the loaded input frame
				myRetina->run(input_frame);
				// Retrieve and display retina output
				myRetina->getParvo(retinaOutput_parvo);
				myRetina->getMagno(retinaOutput_magno);
				imshow("Retina Parvo", retinaOutput_parvo);
				imshow("Retina Magno", retinaOutput_magno);
				imshow("Frame", input_frame);
				/// Show the image
				/// Reduce noise with a kernel 3x3
				blur( src_gray, detected_edges, Size(3,3) );

				/// Canny detector
				Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

				/// Using Canny's output as a mask, we display our result
				dst = Scalar::all(0);

				input_frame.copyTo( dst, detected_edges);
				imshow( window_name, dst );
				key = waitKey(1);
			}
		while (key != 27);
	}
	catch(const Exception& e)
	{
		cerr<<"Error using Retina or end of video sequence reached : "<<e.what()<<endl;
	}
	destroyAllWindows();
	return 0;
}