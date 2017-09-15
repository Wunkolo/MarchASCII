#include "Operations.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

namespace Operations
{
glm::vec4 Union(glm::vec4 A, glm::vec4 B)
{
	return A.w < B.w ? A : B;
}

std::float_t SmoothUnion(std::float_t A, std::float_t B, std::float_t K)
{
	std::float_t H = glm::clamp<std::float_t>(0.5f + 0.5f * (B - A) / K, 0.0f, 1.0f);
	return glm::mix(B, A, H) - (K * H * (1.0f - H));
}

glm::vec4 Intersection(glm::vec4 A, glm::vec4 B)
{
	return A.w > B.w ? A : B;
}

glm::vec4 Subtract(glm::vec4 A, glm::vec4 B)
{
	return Intersection(
		glm::vec4(glm::vec3(A),-A.w),
		B
	);
}
}
