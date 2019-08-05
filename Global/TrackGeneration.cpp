//
// Created by Unknown on 05.05.2019.
//

#include "TrackGeneration.h"
#include <iostream>

constexpr float TANGENTCONTROLDISTANCE = 200.f;

TrackGeneration::TrackGeneration() {

}

TrackGeneration::~TrackGeneration() {

}

void TrackGeneration::Step(ImageAnalysisResult *input, TrackGeometry *output, TrackGeometry_CV *output_CV) {
    if (input->Markers.size() < 2) {
        std::cout << " Not enough markers" << std::endl;
        return; //Can't build a track without at least 2 markers
    }
    auto first = input->Markers[0];
    auto second = input->Markers[1];
    // Start For OpenGL
    output->ControlPoints.emplace_back(TrackControlPoint {
        true, first.Location, first.Normal
    });
    output->ControlPoints.emplace_back(TrackControlPoint {
        false,
        moveAlongDirection(first.Location, first.Direction, TANGENTCONTROLDISTANCE),
        first.Normal
    });
    for (size_t i = 1; i < input->Markers.size(); ++i) {
        auto src = input->Markers[i];
        auto last = input->Markers[i-1];
        // OpenGL part
        output->ControlPoints.emplace_back(TrackControlPoint {
            false,
            moveAlongDirection(src.Location, src.Direction, -TANGENTCONTROLDISTANCE),
            src.Normal
        });
        output->ControlPoints.emplace_back(TrackControlPoint {
            true,
            src.Location,
            src.Normal
        });

        output->ControlPoints.emplace_back(TrackControlPoint {
                false,
                moveAlongDirection(src.Location, src.Direction, TANGENTCONTROLDISTANCE),
                src.Normal
        });
    }
    // OpenGL part
    output->ControlPoints.emplace_back(TrackControlPoint {
        false,
        moveAlongDirection(first.Location, first.Direction, -TANGENTCONTROLDISTANCE),
        first.Normal
    });
    output->ControlPoints.emplace_back(TrackControlPoint {
        true, first.Location, first.Normal
    });


}

glm::vec3 TrackGeneration::moveAlongDirection(const glm::vec3& start, const glm::vec3 &dir, float distance) const {
    return start + glm::normalize(dir)*distance;
}

cv::Vec3d TrackGeneration::cv_moveAlongDirection(const cv::Vec3d& start, const cv::Vec3d& dir, float distance) const{
    return start + cv::normalize(dir)*distance;
}
