///////////////////////////////////////////////////////////////////////////
//
// This code is developed by Dr. Shan Luo, based on code from STEREOLABS.
//
//Copyright (c) 2017, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/***********************************************************************************************
 ** This sample demonstrates how to use the ZED SDK with OpenCV. 					  	      **
 ** Depth and images are captured with the ZED SDK, converted to OpenCV format and displayed. **
 ***********************************************************************************************/

#include <sl/Camera.hpp>
#include <SaveDepth.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

using namespace sl;
using namespace std;
using namespace cv;

cv::Mat slMat2cvMat(sl::Mat& input);
void printHelp();
void onMouse(int event, int i, int j, int flags,  void* );

cv::Mat img;
sl::Mat point_cloud;
sl::Mat depthMat;

int main(int argc, char **argv) {

    if (argc != 2) {
        cout << "Only the path of the input SVO file should be passed as argument.\n";
        return 1;
    }

    // Create a ZED camera object
    Camera zed;

    // Set configuration parameters
    InitParameters init_params;
    init_params.svo_input_filename.set(argv[1]);
    //init_params.camera_resolution = RESOLUTION_HD1080;
    //init_params.depth_mode = DEPTH_MODE_PERFORMANCE;
    //init_params.coordinate_units = UNIT_METER;

    // Open the camera
    ERROR_CODE err = zed.open(init_params);
    if (err != SUCCESS) {
        printf("%s\n", errorCode2str(err).c_str());
        zed.close();
        return 1; // Quit if an error occurred
    }

    // Display help in console
    printHelp();

    // Set runtime parameters after opening the camera
    RuntimeParameters runtime_parameters;
    runtime_parameters.sensing_mode = SENSING_MODE_STANDARD;

    // Prepare new image size to retrieve half-resolution images
    Resolution image_size = zed.getResolution();
    int new_width = image_size.width / 2 ;
    int new_height = image_size.height / 2 ;
    //std::cout << " image width " << new_width << std::endl;
    //std::cout << " image height " << new_height << std::endl;

    // To share data between sl::Mat and cv::Mat, use slMat2cvMat()
    // Only the headers and pointer to the sl::Mat are copied, not the data itself
    sl::Mat left_zed(new_width, new_height, sl::MAT_TYPE_8U_C4);
    cv::Mat left_ocv = slMat2cvMat(left_zed);
    sl::Mat right_zed(new_width, new_height, sl::MAT_TYPE_8U_C4);
    cv::Mat right_ocv = slMat2cvMat(right_zed);
    sl::Mat depth_image_zed(new_width, new_height, sl::MAT_TYPE_8U_C4);
    cv::Mat depth_image_ocv = slMat2cvMat(depth_image_zed);

    // Loop until 'q' is pressed
    char key = ' ';
    while (key != 'q') {

        if (zed.grab(runtime_parameters) == SUCCESS) {

            // Retrieve the left image, right image, depth image and depth values in set-resolution
            zed.retrieveImage(left_zed, VIEW_LEFT, MEM_CPU, new_width, new_height);
            zed.retrieveImage(right_zed, VIEW_RIGHT, MEM_CPU, new_width, new_height);
            zed.retrieveImage(depth_image_zed, VIEW_DEPTH, MEM_CPU, new_width, new_height);
            zed.retrieveMeasure(depthMat, MEASURE_DEPTH, MEM_CPU, new_width, new_height);

            // Retrieve the RGBA point cloud in half-resolution
            // To learn how to manipulate and display point clouds, see Depth Sensing sample
            zed.retrieveMeasure(point_cloud, MEASURE_XYZRGBA, MEM_CPU, new_width, new_height);

            // Display left image and call MouseCallback
            namedWindow("Left image");
            setMouseCallback( "Left image", onMouse, 0 );
            img = left_ocv;
            imshow("Left image",img);

            // Display image and depth using cv:Mat which share sl:Mvat data
            cv::imshow("Right image", right_ocv);
            cv::imshow("Depth image", depth_image_ocv);

            //Display depth in different colormap
            double min;
            double max;
            cv::Mat colorDisplay;
            cv::minMaxIdx(depth_image_ocv, &min, &max);
            cv::convertScaleAbs(depth_image_ocv, colorDisplay, 255 / max);  
            applyColorMap(colorDisplay, colorDisplay, cv::COLORMAP_AUTUMN);
            imshow("Colored depth image", colorDisplay);

            

            // Handle key event
            key = cv::waitKey(10);
            processKeyEvent(zed, key);
        }
    }
    zed.close();
    return 0;
}

/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::Mat slMat2cvMat(sl::Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
        case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
        case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
        case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
        case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
        case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
        case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
        case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
        case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}

/**
* This function displays help in console
**/
void printHelp() {
    std::cout << " Press 's' to save Side by side images" << std::endl;
    std::cout << " Press 'p' to save Point Cloud" << std::endl;
    std::cout << " Press 'd' to save Depth image" << std::endl;
    std::cout << " Press 'm' to switch Point Cloud format" << std::endl;
    std::cout << " Press 'n' to switch Depth format" << std::endl;
}

void onMouse(int event, int i, int j, int flags, void* )
{
    if( event != CV_EVENT_LBUTTONDOWN )
            return;

    cv::Point pt = cv::Point(i,j);
    float depth_value;
    depthMat.getValue(i,j,&depth_value);
    sl::float4 point3D;
    // Get the 3D point cloud values for pixel (i,j)
    point_cloud.getValue(i,j,&point3D);
    float x = point3D.x;
    float y = point3D.y;
    float z = point3D.z;
    //float color = point3D.w;
    
    std::cout<<"x img col="<<pt.x<<"\ty img col="<<pt.y<<"\tdepth in mm="<<depth_value<< "\n";
    std::cout<<"coordinates in space= "<< x << ", " << y << ", " << z << "\n";

}
