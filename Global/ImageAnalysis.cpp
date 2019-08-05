//
// Created by Unknown on 05.05.2019.
//

#include "ImageAnalysis.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "glm/glm/gtc/matrix_transform.hpp"
#include <opencv2/highgui.hpp> //Enable for debugging using cv::imshow, also requires adding highgui in Desktop/CMakeList.txt
#include <opencv2/calib3d.hpp>
#include "glm/glm/gtx/string_cast.hpp"


#define PI 3.14159265

constexpr int CALIBRATIONFRAMESPACING = 1000; //ms

ImageAnalysis::ImageAnalysis() :
    _state((ImageAnalysisState)(ImageAnalysis_Calibrating | ImageAnalysis_DebugOverlay)),
    _dict(cv::aruco::Dictionary::get(cv::aruco::DICT_5X5_250)),
    _calibBoard(cv::aruco::CharucoBoard::create(7, 7, 30, 20, _dict))
{
    _detectorParams = cv::aruco::DetectorParameters::create();
    //Note: If we're having performance problems, trying other refinements is an easy optimization:
    _detectorParams->cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX;
}

ImageAnalysis::~ImageAnalysis() {

}

void ImageAnalysis::Step(const CameraImageData *cameraImage, ImageAnalysisResult *result) {
    //ToDo: Implement image Analysis here. For debugging purposes, you can edit the camera image to draw stuff to the output window
    //Also, contents of the result struct will be printed
    cv::Mat inputImage(cameraImage->Height, cameraImage->Width, CV_8UC3, cameraImage->Data);
    result->ViewMatrix = glm::mat4(1.f);

    switch(_state & 0xFF) {
        case ImageAnalysis_Calibrating: {
            //Always specify distances in millimeters
            time_point<system_clock> now = system_clock::now();
            auto calibAge = duration_cast<milliseconds>(now - _calibLastFrameTime);
            //if(calibAge.count() >= CALIBRATIONFRAMESPACING) {
                _calibrate(inputImage);
                _calibLastFrameTime = now;
            //}
            break;
        }
        case ImageAnalysis_Operating: {
            _detectMarkers(inputImage, result);
            break;
        }
        case ImageAnalysis_Simulating: {
            _detectMarkers(inputImage, result);
            float clock_ms = duration_cast<milliseconds>(
                    time_point_cast<milliseconds>(std::chrono::system_clock::now()).time_since_epoch() ).count() % (3141 * 2);


            // Was passiert hier? TODO: Erklärung bitte
            //auto rotate = glm::rotate(glm::mat4(1.0f), 1.0f, glm::vec3(0,0,1));
            //glm::vec3 location = rotate * glm::vec4(1000, 0, 1000, 1);
            // result->ViewMatrix = glm::lookAt(location, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f));

                        /*result->Markers.emplace_back(MarkerInfo {
                    0, //Marker 0 always should have these properties, since it defines the origin
                    glm::vec3(0,0,0),
                    glm::vec3(0, 1, 0),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    1,
                    glm::vec3(0,300,0),
                    glm::vec3(0, 1, 0),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    2,
                    glm::vec3(tvecs[2][0], tvecs[2][1], -tvecs[2][2]),
                    glm::vec3(rvecs[2][0], rvecs[2][1], rvecs[2][2]),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    3,
                    glm::vec3(tvecs[3][0], tvecs[3][1], -tvecs[3][2]),
                    glm::vec3(rvecs[3][0], rvecs[3][1], rvecs[3][2]),
                    glm::vec3(0,0,1)
            });
            result->Markers.emplace_back(MarkerInfo {
                    4,
                    glm::vec3(tvecs[4][0], tvecs[4][1], -tvecs[4][2]),
                    glm::vec3(rvecs[4][0], rvecs[4][1], rvecs[4][2]),
                    glm::vec3(0,0,1)
            });*/
            break;
        }
        case ImageAnalysis_MarkerOutput: {
            cv::Mat marker(1000, 1000, CV_8UC3);
            _calibBoard->draw(marker.size(), marker);
            cv::imshow("board", marker);
            cv::waitKey(0);
            break;
        }
        case ImageAnalysis_Unknown:
        default:
            std::cerr << "ImgAnalysis state: "<< std::hex << _state << std::endl;
            throw std::logic_error("Image analysis got into unknown state");
    }
    result->CalibrationError = _calibCurrentError;
    result->State = _state;
}

void ImageAnalysis::ChangeState(const ImageAnalysisState &newstate) {
    switch (newstate) {
        case ImageAnalysis_Simulating:
            if((_state & 0xFF) != ImageAnalysis_Simulating) {
                //For switching to simulating, we don't need to be in a specific prior state
                _state = ImageAnalysis_Simulating;
            }
            else
                _state = ImageAnalysis_Calibrating;
            break;
        case ImageAnalysis_DebugOverlay:
            _state = (ImageAnalysisState)(_state ^ ImageAnalysis_DebugOverlay); //Toggle Overlay
            break;
        case ImageAnalysis_MarkerOutput:
            _state = ImageAnalysis_MarkerOutput;
            break;
        default:
            throw std::logic_error("State switch not implemented");
    }
}

