#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Image", WINDOW_AUTOSIZE );

    cvtColor(image, image, CV_BGR2GRAY);

    // threshold(image, image, 100, 255, THRESH_OTSU);

    resize(image, image, Size(128, 64));


    imshow("Display Image", image);

    waitKey(0);

    return 0;
}