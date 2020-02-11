#include <opencv2/bioinspired/bioinspired.hpp>
//#include <opencv2/bioinspired/retina.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace bioinspired;
using std::cout;
using std::cerr;
using std::endl;

int main(int, char**)
{
	Mat input_frame;
	int key = 0;
	int tmp = 1;
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
				}
				// run retina filter on the loaded input frame
				myRetina->run(input_frame);
				// Retrieve and display retina output
				myRetina->getParvo(retinaOutput_parvo);
				myRetina->getMagno(retinaOutput_magno);
				imshow("Retina Parvo", retinaOutput_parvo);
				imshow("Retina Magno", retinaOutput_magno);
				imshow("Frame", input_frame);
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