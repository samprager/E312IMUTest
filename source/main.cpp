#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <streambuf>
#include <vector>
#include <csignal>
#include <sys/types.h>
#include <sys/time.h>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include "RTIMULib.h"

#define IMU_LOG_INTERVAL 1000 // imu log interval in microseconds

RTIMU *_imu;
RTIMU_DATA _imuData;

int imu_init(){
  // These aren't used here, but were probably included in an example from RTIMULIb...
  // I'll keep them around for now in case they provide hints moving forward
  int sampleCount = 0;
  int sampleRate = 0;
  uint64_t rateTimer;
  uint64_t displayTimer;
  uint64_t now;

  //  Using RTIMULib here allows it to use the .ini file generated by RTIMULibDemo.
  //  Or, you can create the .ini in some other directory by using:
  //      RTIMUSettings *settings = new RTIMUSettings("<directory path>", "RTIMULib");
  //  where <directory path> is the path to where the .ini file is to be loaded/saved

  // RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
  RTIMUSettings *settings = new RTIMUSettings("/etc","RTIMULib");


  _imu = RTIMU::createIMU(settings);

  if ((_imu == NULL) || (_imu->IMUType() == RTIMU_TYPE_NULL)) {
      std::cerr<<"No IMU found\n";
      return 1;
  }
  //  This is an opportunity to manually override any settings before the call IMUInit

  //  set up IMU

  _imu->IMUInit();

  //  this is a convenient place to change fusion parameters
  _imu->setSlerpPower(0.02);
  _imu->setGyroEnable(true);
  _imu->setAccelEnable(true);
  _imu->setCompassEnable(true);

  _imu->IMURead();
  _imuData = _imu->getIMUData();

  return 0;
}

std::string get_imu_data(const std::string &type){
    std::stringstream imuss;
    if (type=="fusion"){
      imuss << boost::format("(roll:%+09.4f,pitch:%+08.4f,yaw:%+09.4f)") % (RTMATH_RAD_TO_DEGREE*_imuData.fusionPose.x()) % (RTMATH_RAD_TO_DEGREE*_imuData.fusionPose.y()) % (RTMATH_RAD_TO_DEGREE*_imuData.fusionPose.z());
    }
    else if (type =="accel"){
      imuss << boost::format("(%+08.4f,%+08.4f,%+08.4f)") % (10*_imuData.accel.x())%(10*_imuData.accel.y())%(10*_imuData.accel.z());
    }
    else if (type == "gyro"){
      imuss << boost::format("(%+09.4f,%+09.4f,%+09.4f)") % (RTMATH_RAD_TO_DEGREE*_imuData.gyro.x()) % (RTMATH_RAD_TO_DEGREE*_imuData.gyro.y())%(RTMATH_RAD_TO_DEGREE*_imuData.gyro.z());
    }
    else if (type == "compass"){
      imuss << boost::format("(%+08.4f,%+08.4f,%+08.4f)") % (_imuData.compass.x())%(_imuData.compass.y())%(_imuData.compass.z());
    }
    else if (type == "all") {
        imuss << boost::format("(%+09.4f,%+08.4f,%+09.4f)") % (RTMATH_RAD_TO_DEGREE*_imuData.fusionPose.x()) % (RTMATH_RAD_TO_DEGREE*_imuData.fusionPose.y()) % (RTMATH_RAD_TO_DEGREE*_imuData.fusionPose.z());
        imuss << boost::format(" : (%+08.4f,%+08.4f,%+08.4f)") % (10*_imuData.accel.x())%(10*_imuData.accel.y())%(10*_imuData.accel.z());
        imuss << boost::format(" : (%+09.4f,%+09.4f,%+09.4f)") % (RTMATH_RAD_TO_DEGREE*_imuData.gyro.x()) % (RTMATH_RAD_TO_DEGREE*_imuData.gyro.y())%(RTMATH_RAD_TO_DEGREE*_imuData.gyro.z());
        imuss << boost::format(" : (%+08.4f,%+08.4f,%+08.4f)") % (_imuData.compass.x())%(_imuData.compass.y())%(_imuData.compass.z());
    }
    else{
        imuss<<"[get_imu_data] Error: Unrecognized option";
    }
    return imuss.str();
}

int main(int argc, char *argv[])
{
    boost::filesystem::path out_path;
    const char * home = getenv ("HOME");
    if (home == NULL) {
      out_path = boost::filesystem::system_complete(argv[0]).parent_path().parent_path().parent_path();
      std::cerr << "error: Env HOME variable not set. Using " <<out_path.string()<< std::endl;
     //  throw std::invalid_argument ("error: HOME environment variable not set.");
    }
    else{
        out_path = boost::filesystem::path(std::string(home)) / boost::filesystem::path(".E312IMUTest");
        if (!boost::filesystem::exists(out_path)) boost::filesystem::create_directory(out_path);
    }

    // set global extern variables (declared in global.hpp)
    int err = imu_init();

    bool exitloop = false;
    bool imu_log = true;
    uint64_t now, logtimer;
    boost::filesystem::path imu_logfile = out_path / boost::filesystem::path("imu_data.log");
    std::ofstream outfile(imu_logfile.c_str(),std::ios_base::out | std::ios_base::app);
    outfile <<"\nTimestamp : Vita Time : Fusion(roll,pitch,yaw) : Accel(x,y,z) : Gyro(x,y,z) : Compass(x,y,z)\n"<<std::endl;
    logtimer = 0;//RTMath::currentUSecsSinceEpoch();
    while (!exitloop){
      usleep(_imu->IMUGetPollInterval() * 1000); // this should be in usec? check to confirm IMUGetPollInterval() return value
      while (_imu->IMURead()) {
        _imuData = _imu->getIMUData();
        now = RTMath::currentUSecsSinceEpoch();
        if ((now-logtimer)>IMU_LOG_INTERVAL){
            if(imu_log){
              std::stringstream ss;
              std::string timestamp = to_iso_string(boost::posix_time::microsec_clock::local_time());
              ss << timestamp<<" : "<<get_imu_data("all") <<"\n";
              outfile << ss.str() <<std::endl;
              //outfile.flush();
            }
            logtimer = now;
        }
      }
    }
    outfile.close();
    return 0;
}
