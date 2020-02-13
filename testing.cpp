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

Mat src_gray;
Mat dst, detected_edges;
 /*
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
*/
/// Global variables #6 TestRetina
Ptr<Retina> myRetina;
Ptr<TransientAreasSegmentationModule> TASM;
Mat retinaOutput_magno;
Mat tasmOutput;
Mat op_element;
Mat temp;
Mat bl_retina_dst;
double retina_thresh_ots;
vector<Point> contour_dst;
double len;
double epsilon;
vector<KeyPoint> keypoints;
Mat im_with_keypoints;


Mat tr_retina_ots_dst;
Mat op_retina_ots_dst;
Mat cl_retina_ots_dst;
Mat g_retina_ots_dst;
Mat can_retina_ots_dst;
Mat dil_retina_ots_dst;
vector<vector<Point>> contour_ots;
vector<Vec4i> hierarchy_ots;
Mat draw_ots(Size(640,480), CV_8UC1);
Mat draw_ots_3(Size(640,480), CV_8UC3);
Rect rect_ots;

Mat tr_retina_ad_dst;
Mat op_retina_ad_dst;
Mat cl_retina_ad_dst;
Mat g_retina_ad_dst;
Mat can_retina_ad_dst;
Mat dil_retina_ad_dst;
vector<vector<Point>> contour_ad;
vector<Vec4i> hierarchy_ad;
Mat draw_ad(Size(640,480), CV_8UC1);
Mat draw_ad_3(Size(640,480), CV_8UC3);
Rect rect_ad;

