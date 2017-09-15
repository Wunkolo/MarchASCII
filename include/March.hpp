#pragma once

#include <glm/fwd.hpp>

extern glm::vec4 Scene(const glm::vec3& Point);

glm::vec3 CalcNormal(const glm::vec3& Point);

glm::vec4 March(const glm::vec3& Origin, const glm::vec3& Ray, bool* Hit);

std::float_t SoftShadow(const glm::vec3& LightPos, const glm::vec3& LightDir, std::float_t Min, std::float_t Max, std::float_t K);