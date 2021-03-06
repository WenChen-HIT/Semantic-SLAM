/**
* This file is part of ORB-SLAM2.
*
* Copyright (C) 2014-2016 Raúl Mur-Artal <raulmur at unizar dot es> (University of Zaragoza)
* For more information see <https://github.com/raulmur/ORB_SLAM2>
*
* ORB-SLAM2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* ORB-SLAM2 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ORB-SLAM2. If not, see <http://www.gnu.org/licenses/>.
*/


#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include<iomanip>

#include<opencv2/core/core.hpp>

#include"System.h"

//darknet
#include"Thirdparty/darknet/src/yolo.h"
#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
//    FileStorage fs("config.yml", FileStorage::WRITE);
//    fs << "dataPath" <<  "./cfg/coco.data";
//    fs << "netCfgPath" << "./cfg/yolo.cfg";
//    fs << "weightPath" << "./data/yolo.weights";
//    fs << "nameListPath" << "./data/coco.names";
//    fs << "alphabetPath" << "./data/labels/";
//    fs << "imgPathFile" << "./robotcar2015-11-11-14-13-43.txt";
//    fs << "thresh" << 0.30;
//    fs << "hier_thresh" << 0.50;
//    fs << "nms" << 0.40;
    if(argc < 2){
        fprintf(stderr, "usage: %s <cfgfile>\n", argv[0]);
        return 0;
    }

//    std::ofstream file("out.csv");
    int nFrame = 0;

    Yolo yolo;
    yolo.readConfig(argv[1]);
    yolo.loadConfig();

    char* imgPath = yolo.getImgPath();

    ifstream imgPathFile(imgPath);
    if(!imgPathFile.is_open()){
        cout << "cannot open img path file" << endl;
        return 0;
    }

    string videoFile = yolo.getVideoPath();
    VideoWriter recordVideo(videoFile, VideoWriter::fourcc('M','J','P','G'), 10, Size(1280,960));

    cv::Mat img;
    string imgPathLine;

    vector<string> names = yolo.get_labels_();

    std::cout << "--------" << std::endl;
    std::cout << "Object class names: " << std::endl;
    for(int i = 0; i < names.size(); i++)
        std::cout << names[i] << std::endl;

    while(getline(imgPathFile, imgPathLine))
    {
        img = imread(imgPathLine.c_str());
//        img = img(Range(0,820), Range::all());
        if(img.empty())
            break;

        std::vector<DetectedObject> detection;

        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        yolo.detect(img, detection);
        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();
        std::cout << ttrack << std::endl;

        std ::cout << "Frame: " << nFrame << " detect objects " << int(detection.size()) << std::endl;

        for(int i = 0; i < detection.size(); i++)
        {
            DetectedObject& o = detection[i];
            cv::rectangle(img, o.bounding_box, cv::Scalar(255,0,0), 2);

            string class_name = names[o.object_class];

            char str[255];
            //sprintf(str,"%s %f", names[o.object_class], o.prob);
            sprintf(str,"%s %%%.2f", class_name.c_str(), o.prob);
            cv::putText(img, str, cv::Point2f(o.bounding_box.x,o.bounding_box.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,255,0), 1);
        }
        recordVideo.write(img);
        cv::imshow("yolo-robotcar demo", img);
        cv::waitKey(1);
        nFrame++;
    }
    img.release();
    recordVideo.release();
    imgPathFile.close();
}


//void LoadImages_robotcar(const string &strSequence, vector<string> &vstrImageFilenames,
//                vector<double> &vTimestamps);

//int main(int argc, char **argv)
//{
//    if(argc != 4)
//    {
//        cerr << endl << "Usage: ./mono_robotcar path_to_vocabulary path_to_settings path_to_sequence" << endl;
//        return 1;
//    }
//    // Retrieve paths to images, load Image file path and time of every frame (second)
//    vector<string> vstrImageFilenames;
//    vector<double> vTimestamps;
//    LoadImages_robotcar(string(argv[3]), vstrImageFilenames, vTimestamps);
//    int nImages = vstrImageFilenames.size();

