#pragma once
#include <cmath>
#include <glm/fwd.hpp>

namespace Shapes
{
std::float_t Plane(const glm::vec3& Position, const glm::vec3& Normal);

std::float_t Floor(const glm::vec3& Position);

std::float_t Sphere(const glm::vec3& Position, std::float_t Radius);

std::float_t Box(const glm::vec3& Position, const glm::vec3& Bounds);

std::float_t RoundBox(const glm::vec3& Position, const glm::vec3& Bounds, std::float_t Radius);

std::float_t Capsule(const glm::vec3& Position, const glm::vec3& A, const glm::vec3& B, std::float_t Radius);

std::float_t Torus(const glm::vec3& Position, std::float_t InRadius, std::float_t OutRadius);
}
