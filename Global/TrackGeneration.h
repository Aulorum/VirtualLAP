#pragma once

#include "PipelineInterfaces.h"

class TrackGeneration {
private: //Class members

public:  //Public methods
    TrackGeneration();
    ~TrackGeneration();

    void Step(ImageAnalysisResult* input, TrackGeometry* output, TrackGeometry_CV *output_CV);
private: //Private methods
    glm::vec3 moveAlongDirection(const glm::vec3& start, const glm::vec3& dir, float distance) const;
    cv::Vec3d cv_moveAlongDirection(const cv::Vec3d& start, const cv::Vec3d& dir, float distance) const;
};
