#include <stdio.h>
#include "opencv2/opencv.hpp"
// C++ code:

using namespace cv;

int main(){
    VideoCapture cap(0); // open the default camera

 // check if we succeeded
    if(!cap.isOpened()){
        printf("Could not open capture device\n");
        return -1;
    }

    Mat edges;

    namedWindow("edges",1);

    for(;;){

        Mat frame, small;

        cap >> frame; // get a new frame from camera

        resize(frame, frame, Size(128, 64));
        cvtColor(frame, frame, CV_BGR2GRAY);

        GaussianBlur(frame, frame, Size(7,7), 1.5, 1.5);

        // adaptiveThreshold(frame, frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 10);

        threshold(frame, frame, 100, 255, THRESH_OTSU);


        // Canny(frame, frame, 0, 30, 3);

        imshow("edges", frame);

        if(waitKey(30) >= 0) break;

        // the camera will be deinitialized automatically in VideoCapture destructor
    }

    return 0;
}