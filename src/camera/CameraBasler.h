#ifndef CAMERABASLER_H
#define CAMERABASLER_H

#include "Camera.h"
#include <pylon/PylonIncludes.h>
//// Namespace for using pylon objects.
using namespace Pylon;
//// Settings for using Basler USB cameras.
//#include <pylon/usb/BaslerUsbInstantCamera.h>

using namespace std;

class CameraBasler : public Camera {
    public:
        // Static methods
        static vector<CameraInfo> getCameraList();
        // Interface function
        CameraBasler(unsigned int camNum, CameraTriggerMode triggerMode);
        CameraSettings getCameraSettings();
        void setCameraSettings(CameraSettings);
        void startCapture();
        void stopCapture();
        CameraFrame getFrame();
        size_t getFrameSizeBytes();
        size_t getFrameWidth();
        size_t getFrameHeight();
        ~CameraBasler();
    private:
        Pylon::CInstantCamera *cam;
        Pylon::String_t cam_config;
        size_t frameWidth, frameHeight;
};
#endif // CAMERABASLER_H
