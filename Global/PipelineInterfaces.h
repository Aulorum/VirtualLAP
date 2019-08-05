#pragma once

#include <vector>
#include "glm/glm/glm.hpp" //Math Types
#include <opencv2/core.hpp>

struct CameraImageData {
    int Width;
    int Height;
    char* Data; //RGB data stream
};

enum ImageAnalysisState {
    ImageAnalysis_Unknown       = 0x0000,
    ImageAnalysis_Calibrating   = 0x0001,
    ImageAnalysis_Operating     = 0x0002,

    ImageAnalysis_Simulating    = 0x0012,
    ImageAnalysis_MarkerOutput  = 0x0021,

    ImageAnalysis_DebugOverlay  = 0x1000
};
struct MarkerInfo {
    int id;
    glm::vec3 Location;
    glm::vec3 Direction;
    glm::vec3 Normal;
    cv::Vec3d Location_CV;
    cv::Vec3d Normal_CV;
    cv::Vec3d Direction_CV;

};
struct ImageAnalysisResult {
    ImageAnalysisState State;
    double CalibrationError;

    glm::mat4 ViewMatrix;

    std::vector<MarkerInfo> Markers;
};

struct TrackControlPoint {
    bool Important; //Does the track have to go trough this point?
    glm::vec3 Location;
    glm::vec3 Normal;
    //ToDo(AMü): Additional info, like pavement type
};
struct TrackGeometry {
    //ToDo(AMü): Should the generator already triangulate this?
    std::vector<TrackControlPoint> ControlPoints;
};
struct TrackControlPoint_CV{
    cv::Vec3d Location_CV;
    cv::Vec3d Normal_CV;
    cv::Vec3d First_CV;
    cv::Vec3d Next_CV;

};
struct TrackGeometry_CV{
    std::vector<TrackControlPoint_CV> ControlPoints_CV;
};


struct GameState {

};