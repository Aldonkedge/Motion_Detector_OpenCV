#include "header.h"

static int error_handler(const char *message)
{
	cerr << "ERROR: " << message << endl;
	return (1);
}

static int init_retina(Ptr<Retina> *retina_eye,
					   Mat *input_frame,
					   VideoCapture *capture)
{
	(*retina_eye) = Retina::create(IM_SIZE, false);
	(*retina_eye)->write("RetinaDefaultParameters.xml");
	(*retina_eye)->setup("RetinaDefaultParameters.xml");
	(*retina_eye)->clearBuffers();
	// Даём нашему модулю немного "прогреться", пропуская первые 25 кадров
	for(int i = 0; i< 25; i++)
	{
		(*capture) >> (*input_frame);
		if (input_frame->empty())
			return (error_handler("Can't grab camera input_frame."));
		resize(*input_frame, *input_frame, IM_SIZE);
		(*retina_eye)->run(*input_frame);
	}
	return (0);
}

int main(int argc, char *argv[])
{
	VideoCapture capture;
	Mat input_frame;
	Ptr<Retina> retina_eye;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	int key = -2;

	cout << "Opening camera..." << endl;
	if (argc == 2)
		capture.open(argv[1]);
	else
		capture.open(0);
	if (!capture.isOpened())
		return (error_handler("Can't initialize camera capture."));
	cout << "Frame width & height: " << capture.get(CAP_PROP_FRAME_WIDTH) << "&"
		<< capture.get(CAP_PROP_FRAME_HEIGHT) << endl;
	cout << "Press 'ESC' to quit" << endl;
	if (init_retina(&retina_eye, &input_frame, &capture))
		return (error_handler("Can't initialize camera capture."));
	try
	{
		while (key != 27)
			{
				capture >> input_frame;
				if (input_frame.empty())
					break;
				resize(input_frame, input_frame, IM_SIZE);
				find_motion(&retina_eye, &input_frame, &contours, &hierarchy);
				drawing(&input_frame, &contours, &hierarchy);
				imshow("Motion detector", input_frame);
				key = waitKey(1);
			}
	}
	catch(const Exception& e)
	{
		error_handler(e.what());
	}
	capture.release();
	destroyAllWindows();
	return (0);
}

void drawing(Mat *input_frame,
			 vector<vector<Point>> *contours,
			 vector<Vec4i> *hierarchy)
{
	int contours_size = contours->size();

	for(int i = 0; i < contours_size; i++)
	{
		if ((*hierarchy)[i][3] == -1 && contourArea((*contours)[i]) > 10)
			rectangle(*input_frame, boundingRect((*contours)[i]), Scalar(0, 0, 255), 1);
	}
	contours->erase(contours->begin(), contours->end());
}

void find_motion(Ptr<Retina> *retina_eye,
				Mat *input_frame,
				vector<vector<Point>> *contours,
				vector<Vec4i> *hierarchy)
{
	Mat dst, draw;
	double thresh;
	Mat op_element = getStructuringElement(1, Size(5, 5));
	cvtColor(*input_frame, dst, COLOR_BGR2GRAY);
	(*retina_eye)->run(dst);
	(*retina_eye)->getMagno(dst);
	GaussianBlur(dst, dst, Size(21, 21), 0, 0);
	thresh = threshold(dst, dst, 0, 255, THRESH_BINARY + THRESH_OTSU);
	morphologyEx(dst, dst, MORPH_OPEN, op_element);
	morphologyEx(dst, dst, MORPH_CLOSE, op_element);
	dilate(dst, dst, Mat(), Point(-1, -1), 4);
	Canny(dst, dst, thresh, thresh * 3, 3);
	findContours(dst, *contours, *hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
}
