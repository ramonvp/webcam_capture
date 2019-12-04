
//opencv
#include "opencv2/opencv.hpp"

//std
#include <iostream>
#include <cstdlib>


enum { CV_BLUE, CV_GREEN, CV_RED};

enum { NO_FILTER = 0, SWAP_BG, GRAYSCALE, NEGATIVE, MAX_FILTERS};

static int current_filter = 0;
static int MARGIN = 64;
static const int INC = 16;

void toggle_filter()
{
    current_filter = (current_filter+1)%MAX_FILTERS;
}

void increase_margin(int max)
{
  if (MARGIN + INC < max)
    MARGIN += INC;
}

void decrease_margin()
{
  if (MARGIN > INC)
    MARGIN -= INC;
}

void apply_filter(cv::Mat & image)
{
    const int y = image.rows/2;
    const int x = image.cols/2;

    for(int j = 0; j < MARGIN; j++)
    {
        for(int i = 0; i < MARGIN; i++)
        {
            cv::Vec3b intensity = image.at<cv::Vec3b>(y+j-MARGIN/2, x+i-MARGIN/2);
            uchar temp;

            switch(current_filter)
            {
            case SWAP_BG:
                temp = intensity.val[CV_BLUE];
                intensity.val[CV_BLUE] = intensity.val[CV_GREEN];
                intensity.val[CV_GREEN] = temp;
                intensity.val[CV_RED] = 0;
                break;
            case GRAYSCALE:
                temp = (intensity.val[CV_BLUE] + intensity.val[CV_GREEN] + intensity.val[CV_RED])/3;
                intensity.val[CV_BLUE] = temp;
                intensity.val[CV_GREEN] = temp;
                intensity.val[CV_RED] = temp;
                break;
            case NEGATIVE:
                intensity.val[CV_BLUE] = 255 - intensity.val[CV_BLUE];
                intensity.val[CV_GREEN] = 255 - intensity.val[CV_GREEN];
                intensity.val[CV_RED] = 255 - intensity.val[CV_RED] ;
            default:
                // do nothing;
                break;
            }

            image.at<cv::Vec3b>(y+j-MARGIN/2, x+i-MARGIN/2) = intensity;
        }
    }
}

//main
int main(int argc, char *argv[])
{
    cv::VideoCapture camera; //OpenCV video capture object
    cv::Mat image; //OpenCV image object
    int cam_id; //camera id . Associated to device number in /dev/videoX
    int user_key; //user pressed key to quit

    //check user args
    switch(argc)
    {
      case 1: //no argument provided, so try /dev/video0
        cam_id = 0;
        break;
      case 2: //an argument is provided. Get it and set cam_id
        cam_id = atoi(argv[1]);
        break;
      default:
        std::cout << "Invalid number of arguments. Call program as: webcam_capture [video_device_id]. " << std::endl;
        std::cout << "EXIT program." << std::endl;
        break;
    }

    //advertising to the user
    std::cout << "Opening video device " << cam_id << std::endl;

    //open the video stream and make sure it's opened
    if( !camera.open(cam_id) )
    {
        std::cout << "Error opening the camera. May be invalid device id. EXIT program." << std::endl;
        return -1;
    }

    //capture loop. Out of user press a key
    while(1)
    {
        //Read image and check it. Blocking call up to a new image arrives from camera.
        if(!camera.read(image))
        {
            std::cout << "No frame" << std::endl;
            cv::waitKey();
        }

        if( current_filter )
        {
            apply_filter(image);
        }

        //show image in a window
        cv::imshow("Output Window", image);

        // Waits 30 millisecond to check if a key has been pressed.
        // if 'q' key has been pressed, break the loop and exit.
        // Of 't' key has been pressed, toggle the filter selection
        // Otherwise continues.
        user_key = cv::waitKey(30);
        if( user_key == -1 ) continue;
        unsigned char key_pressed = (unsigned char)(user_key & 0xff);
        if (key_pressed == 'q' ) break;
        if (key_pressed == 't' ) toggle_filter();
        if (key_pressed == '+' ) increase_margin( std::min(image.rows, image.cols));
        if (key_pressed == '-' ) decrease_margin();
    }

    camera.release();
}
