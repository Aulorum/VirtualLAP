#pragma once

#include "PipelineInterfaces.h"

class TrackGeneration {
private: //Class members

public:  //Public methods
    TrackGeneration();
    ~TrackGeneration();

    void Step(ImageAnalysisResult* input, TrackGeometry* output);
private: //Private methods
    glm::vec3 moveAlongDirection(const glm::mat4 &trans,  float distance) const;
};
