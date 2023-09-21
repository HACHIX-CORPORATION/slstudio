#include "CameraBasler.h"
#include <cstring>

vector<CameraInfo> CameraBasler::getCameraList(){
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


