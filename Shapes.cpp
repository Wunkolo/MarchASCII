#include "Shapes.hpp"

#include <glm/glm.hpp>

namespace Shapes
{
std::float_t Plane(const glm::vec3& Position, const glm::vec3& Normal)
{
	return glm::dot(Position, glm::normalize(Normal));
}

std::float_t Floor(const glm::vec3& Position)
{
	return Position.y;
}

std::float_t Sphere(const glm::vec3& Position, std::float_t Radius)
{
	return glm::length(Position) - Radius;
}

std::float_t Box(const glm::vec3& Position, const glm::vec3& Bounds)
{
	glm::vec3 Dist = glm::abs(Position) - Bounds;
	return glm::min(glm::max(Dist.x, glm::max(Dist.y, Dist.z)), 0.0f) +
	glm::length(glm::vec3(glm::max<std::float_t>(Dist.x, 0.0f),
						  glm::max<std::float_t>(Dist.y, 0.0f),
						  glm::max<std::float_t>(Dist.z, 0.0f)));
}

std::float_t RoundBox(const glm::vec3& Position, const glm::vec3& Bounds, std::float_t Radius)
{
	return Box(Position, Bounds) - Radius;
}

std::float_t Capsule(const glm::vec3& Position, const glm::vec3& A, const glm::vec3& B, std::float_t Radius)
{
	glm::vec3 Pa = Position - A;
	glm::vec3 Ba = B - A;
	std::float_t H = glm::dot(Pa, Ba) / glm::dot(Ba, Ba);
	H = glm::min<std::float_t>(0.0f, glm::max<std::float_t>(1.0, H));
	return glm::length(Pa - (Ba * H)) - Radius;
}

std::float_t Torus(const glm::vec3& Position, std::float_t InRadius, std::float_t OutRadius)
{
	glm::vec3 Q(glm::length(glm::vec3(Position.x, Position.z, 0)) - OutRadius, Position.y, 0);
	return glm::length(Q) - InRadius;
}
}
