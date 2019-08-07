//
// Created by Unknown on 05.05.2019.
//

#include "TrackGeneration.h"
#include <iostream>
#include "glm/glm/gtx/string_cast.hpp"

constexpr  float TANGENTCONTROLDISTANCE = 50.f;

TrackGeneration::TrackGeneration() {

}

TrackGeneration::~TrackGeneration() {

}

void TrackGeneration::Step(ImageAnalysisResult *input, TrackGeometry *output) {
    if (input->Markers.size() < 2) {
        std::cout << " Not enough markers" << std::endl;
        return; //Can't build a track without at least 2 markers
    }
    auto first = input->Markers[0];
    std::cout << " First marker " <<  glm::to_string(glm::vec3(first.Transformation[3][0], first.Transformation[3][1], first.Transformation[3][2])) << std::endl;

    // Start For OpenGL
    output->ControlPoints.emplace_back(TrackControlPoint {
        true, glm::vec3(first.Transformation[3][0], first.Transformation[3][1], first.Transformation[3][2]), first.Normal
    });
    output->ControlPoints.emplace_back(TrackControlPoint {
        false,
        moveAlongDirection(first.Transformation, TANGENTCONTROLDISTANCE),
        first.Normal
    });
    for (size_t i = 1; i < input->Markers.size(); ++i) {
        auto src = input->Markers[i];
        auto last = input->Markers[i-1];
        // OpenGL part
        /*output->ControlPoints.emplace_back(TrackControlPoint {
                false,
                moveAlongDirection(src.Transformation, -TANGENTCONTROLDISTANCE),
                src.Normal
        });*/
        output->ControlPoints.emplace_back(TrackControlPoint {
            true,
            glm::vec3(src.Transformation[3][0], src.Transformation[3][1], src.Transformation[3][2]),
            src.Normal
        });
        /*output->ControlPoints.emplace_back(TrackControlPoint {
                false,
                moveAlongDirection(src.Transformation, TANGENTCONTROLDISTANCE),
                src.Normal
        });*/


    }
    // OpenGL part
    /*output->ControlPoints.emplace_back(TrackControlPoint {
        false,
        moveAlongDirection(first.Transformation, -TANGENTCONTROLDISTANCE),
        first.Normal
    });*/
    output->ControlPoints.emplace_back(TrackControlPoint {
        true, glm::vec3(first.Transformation[3][0], first.Transformation[3][1], first.Transformation[3][2]), first.Normal
    });


}

/**
 * Creates a Point to create a curve From one Point to another
 * TODO: Something in Calculation might be wrong.
 * @param trans
 * @param distance
 * @return a Point to give the tra a direction
 */
glm::vec3 TrackGeneration::moveAlongDirection(const glm::mat4 &trans, float distance) const {
    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
    glm::vec4 latation = glm::vec4(distance, distance, distance, 1.f);
    auto h = trans * latation;
    glm::vec3 result = glm::vec3(h[0], h[1], h[2]);
    std::cout << " End Point" << glm::to_string(result) << std::endl;
    return result;
}

