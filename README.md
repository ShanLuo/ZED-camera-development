# ZED-camera-development

This project is using ZED stereo camera to get the depths and 3D coordinates of objects using OpenCV. The code is based on the ZED SDK 2.1, STEREO LABS.

#### Main functions
- Display left and right images and the depth map of the ZED stereo camera.
- On the left image, click the insterest pixels, the corresponding
  depths and 3D coordinates will be shown in the termial.
- You can also save the data with keyboard shortcuts.

#### Build for Linux

Open a terminal in the sample directory and execute the following command:

    mkdir build
    cd build
    cmake ..
    make

## Run the program

- Navigate to the build directory and launch the executable file
- Or open a terminal in the build directory and run the sample :

        ./ZED\ with\ OpenCV [path to SVO file]

You can optionally provide an SVO file path (recorded stereo video of the ZED).

### Keyboard shortcuts

This table lists keyboard shortcuts that you can use in the sample application.

Parameter             | Description                   |   Hotkey
---------------------|------------------------------------|-------------------------------------------------
Save Side by Side      | Save side by side image.       |   's'                             
Save Depth             | Save depth image.              |   'p'                              
Save Point Cloud       | Save 3D point cloud.        |   'd'
Switch cloud format    | Toggle between point cloud formats.    |   'm'
Switch depth format    | Toggle between depth image formats. |   'n'                                                      
Exit         | Quit the application.             | 'q'
