#pragma once

#include <cmath>
#include <glm/fwd.hpp>

namespace Operations
{
	glm::vec4 Union(glm::vec4 A, glm::vec4 B);

	std::float_t SmoothUnion(std::float_t A, std::float_t B, std::float_t K = 1.0f);

	glm::vec4 Intersection(glm::vec4 A, glm::vec4 B);

	glm::vec4 Subtract(glm::vec4 A, glm::vec4 B);
}