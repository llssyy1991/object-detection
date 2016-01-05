#include <sstream>
#include <string>
#include <iostream>
#include <opencv\highgui.h>
#include <opencv\cv.h>

using namespace cv;

int H_MIN =0;//152
int H_MAX = 255;//198
int S_MIN = 0;//142
int S_MAX = 255;//255
int V_MIN = 0;//75
int V_MAX = 255;//255

const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

const int MAX_NUM_OBJECTS = 50;

const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;

const string windowName = "mirror,mirror,show my face";
const string windowName1 = "show here";
const string windowName2 = "Thresholded";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
void on_trackbar(int, void*)
{}

string intToString(int number){


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void motion_detection(Mat prev, Mat present, Mat next)
{


}



void createTrackbars(){



	namedWindow(trackbarWindowName, WINDOW_NORMAL);

	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);


}
void drawObject(int x, int y, Mat &frame){


	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);

}
void morphOps(Mat &thresh){


	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));

	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}
void trackFilteredObject(int &max_x, int &min_x,int &max_y,int &min_y, Mat threshold, Mat &cameraFeed){

	Mat temp;
	threshold.copyTo(temp);

	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;

	
	//4-th parament changed
	findContours(temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);


	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		Scalar color(255, 0, 0);
		//drawContours(cameraFeed, contours, 0, color, 1, 8, hierarchy);
		if (numObjects < MAX_NUM_OBJECTS)
		{
			for (int i = 0; i < hierarchy.size(); i++)
			{
				int x_max = 0, x_min = 2000;
				int y_max = 0, y_min = 2000;
				for (int j = 0; j < contours.at(i).size(); j++)
				{

					if (x_max < contours.at(i).at(j).x) x_max = contours.at(i).at(j).x;
					if (x_min > contours.at(i).at(j).x) x_min = contours.at(i).at(j).x;
					if (y_max < contours.at(i).at(j).y) y_max = contours.at(i).at(j).y;
					if (y_min > contours.at(i).at(j).y) y_min = contours.at(i).at(j).y;
				}
				
				if (x_max<=max_x && x_min>=min_x && y_max<=max_y && y_min>=min_y)
				{
					max_x = x_max;
					min_x = x_min;
					max_y = y_max;
					min_y = y_min;
				}


			}
		}

		/*
		if (numObjects<MAX_NUM_OBJECTS){
		for (int index = 0; index >= 0; index = hierarchy[index][0]) {

		Moments moment = moments((cv::Mat)contours[index]);
		double area = moment.m00;




		if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea){
		x = moment.m10 / area;
		y = moment.m01 / area;
		objectFound = true;
		refArea = area;
		}
		else objectFound = false;


		}

		if (objectFound == true){
		putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);

		drawObject(x, y, cameraFeed);
		}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);*/
	}
}
int main(int argc, char* argv[])
{

	bool trackObjects = true;
	bool useMorphOps = true;
	int x_start = 0, x_end = 640;
	int y_start = 0, y_end = 480;
	Scalar color = Scalar(0, 255, 0);


	namedWindow(windowName, WINDOW_NORMAL);
	namedWindow(windowName1, WINDOW_NORMAL);
	namedWindow(windowName2, WINDOW_NORMAL);

	Mat prev_frame, current_frame, next_frame;
	Mat prev_HSV, current_HSV, next_HSV;
	Mat prev_threshold, current_threshold, next_threshold;
	Mat prev_grey, current_grey, next_grey;
	Mat result;
	Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2, 2));


	Mat HSV;
	Mat d1, d2;

	//Mat threshold;
	Mat motion;

	int x = 0, y = 0;

	createTrackbars();

	VideoCapture capture;

	capture.open(0);

	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	while (capture.read(prev_frame) == false)
	{
		continue;
	}
	capture.read(current_frame);
	capture.read(next_frame);
	while (1){
		
		current_frame.copyTo(prev_frame);
		next_frame.copyTo(current_frame);
		current_frame.copyTo(result);
		capture.read(next_frame);

		
		cvtColor(prev_frame, prev_grey, CV_RGB2GRAY);
		cvtColor(current_frame, current_grey, CV_RGB2GRAY);
		cvtColor(next_frame, next_grey, CV_RGB2GRAY);
		
		absdiff(prev_grey, next_grey, d1);
		absdiff(current_grey, next_grey, d2);
		imshow(windowName3, d1);
		bitwise_and(d1, d2, motion);
		threshold(motion, motion, 25, 255, CV_THRESH_BINARY);
		erode(motion, motion, kernel_ero);
		int min_x = 0, max_x = 640;
		int min_y = 0, max_y = 480;
		int number_of_changes = 0;
		
		for (int i = x_start; i < x_end; i++)
		{
			for (int j = y_start; j < y_end; j++)
			{
				if ((int)motion.at<uchar>(j, i) == 255)
				{
					number_of_changes++;
					if (min_x>i) min_x = i;
					if (max_x<i) max_x = i;
					if (min_y>j) min_y = j;
					if (max_y<j) max_y = j;
				}

			}
		}
		if (number_of_changes){
			//check if not out of bounds
			if (min_x - 10 > 0) min_x -= 10;
			if (min_y - 10 > 0) min_y -= 10;
			if (max_x + 10 < result.cols - 1) max_x += 10;
			if (max_y + 10 < result.rows - 1) max_y += 10;
			// draw rectangle round the changed pixel
			Point x(min_x, min_y);
			Point y(max_x, max_y);
			Rect rect(x, y);
			//Mat cropped = result(rect);
			//cropped.copyTo(result_cropped);
			rectangle(result, rect, color, 1);
			imshow(windowName, result);
		}

		

		

		cvtColor(prev_frame, prev_HSV, COLOR_BGR2HSV);
		cvtColor(current_frame, current_HSV, COLOR_BGR2HSV);
		cvtColor(next_frame, next_HSV, COLOR_BGR2HSV);
		//inRange(prev_HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), prev_threshold);
		inRange(current_HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), current_threshold);
		morphOps(current_threshold);
		//inRange(next_HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), next_threshold);
		imshow(windowName2, current_threshold);
		trackFilteredObject(max_x, min_x, max_y, min_y, current_threshold, current_frame);

		Point lefttop(min_x, min_y), rightbuttom(max_x, max_y);
		Rect target(lefttop, rightbuttom);
		rectangle(result, target, Scalar(255, 0, 0));
		
		imshow(windowName1, result);


		waitKey(60);
		

	}






	return 0;
}
