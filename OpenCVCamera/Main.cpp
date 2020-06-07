#include <conio.h>


#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

// Buffers
Mat frame;
Mat grey;
Mat edges;
Mat threshold_output;
vector<vector<Point>> contours;

// Variables
int thresh = 100;



//TODO: stabilization filter (use center from minRect data struct)


int main(int, char**)
{
	// Open the default camera
	VideoCapture cap(0);
	// Check if we succeeded
	if (!cap.isOpened())
	{
		_getch();
		return -1;
	}

	// Windows
	namedWindow("frame", 1);
	namedWindow("edges", 1);
	namedWindow("Contours", WINDOW_AUTOSIZE);

	// GUI elements
	createTrackbar(" Threshold:", "Contours", &thresh, 255);

	// Main loop
	while (true)
	{
		// Get a new frame from camera
		cap >> frame;

		// Convert to greyscale
		cvtColor(frame, edges, COLOR_BGR2GRAY);

		// Apply blur
		GaussianBlur(edges, edges, Size(7, 7), 1.5, 1.5);

		// Contours detection
		Canny(edges, edges, 0, 30, 3);


		RotatedRect rRect = RotatedRect(Point2f(100, 100), Size2f(100, 50), 30);




		imshow("frame", frame);
		imshow("edges", edges);


		// Detect edges using Threshold
		threshold(frame, threshold_output, thresh, 255, THRESH_BINARY);

		// Find contours
		findContours(edges, contours, RETR_TREE, CHAIN_APPROX_SIMPLE/*, Point(0, 0)*/);

		// Find the rotated rectangles and ellipses for each contour
		vector<RotatedRect> minRect(contours.size());
		vector<RotatedRect> minEllipse(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			minRect[i] = minAreaRect(Mat(contours[i]));
			if (contours[i].size() > 5)
			{
				minEllipse[i] = fitEllipse(Mat(contours[i]));
			}
		}

		// Draw contours + rotated rects + ellipses
		Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
		// Magic number lol
		int sizeFilter = 80;
		for (int i = 0; i < contours.size(); i++)
		{
			if (minRect[i].size.width > sizeFilter && minRect[i].size.height > sizeFilter)
			{
				Scalar color = Scalar(0, 255, 0);
				// contour
				drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
				// ellipse
				ellipse(drawing, minEllipse[i], color, 2, 8);
				// rotated rectangle
				Point2f rect_points[4]; minRect[i].points(rect_points);
				for (int j = 0; j < 4; j++)
				{
					line(drawing, rect_points[j], rect_points[(j + 1) % 4], color, 1, 8);
				}
			}
		}

		// Show in a window
		imshow("Contours", drawing);

		std::cout << thresh << std::endl;

		if (waitKey(30) >= 0) break;
	}
	// The camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}