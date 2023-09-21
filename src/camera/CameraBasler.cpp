#include "CameraBasler.h"
#include <cstring>

vector<CameraInfo> CameraBasler::getCameraList(){
    vector<CameraInfo> ret;
//    Pylon::DeviceInfoList_t cameraList;

//    Pylon::CTlFactory::GetInstance().EnumerateDevices(cameraList, true);


//    for (unsigned int i=0; i < cameraList.size(); i++){
//        CameraInfo camera;
//        camera.busID = i;
//        camera.model = cameraList[i].GetFriendlyName();
//        camera.vendor = cameraList[i].GetVendorName();

//        ret.push_back(camera);
//    }
    CameraInfo camera;
    camera.busID = 12345;
    camera.model = "192245";
    camera.vendor = "Basler";
    ret.push_back(camera);

    return ret;
}

CameraBasler::CameraBasler(unsigned int camNum, CameraTriggerMode triggerMode) : Camera(triggerMode) {
    Pylon::PylonInitialize();

    cam = new Pylon::CInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());

    cam->Open();

    if (cam_config.empty()) {
        CFeaturePersistence::SaveToString(cam_config, &cam->GetNodeMap());
    }
}