//    // Create SLAM system. It initializes all system threads and gets ready to process frames.
//    ORB_SLAM2::System SLAM(argv[1],argv[2],ORB_SLAM2::System::MONOCULAR,true);
//    // Vector for tracking time statistics
//    vector<float> vTimesTrack;
//    vTimesTrack.resize(nImages);
//    cout << endl << "-------" << endl;
//    cout << "Start processing sequence ..." << endl;
//    cout << "Images in the sequence: " << nImages << endl << endl;

//    // Main loop
//    cv::Mat im;
//    for(int ni=0; ni<nImages; ni++)
//    {
//        // Read image from file
//        im = cv::imread(vstrImageFilenames[ni],CV_LOAD_IMAGE_UNCHANGED);
//        double tframe = vTimestamps[ni];

//        if(im.empty())
//        {
//            cerr << endl << "Failed to load image at: " << vstrImageFilenames[ni] << endl;
//            return 1;
//        }

//#ifdef COMPILEDWITHC11
//        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
//#else
//        std::chrono::monotonic_clock::time_point t1 = std::chrono::monotonic_clock::now();
//#endif

//        // Pass the image to the SLAM system
//        SLAM.TrackMonocular(im,tframe);

//#ifdef COMPILEDWITHC11
//        std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
//#else
//        std::chrono::monotonic_clock::time_point t2 = std::chrono::monotonic_clock::now();
//#endif

//        double ttrack= std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1).count();

//        vTimesTrack[ni]=ttrack;

//        // Wait to load the next frame
//        double T=0;
//        if(ni<nImages-1)
//            T = vTimestamps[ni+1]-tframe;
//        else if(ni>0)
//            T = tframe-vTimestamps[ni-1];

//        if(ttrack<T)
//            usleep((T-ttrack)*1e6);
//    }

//    // Stop all threads
//    SLAM.Shutdown();

//    // Tracking time statistics
//    sort(vTimesTrack.begin(),vTimesTrack.end());
//    float totaltime = 0;
//    for(int ni=0; ni<nImages; ni++)
//    {
//        totaltime+=vTimesTrack[ni];
//    }
//    cout << "-------" << endl << endl;
//    cout << "median tracking time: " << vTimesTrack[nImages/2] << endl;
//    cout << "mean tracking time: " << totaltime/nImages << endl;

//    // Save camera trajectory
//    SLAM.SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

//    return 0;
//}

//void LoadImages_robotcar(const string &strPathToSequence, vector<string> &vstrImageFilenames, vector<double> &vTimestamps)
//{
//    ifstream fTimes;
//    string strPathTimeFile = strPathToSequence + "time.txt";
//    fTimes.open(strPathTimeFile.c_str());
//    if(!fTimes.is_open()){
//        cerr << "Cannot open " << strPathTimeFile << endl;
//        exit(0);
//    }
//    while(!fTimes.eof())
//    {
//        string s;
//        getline(fTimes,s);
//        if(!s.empty())
//        {
//            stringstream ss;
//            ss << s;
//            double t;
//            ss >> t;
//            vTimestamps.push_back(t);
//        }
//    }

//    string strPrefixLeft = strPathToSequence + "imagePath.txt";
//    ifstream pathFile(strPrefixLeft.c_str());
//    if(!pathFile.is_open()){
//        cerr << "Cannot open " << strPrefixLeft << endl;
//        exit(0);
//    }

//    const int nTimes = vTimestamps.size();
//    vstrImageFilenames.resize(nTimes);

//    for(int i=0; i<nTimes; i++)
//    {
//        string s;
//        getline(pathFile, s);
//        if(!s.empty()){
//            vstrImageFilenames[i] = s;
//        }
//    }
//    fTimes.close();
//    pathFile.close();
//}
