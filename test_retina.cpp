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

Ptr<Retina> myRetina;
// размер буфера для медианного фильтра средней яркости и энтропии
#define CV_MOTION_DETECTOR_MEDIAN_FILTER_N 512

// буферы для фильтров
static float meanBuffer[CV_MOTION_DETECTOR_MEDIAN_FILTER_N];
static float entropyBuffer[CV_MOTION_DETECTOR_MEDIAN_FILTER_N];
// количество кадров
static int numFrame = 0;

// Возвращает медиану массива
float getMedianArrayf(float* data, unsigned long nData);

float calcEntropy(Mat* inputFrame);

// детектор движения
//  inputFrame - входное изображение RGB типа CV_8UC3
// arrayBB - массив ограничительных рамок
void updateMotionDetector(Mat* inputFrame, std::vector<cv::Rect2f>& arrayBB)
{
	cv::Mat retinaOutputMagno; // изображение на выходе magno
	cv::Mat imgTemp; // изображение для порогового преобразования
	float medianEntropy, medianMean; // отфильтрованные значения
	myRetina->run(*inputFrame);
	// загружаем изображение детектора движения
	myRetina->getMagno(retinaOutputMagno);
	// отобразим на экране, если нужно для отладки
	cv::imshow("retinaOutputMagno", retinaOutputMagno);
	// подсчет количества кадров до тех пор, пока их меньше заданного числа
	if (numFrame < CV_MOTION_DETECTOR_MEDIAN_FILTER_N)
	{
		numFrame++;
	}
	// получаем среднее значение яркости всех пикселей
	float mean = cv::mean(retinaOutputMagno)[0];
	// получаем энтропию
	float entropy = calcEntropy(&retinaOutputMagno);
	// фильтруем данные
	if (numFrame >= 2)
	{
		// фильтруем значения энтропии
		// сначала сдвинем буфер значений
		// энтропии и запишем новый элемент
		for (int i = numFrame - 1; i > 0; i--)
		{
			entropyBuffer[i] = entropyBuffer[i - 1];
		}
		entropyBuffer[0] = entropy;
		// фильтруем значения средней яркости
		// сначала сдвинем буфер значений
		// средней яркости и запишем новый элемент
		for (int i = numFrame - 1; i > 0; i--)
		{
			meanBuffer[i] = meanBuffer[i - 1];
		}
		meanBuffer[0] = mean;
		// для фильтрации применим медианный фильтр
		medianEntropy = getMedianArrayf(entropyBuffer, numFrame);
		medianMean = getMedianArrayf(meanBuffer, numFrame);
	}
	else
	{
		medianEntropy = entropy;
		medianMean = mean;
	}
	cout <<"medianEntropy "<<medianEntropy<< " entropy "<< entropy<< " m - e "<< medianEntropy- entropy<<endl;

	// если средняя яркость не очень высокая, то на изображении движение, а не шум
	if (medianMean >= mean)
	// если энтропия меньше медианы, то на изображении движение, а не шум
	//if (medianEntropy  >= entropy)
	{
		cout <<"hi!"<<endl;
		// делаем пороговое преобразование
		// как правило, области с движением достаточно яркие
		// поэтому можно обойтись и без медианы средней яркости
		// cv::threshold(retinaOutputMagno, imgTemp,150, 255.0, CV_THRESH_BINARY);
		// пороговое преобразование с учетом медианы средней яркости
		threshold(retinaOutputMagno, imgTemp,150, 255.0, THRESH_BINARY);
		// найдем контуры
		std::vector<std::vector<Point>> contours;
		findContours(imgTemp, contours, RETR_EXTERNAL,
						 CHAIN_APPROX_SIMPLE);
		if (contours.size() > 0) {
			// если контуры есть
			arrayBB.resize(contours.size());
			// найдем ограничительные рамки
			float xMax, yMax;
			float xMin, yMin;
			for (unsigned long i = 0; i < contours.size(); i++)
			{
				xMax = yMax = 0;
				xMin = yMin = imgTemp.cols;
				for (unsigned long z = 0; z < contours[i].size(); z++)
				{
					if (xMax < contours[i][z].x)
					{
						xMax = contours[i][z].x;
					}
					if (yMax < contours[i][z].y)
					{
						yMax = contours[i][z].y;
					}
					if (xMin > contours[i][z].x)
					{
						xMin = contours[i][z].x;
					}
					if (yMin > contours[i][z].y)
					{
						yMin = contours[i][z].y;
					}
				}
				arrayBB[i].x = xMin;
				arrayBB[i].y = yMin;
				arrayBB[i].width = xMax - xMin ;
				arrayBB[i].height = yMax - yMin;
			}
		}
		else
		{
			arrayBB.clear();
		}
	}
	else
	{
		arrayBB.clear();
	}
	// освободим память
	retinaOutputMagno.release();
	imgTemp.release();
}

// быстрая сортировка массива
template<typename aData>
void quickSort(aData* a, long l, long r) {
	long i = l, j = r;
	aData temp, p;
	p = a[ l + (r - l)/2 ];
	do {
		while ( a[i] < p ) i++;
		while ( a[j] > p ) j--;
		if (i <= j) {
			temp = a[i]; a[i] = a[j]; a[j] = temp;
			i++; j--;
		}
	} while ( i<=j );
	if ( i < r )
		quickSort(a, i, r);
	if ( l < j )
		quickSort(a, l , j);
};

// Возвращает медиану массива
float getMedianArrayf(float* data, unsigned long nData) {
	float medianData;
	float mData[nData];
	register unsigned long i;
	if (nData == 0)
		return 0;
	if (nData == 1) {
		medianData = data[0];
		return medianData;
	}
	for (i = 0; i != nData; ++i) {
		mData[i] = data[i];
	}
	quickSort(mData, 0, nData - 1);
	medianData = mData[nData >> 1];
	return medianData;
};

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
	vector<Rect2f> arrayBB;
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
			myRetina = Retina::create(input_frame.size(), false);
			myRetina->write("RetinaDefaultParameters.xml");
			myRetina->setup("RetinaDefaultParameters.xml");
			myRetina->clearBuffers();
		}
		arrayBB.clear();
		updateMotionDetector(&input_frame,arrayBB);
		for (vector<Rect2f>::iterator it = arrayBB.begin() ; it!=arrayBB.end() ; ++it)
			rectangle(input_frame, *it, Scalar(255,0,0),3);
		imshow("test", input_frame);
		key = waitKey(1);
	}
	while (key != 27);
	destroyAllWindows();
	return 0;
}