char window_name_frame[] = "Original Frame BLUE: OTSU | RED: adaptive Gaussan\"";
char window_name_magno[] = "Magno canal from Retina";
char window_name_gaus1[] = "Step1 - GaussianBlur before threshold";
char window_name_threshold[] ="Step2 - Left: OTSU threshold | Right: adaptive Gaussan threshold";
char window_name_morphology_o[] ="Step3 - morphology:open Left: OTSU | Right: adaptive Gaussan";
char window_name_morphology_c[] = "Step4 - morphology:close Left: OTSU | Right: adaptive Gaussan";
char window_name_dilate[] = "Step5 - dilate Left: OTSU | Right: adaptive Gaussan";
char window_name_gaus2[] = "Step6 - GaussianBlur Left: OTSU | Right: adaptive Gaussan";
char window_name_canny[] = "Step7 - Canny edge Left: OTSU | Right: adaptive Gaussan";
char window_name_con[] = "Step8 - Contours Left: OTSU | Right: adaptive Gaussan";
char window_name_rect[] = "Step9 - Rects Left: OTSU | Right: adaptive Gaussan";

 /*
char window_name5[] = "TestRetina";

/// Function headers
void Threshold_Demo( int, void* );
void Dilation( int, void* );
void testAlgoRetina(int, void*);
void adaptiveThreshold_Demo(int, void*);*/
float calcEntropy(Mat* inputFrame)
 {
	/// Establish the number of bins
	int histSize = 256;
	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 } ;
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	 MatND hist;
	/// Compute the histograms:
	calcHist(inputFrame, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
	hist /= inputFrame->total();
	hist += 1e-4; //prevent 0

	Mat logP;
	log(hist,logP);

	return(-1*sum(hist.mul(logP)).val[0]);
}
int main(int, char**)
{
	Mat input_frame;
	int key = 0, tmp = 1;
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
				//resize(input_frame,input_frame,Size(320,240));
				if (input_frame.empty())
				{
					cerr << "ERROR: Can't grab camera input_frame." << endl;
					break;
				}
				else if (tmp)
				{
					tmp = 0;
					myRetina = Retina::create(input_frame.size(), false);
					TASM = TransientAreasSegmentationModule::create(input_frame.size());
					myRetina->write("RetinaDefaultParameters.xml");
					TASM->write("TASMDefaultParameters.xml");
					myRetina->setup("RetinaDefaultParameters.xml");
					TASM->setup("TASMDefaultParameters.xml");
					myRetina->clearBuffers();
					//TASM->clearAllBuffers();
					op_element = getStructuringElement( 1, Size( 5, 5 ));
					/*
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
					namedWindow( window_name4, WINDOW_AUTOSIZE );*/
				}
				//input_frame.convertTo(input_frame, CV_8UC3);
				//retinaOutput_magno.convertTo(retinaOutput_magno, CV_8UC3);
				cvtColor(input_frame, src_gray, COLOR_BGR2GRAY );
				myRetina->run(src_gray);
				myRetina->getMagno(retinaOutput_magno);

				//TASM->run(retinaOutput_magno);
				//TASM->getSegmentationPicture(tasmOutput);

				GaussianBlur(retinaOutput_magno, bl_retina_dst, Size( 3, 3 ), 0, 0 );
				threshold(bl_retina_dst,tr_retina_ots_dst,200,255,THRESH_BINARY+THRESH_OTSU);
				//adaptiveThreshold(bl_retina_dst, tr_retina_ad_dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV,7,1);

				morphologyEx(tr_retina_ots_dst,op_retina_ots_dst, MORPH_OPEN,op_element);
				//morphologyEx(tr_retina_ad_dst,op_retina_ad_dst, MORPH_OPEN,op_element);

				morphologyEx(op_retina_ots_dst,cl_retina_ots_dst, MORPH_CLOSE,op_element);
				//morphologyEx(op_retina_ad_dst,cl_retina_ad_dst, MORPH_CLOSE,op_element);

				dilate(cl_retina_ots_dst, dil_retina_ots_dst, Mat(),Point(-1, -1), 20);
				//dilate(cl_retina_ad_dst, dil_retina_ad_dst, op_element);
				//dilate(cl_retina_ad_dst,dil_retina_ad_dst, Mat(),Point(-1, -1), 7);

				GaussianBlur(dil_retina_ots_dst, g_retina_ots_dst, Size( 3, 3 ), 0, 0);
				//GaussianBlur(dil_retina_ad_dst, g_retina_ad_dst, Size( 3, 3 ), 0, 0);

				retina_thresh_ots = threshold(g_retina_ots_dst, temp,255,255,THRESH_BINARY+THRESH_OTSU);
				Canny(g_retina_ots_dst, can_retina_ots_dst, retina_thresh_ots,0.3*retina_thresh_ots,3);
				//retina_thresh_ots = threshold(g_retina_ad_dst, temp,0,255,THRESH_BINARY+THRESH_OTSU);
				//Canny(g_retina_ad_dst, can_retina_ad_dst, retina_thresh_ots,0.5*retina_thresh_ots,3);

				findContours(can_retina_ots_dst, contour_ots, hierarchy_ots, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
				//findContours(can_retina_ad_dst, contour_ad, hierarchy_ad, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

				draw_ots = Mat::zeros(can_retina_ots_dst.size(), CV_8UC1);
				//cout<< "depth "<< draw_ots.depth()<< " channels "<<draw_ots.channels()<<endl;
				for(int i = 0; i < contour_ots.size(); i++)
				{
					//if (contourArea(contour_ots[i]) < 5)
						//continue;
					//convexHull(contour_ots[i],contour_dst);
					//contour_ots[i] = contour_dst;
					//len = arcLength(contour_ots[i], true);
					//epsilon = 0.01*len;
					//approxPolyDP(contour_ots[i], contour_ots[i], epsilon, true);
					rect_ots = boundingRect(contour_ots[i]);
					rectangle(input_frame, rect_ots, Scalar(255,0,0),3);
					//drawContours(draw_ots, contour_ots, i, Scalar(255, 255, 255),2, LINE_8, hierarchy_ots, 0);
				}
/*
				findContours(draw_ots, contour_ots, hierarchy_ots, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
				draw_ots = Mat::zeros(can_retina_ots_dst.size(), CV_8UC1);
				//cout<< "depth "<< draw_ots.depth()<< " channels "<<draw_ots.channels()<<endl;
				for(int i = 0; i < contour_ots.size(); i++)
				{
					if (contourArea(contour_ots[i]) < 5)
						continue;
					//convexHull(contour_ots[i],contour_dst);
					//contour_ots[i] = contour_dst;
					//len = arcLength(contour_ots[i], true);
					//epsilon = 0.01*len;
					//approxPolyDP(contour_ots[i], contour_ots[i], epsilon, true);
					rect_ots = boundingRect(contour_ots[i]);
					rectangle(draw_ots, rect_ots, Scalar(0,0,255),3);
					//drawContours(draw_ots, contour_ots, i, Scalar(255, 255, 255),2, LINE_8, hierarchy_ots, 0);
				}*/
				/*draw_ad = Mat::zeros(can_retina_ad_dst.size(), CV_8UC1 );
				for(int i = 0; i < contour_ad.size(); i++)
				{
					//cout<<"Area: "<<contourArea(contour_ad[i])<< endl;

					if (contourArea(contour_ad[i]) < 5)
						continue;
					//convexHull(contour_ad[i],contour_dst);
					//contour_ad[i] = contour_dst;
					//len = arcLength(contour_ad[i], true);
					//epsilon = 0.01*len;
					//approxPolyDP(contour_ad[i], contour_ad[i], epsilon, true);
					drawContours(draw_ad, contour_ad, i, Scalar(255, 255, 255), 2, LINE_8, hierarchy_ad, 0);
				}*/

				//rect_ots = boundingRect(draw_ots);
				//rect_ad = boundingRect(draw_ad);

				//rectangle(input_frame, rect_ots, Scalar(255,0,0),1);
				//rectangle(input_frame, rect_ad, Scalar(0,0,255),1);


				//imshow("tasm", tasmOutput);
				imshow(window_name_frame, input_frame);
				imshow(window_name_magno, retinaOutput_magno);
			//	imshow(window_name_gaus1, bl_retina_dst);

				//hconcat(tr_retina_ots_dst, tr_retina_ad_dst,temp);
				imshow(window_name_threshold, temp);

				//hconcat(op_retina_ots_dst, op_retina_ad_dst,temp);
			//	imshow(window_name_morphology_o, op_retina_ots_dst);

				//hconcat(cl_retina_ots_dst, cl_retina_ad_dst, temp);
				//imshow(window_name_morphology_c, cl_retina_ots_dst);

				//hconcat(dil_retina_ots_dst, dil_retina_ad_dst, temp);
				imshow(window_name_dilate, dil_retina_ots_dst);

				//hconcat(g_retina_ots_dst, g_retina_ad_dst, temp);
				//imshow(window_name_gaus2, g_retina_ots_dst);

				//hconcat(can_retina_ots_dst, can_retina_ad_dst, temp);
				imshow(window_name_canny, can_retina_ots_dst);

				//hconcat(draw_ots, draw_ad,temp);
				imshow(window_name_con, draw_ots);

				/*
				///****THRE
				/// Convert the image to grayscale
				cvtColor( input_frame, src_gray, COLOR_BGR2GRAY );
				thresh = threshold( src_gray, dst, threshold_value, max_BINARY_value,threshold_type );
				//imshow( window_name2, dst );

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
				//imshow( window_name3, dilation_dst );

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
				imshow( window_name4, drawing );*/

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
