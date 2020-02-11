#include <opencv2/bioinspired/bioinspired.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace bioinspired;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;


Mat input_frame, src_gray;
Mat dst, detected_edges;

/// Global variables #1 CANNY
Mat dst_edge;
int ratio = 3;
int kernel_size = 3;
char window_name[] = "Edge Map";

/// Global variables #2 THRE
double thresh;
int threshold_value = 0;
int threshold_type = 3;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
//Mat src, src_gray, dst;
char window_name2[] = "Threshold Demo";
char trackbar_type[] = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char trackbar_value[] = "Value";

/// Global variables #3 DIL

Mat  dilation_dst;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;
char window_name3[] = "Dilation Demo";

/// Global variables #4 CONT
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

/// Global variables #5 DRAW
char window_name4[] = "Contours";
double  test;
/// Function headers
void Threshold_Demo( int, void* );
void Dilation( int, void* );

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
					tmp = 0;
					///****INIT RETINA
					myRetina = Retina::create(input_frame.size(), false);
					// save default retina parameters file
					myRetina->write("RetinaDefaultParameters.xml");
					myRetina->setup("RetinaDefaultParameters.xml");
					myRetina->clearBuffers();

					///****THRE
					/// Create a window to display results
					namedWindow( window_name2, WINDOW_AUTOSIZE);
					/// Create Trackbar to choose type of Threshold
					createTrackbar(trackbar_type,
								   window_name2, &threshold_type,
								   max_type, Threshold_Demo);
					createTrackbar(trackbar_value,
								   window_name2, &threshold_value,
								   max_value, Threshold_Demo);

					///****INIT DIL
					/// Create windows
					namedWindow(  window_name3, WINDOW_AUTOSIZE );
					/// Create Dilation Trackbar
					createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse",  window_name3,
									&dilation_elem, max_elem,
									Dilation );
					createTrackbar( "Kernel size:\n 2n +1",  window_name3,
									&dilation_size, max_kernel_size,
									Dilation );

					///****INIT CANNY
					/// Create a window
					namedWindow( window_name, WINDOW_AUTOSIZE );
					///****DRAW
					namedWindow( window_name4, WINDOW_AUTOSIZE );
				}
				///****FRAME
				imshow("Frame", input_frame);
				///****RETINA
				// run retina filter on the loaded input frame
				myRetina->run(input_frame);
				// Retrieve and display retina output
				myRetina->getMagno(retinaOutput_magno);
				imshow("Retina Magno", retinaOutput_magno);

				///****THRE
				/// Convert the image to grayscale
				cvtColor( input_frame, src_gray, COLOR_BGR2GRAY );
				thresh = threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );
				imshow( window_name2, dst );

				///****DIL
				int dilation_type;
				if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
				else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
				else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
				Mat element = getStructuringElement( dilation_type,
													 Size( 2*dilation_size + 1, 2*dilation_size+1 ),
													 Point( dilation_size, dilation_size ) );
				/// Apply the dilation operation
				dilate(dst, dilation_dst, element);
				imshow( window_name3, dilation_dst );

				///****CANNY
				/// Show the image
				/// Reduce noise with a kernel 3x3
				blur(dilation_dst, detected_edges, Size(3,3) );
				/// Canny detector
				Canny( detected_edges, detected_edges,  thresh,  thresh*ratio, kernel_size );
				/// Using Canny's output as a mask, we display our result
				/// Create a matrix of the same type and size as src (for dst)
				dst_edge.create( dilation_dst.size(), dilation_dst.type() );
				dst_edge = Scalar::all(0);
				input_frame.copyTo( dst_edge, detected_edges);
				imshow( window_name, dst_edge );

				///****DRAW
				/// Find contours
				findContours( detected_edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
				/// Draw contours
				Mat drawing = Mat::zeros( detected_edges.size(), CV_8UC3 );
				for( int i = 0; i< contours.size(); i++ )
				{
					if (contourArea(contours[i]) < 5)
						continue;
					drawContours( drawing, contours, i, Scalar( 255, 255, 255 ), 2);
				}
				/// Show in a window
				imshow( window_name4, drawing );

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
/**
 * @function Threshold_Demo
 */
void Threshold_Demo( int, void* )
{
	/* 0: Binary
	   1: Binary Inverted
	   2: Threshold Truncated
	   3: Threshold to Zero
	   4: Threshold to Zero Inverted
	 */

	threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );

	imshow( window_name2, dst );
}
/** @function Dilation */
void Dilation( int, void* )
{
	int dilation_type;
	if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
	else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
	else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement( dilation_type,
										 Size( 2*dilation_size + 1, 2*dilation_size+1 ),
										 Point( dilation_size, dilation_size ) );
	/// Apply the dilation operation
	dilate(dst, dilation_dst, element);
	imshow( window_name3, dilation_dst );
}