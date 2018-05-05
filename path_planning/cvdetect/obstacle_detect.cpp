#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"

#include <stdio.h>
#include <math.h>


using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );

/** Global variables */
String cascade_name;
CascadeClassifier cascade;
String window_name = "Capture - Obstacle Detection";

/** @function main */
int main( int argc, const char** argv )
{
    CommandLineParser parser(argc, argv,
        "{help h||}"
        "{cascade|cascade_xml/cascade.xml|}");

    cascade_name = parser.get<String>("cascade");
    VideoCapture capture;
    Mat frame;

    //-- 1. Load the cascades
    if( !cascade.load( cascade_name ) ){ printf("--(!)Error loading cascade\n"); return -1; };

    //-- 2. Read the video stream
    capture.open( "source/videos/bin_1.wmv" );

    if ( ! capture.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }

    while ( capture.read(frame) )
    {
        if( frame.empty() )
        {
            printf(" --(!) No captured frame -- Break!");
            break;
        }

        //-- 3. Apply the classifier to the frame
        detectAndDisplay( frame );

        if( waitKey(25) == 27 ) { break; } // escape
    }
    return 0;
}

int calculateDist (int v, Mat image) {
    double pi = 3.1415926535897;
    double alpha = 8.0 * alpha/180;
    int u0 = 200;
    int ay = 500;

    double wid = 18
    double d;

    d = wid/tan(alpha + atan((v-u0)/ay));
    
    return d;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame )
{
    std::vector<Rect> obst;
    Mat frame_gray;
    Mat res_gray;

    float angle;

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    resize(frame_gray, res_gray, Size(320, 240))
    // equalizeHist( frame_gray, frame_gray );

    //-- Detect obst
    cascade.detectMultiScale( res_gray, obst, 1.1, 5, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    for ( size_t i = 0; i < obst.size(); i++ )
    {
        rectangle(frame, obst[i].x, obst[i].y, obst[i].x+obst[i].width, obst[i].y+obst[i].height, Scalar(255,0,255),2  )
        Point center( obst[i].x + obst[i].width/2, obst[i].y + obst[i].height/2 );

        angle = (center.x - 160)/float(320)

        d = calculateDist(obst[i].x + obst[i].width, res_gray)

        if d > 0:
            putText(image, "%.1fin" % d, Point (frame.size().width - 300, frame.size().height[0] - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

        printf("Detected object at (%d, %d). It is %d in away at angle of %d and width %d px.", center.x, center.y, dist, angle, obst[i].width );

    }
    //-- Show what you got
    imshow( window_name, frame );
}