void ImageAnalysis::_calibrate(cv::Mat& cameraImage) {

    std::vector< int > markerIds;
    std::vector< std::vector< cv::Point2f > > markerCorners, markerRejected;
    cv::aruco::detectMarkers(cameraImage, _dict, markerCorners, markerIds,
                             _detectorParams, markerRejected);

    if(markerIds.empty())
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedMarkers(cameraImage, markerCorners, markerIds, cv::Scalar(0,255,0));
        cv::aruco::drawDetectedMarkers(cameraImage, markerRejected, cv::noArray(), cv::Scalar(255,0,0));
    }

    std::vector< cv::Point2f > charucoCorners;
    std::vector< int > charucoIds;
    cv::aruco::interpolateCornersCharuco(markerCorners, markerIds, cameraImage,
            _calibBoard, charucoCorners, charucoIds);

    if(charucoIds.size() < 2)
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedCornersCharuco(cameraImage, charucoCorners, charucoIds, cv::Scalar(0,0,255));
    }

    _calibFramesCorners.push_back(charucoCorners);
    _calibFramesIds.push_back(charucoIds);

    _calibCurrentError = cv::aruco::calibrateCameraCharuco(_calibFramesCorners,
            _calibFramesIds, _calibBoard, cameraImage.size(), _camera, _distortion);

    if(_calibFramesIds.size() >= 20 && _calibCurrentError < 1.4) {
        _state = static_cast<ImageAnalysisState>(
                (_state | ImageAnalysis_Operating) & ~ImageAnalysis_Calibrating);
    }
}

void ImageAnalysis::_detectMarkers(cv::Mat &cameraImage, ImageAnalysisResult* result) {
    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f> > markerCorners;
    cv::aruco::DetectorParameters detectorParams;

    cv::aruco::detectMarkers(cameraImage, _dict, markerCorners, markerIds,
                             _detectorParams, cv::noArray(), _camera, _distortion);

    if(markerIds.empty())
        return;

    if(_state & ImageAnalysis_DebugOverlay) {
        cv::aruco::drawDetectedMarkers(cameraImage, markerCorners, markerIds, cv::Scalar(0,128,0));
    }

    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::aruco::estimatePoseSingleMarkers(markerCorners, 20.f, _camera, _distortion, rvecs, tvecs);


    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
    for(auto i = 0; i < markerIds.size(); i++){
        auto rvec = rvecs[i];
        auto markerFocus = 0;
        if(markerIds[i] == markerFocus) {
            _setViewMatrix(rvec, tvecs[i], result);
            cv::drawFrameAxes(cameraImage, _camera, _distortion, rvec, tvecs[i], 20.f);
        }

        cv::aruco::drawAxis(cameraImage, _camera, _distortion, rvec, tvecs[i], 20.f);
        if (markerIds[i] < 4){
            std::cout << tvecs[i]<< " " << std::endl;
            result->Markers.emplace_back(MarkerInfo {
                    markerIds[i],
                    glm::vec3(tvecs[i][0], -tvecs[i][1], -tvecs[i][2]),
                    glm::vec3(rvecs[i][0], rvecs[i][1], rvecs[i][2]),
                    glm::vec3(0,0,1),
                    cv::Vec3d(tvecs[i][0], tvecs[i][1], -tvecs[i][2]),
                    rvecs[i],
                    cv::Vec3d(0,0,1)

            });
        }
    }
    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
    _sortMarkers(result);
}


void ImageAnalysis::_sortMarkers(ImageAnalysisResult* result){
    auto markares = result->Markers;
    if (markares.size() > 1) {
        for (auto i = 0; i < markares.size()-1; i++){
            if (markares[i].id > markares[i+1].id){
                auto h_m = markares[i+1];
                markares[i+1] = markares[i];
                markares[i] = h_m;
            }
        }
    }
}

void ImageAnalysis::_setViewMatrix(cv::Vec3d &rvec0, cv::Vec3d &tvec0, ImageAnalysisResult* result) {
    cv::Mat rotation;
    cv::Mat viewMatrix = cv::Mat::zeros(4, 4, CV_64FC1);
    cv::Rodrigues(rvec0, rotation);
    for (size_t j = 0; j < 3; ++j) {
        for (size_t k = 0; k < 3; ++k) {
            viewMatrix.at<double>(j, k) = rotation.at<double>(j, k);
        }
    }
    viewMatrix.at<double>(3, 3) = 1.0f;


    cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_64F);
    cvToGl.at<double>(0, 0) = 1.0f;
    cvToGl.at<double>(1, 1) = -1.0f; // Invert the y axis
    cvToGl.at<double>(2, 2) = -1.0f; // invert the z axis
    cvToGl.at<double>(3, 3) = 1.0f;
    viewMatrix = cvToGl * viewMatrix;

    cv::Mat glViewMatrix = cv::Mat::zeros(4, 4, CV_64F);
    cv::transpose(viewMatrix, glViewMatrix);

    for (size_t j = 0; j < 4; ++j) {
        for (size_t k = 0; k < 4; ++k) {
            result->ViewMatrix[j][k] = glViewMatrix.at<double>(j, k);
        }
    }

    result->ViewMatrix[3][0] = tvec0[0];
    result->ViewMatrix[3][1] = -tvec0[1];
    result->ViewMatrix[3][2] = -tvec0[2];
}