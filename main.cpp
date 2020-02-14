#include "header.h"

int main(int argc, char**argv)
{
	Mat input_frame;
	int key = 0, contours_size = 0;
	Ptr<Retina> retina_eye;
	vector<vector<Point>> contours;

	try
	{
		cout << "Opening camera..." << endl;
		if (argc == 2)
			VideoCapture capture(argv[0]);
		else
			VideoCapture capture(0);
		cout << "Frame width & height: " << capture.get(CAP_PROP_FRAME_WIDTH) << capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
		cout << "Press 'ESC' to quit" << endl;
		capture >> input_frame;
		init_retina(retina_eye);
		do
			{
				find_motion(retina_eye, &input_frame, &contours);
				for(int i = 0; i < contours_size; i++)
					rectangle(input_frame, boundingRect(contour_ots[i]);, Scalar(0, 0, 255), 2);
				imshow("Motion detector", input_frame);
				key = waitKey(1);
			}
		while (key != 27);
	}
	catch(const Exception& e)
		cerr<<"Error:\n"<<e.what()<<endl;
	destroyAllWindows();
	return 0;
}

void init_retina(Ptr<Retina> Retina_eye)
{
	myRetina = Retina::create(input_frame.size(), false);
	myRetina->write("RetinaDefaultParameters.xml");
	myRetina->setup("RetinaDefaultParameters.xml");
	myRetina->clearBuffers();
}

void find_motion(Ptr<Retina> retina_eye, Mat *input_frame, vector<vector<Point>> *contours)
{
	Mat dst;
	double thresh;
	Mat op_element = getStructuringElement(1, Size(5, 5));
	vector<Vec4i> hierarchy;

	cvtColor(input_frame, dst, COLOR_BGR2GRAY);
	retina_eye->run(dst);
	retina_eye->getMagno(dst);
	GaussianBlur(dst, dst, Size(3, 3), 0, 0);
	threshold(dst, dst, 200, 255,THRESH_BINARY+THRESH_OTSU);
	morphologyEx(dst, dst, MORPH_OPEN, op_element);
	morphologyEx(dst, dst, MORPH_CLOSE, op_element);
	dilate(dst, dst, Mat(),Point(-1, -1), 1);
	GaussianBlur(dst, dst, Size( 3, 3 ), 0, 0);
	thresh = threshold(dst, dst, 0, 255, THRESH_BINARY+THRESH_OTSU);
	Canny(dst, dst, thresh, 0.3*thresh, 3);
	findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
}
