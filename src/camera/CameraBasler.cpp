#include "CameraBasler.h"
#include <cstring>
#include <QDebug>

vector<CameraInfo> CameraBasler::getCameraList(){
    // NOTE: Because initialized in constructor
    PylonInitialize();

    Pylon::DeviceInfoList_t cameraList;

    Pylon::CTlFactory::GetInstance().EnumerateDevices(cameraList, true);

    vector<CameraInfo> ret;
    for (unsigned int i=0; i < cameraList.size(); i++){
        CameraInfo camera;
        camera.busID = i;
        camera.model = cameraList[i].GetFriendlyName();
        camera.vendor = cameraList[i].GetVendorName();

        ret.push_back(camera);
    }

    return ret;
}

CameraBasler::CameraBasler(unsigned int camNum, CameraTriggerMode triggerMode) : Camera(triggerMode) {
   PylonInitialize();

   try {
       cam = new Pylon::CInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
       cam -> MaxNumBuffer=5;
       if (!isOpen()) {
           cam->Open();
       }
       qDebug() << "Using device " << cam->GetDeviceInfo().GetModelName() << endl;
   } catch (GenICam::GenericException &e) {
       cam = nullptr;
       qWarning() << "Camera Error: " << e.GetDescription();
   }
}

CameraSettings CameraBasler::getCameraSettings(){
    if (cam->IsGrabbing()) cam->StopGrabbing();

    CameraSettings settings;

//    settings.shutter = cam->ExposureTime.GetValue();

//    settings.gain = cam->GainRaw.GetValue();

    return settings;
}

void CameraBasler::setCameraSettings(CameraSettings settings){
    PylonInitialize();
    if(capturing){
        std::cerr << "CameraBasler: already capturing!" << std::endl;
        return;
    }
    try {
        return;
        //if (cam->IsGrabbing()) cam->StopGrabbing();
        // Get a camera parameter using generic parameter access.
//        Pylon::CIntegerParameter ExposureTime( cam->GetNodeMap(), "ExposureTime" );
//        ExposureTime.SetValue(settings.shutter);

    } catch (GenICam::GenericException &e) {
        cam = nullptr;
        qWarning() << "Camera Error: " << e.GetDescription();
    };
//    cam->GainRaw.SetValue(settings.gain);

//    cam->ExposureTime.SetValue(settings.shutter);
}

void CameraBasler::startCapture(){
   PylonInitialize();
   if(capturing){
       std::cerr << "CameraBasler: already capturing!" << std::endl;
       return;
   }

   cam->StartGrabbing(GrabStrategy_LatestImageOnly);

   Pylon::CPylonImage pylonFrame;
   Pylon::CGrabResultPtr ptrGrabResult;
   cam->ExecuteSoftwareTrigger();
   cam->RetrieveResult(500, ptrGrabResult, TimeoutHandling_ThrowException);

   if (ptrGrabResult->GrabSucceeded()){
       frameWidth = ptrGrabResult->GetWidth();
       frameHeight = ptrGrabResult->GetHeight();
   }
   capturing = true;
}

void CameraBasler::stopCapture(){
    if (cam->IsGrabbing())
        cam->StopGrabbing();
    if (isOpen()) {
        cam->Close();
    }
}

CameraFrame CameraBasler::getFrame(){
   PylonInitialize();
   CameraFrame frame;

   if (!capturing) {
       std::cerr << "ERROR: Cannot get frame before capturing." << std::endl;
       return frame;
   }
   Pylon::CPylonImage pylonFrame;
   Pylon::CGrabResultPtr ptrGrabResult;

   cam->ExecuteSoftwareTrigger();
   cam->RetrieveResult(1000, ptrGrabResult, TimeoutHandling_ThrowException);

   if (ptrGrabResult->GrabSucceeded()){
       frame.height = ptrGrabResult->GetHeight();
       frame.width = ptrGrabResult->GetWidth();
       frame.memory = (unsigned char*)ptrGrabResult->GetBuffer();
   }
   return frame;
}

bool CameraBasler::isOpen() const
{
    return cam != nullptr && cam->IsOpen();
}

size_t CameraBasler::getFrameSizeBytes(){
    if (!capturing) {
        std::cerr << "ERROR: Cannot get frame size before capturing." << std::endl;
        return 0;
    }
    return frameWidth*frameHeight;
}

size_t CameraBasler::getFrameWidth(){
    if (!capturing) {
        std::cerr << "ERROR: Cannot get frame width before capturing." << std::endl;
        return 0;
    }
    return frameWidth;
}

size_t CameraBasler::getFrameHeight(){
    if (!capturing) {
        std::cerr << "ERROR: Cannot get frame height before capturing." << std::endl;
        return 0;
    }

    return frameHeight;
}

CameraBasler::~CameraBasler(){
    // Stop camera transmission
    if(capturing)
        stopCapture();
    if (isOpen()) {
        cam->Close();
    }
    cam->DestroyDevice();
    delete cam;
    cam = nullptr;
    PylonTerminate();
}
