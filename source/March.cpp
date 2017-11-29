#include <March.hpp>

#include <cstddef>
#include <cstdint>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_AVX2
#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Operations.hpp"
#include "Shapes.hpp"

#define PREC 0.003f

glm::vec3 CalcNormal(const glm::vec3& Point)
{
	const glm::vec2 Epsilon = glm::vec2(1.0, -1.0) * (PREC * 3.0f);
	return glm::normalize(glm::vec3(
		glm::xyy(Epsilon) * Scene(Point + glm::xyy(Epsilon)).w +
		glm::yyx(Epsilon) * Scene(Point + glm::yyx(Epsilon)).w +
		glm::yxy(Epsilon) * Scene(Point + glm::yxy(Epsilon)).w +
		glm::xxx(Epsilon) * Scene(Point + glm::xxx(Epsilon)).w
	));
}

glm::vec4 March(const glm::vec3& Origin, const glm::vec3& Ray, bool* Hit)
{
	constexpr std::float_t NearPlane = 0.125;
	constexpr std::float_t FarPlane = 100.0f;

	glm::vec4 t;
	t.w = NearPlane;

	for( std::size_t i = 0; i < 128; i++ )
	{
		const std::float_t Precision = PREC * t.w;
		const glm::vec4 Query = Scene(Origin + Ray * t.w);
		if( Query.w < Precision )
		{
			*Hit = true;
			t = glm::vec4(glm::xyz(Query), t.w);
			break;
		}
		if( t.w > FarPlane )
		{
			break;
		}
		t.w += Query.w;
	}
	return t;
}

std::float_t SoftShadow(const glm::vec3& LightPos, const glm::vec3& LightDir, std::float_t Min, std::float_t Max, std::float_t K)
{
	std::float_t Res = 1.0f;
	for( std::float_t t = Min; t < Max; )
	{
		const std::float_t Distance = Scene(LightPos + LightDir * glm::vec3(t)).w;
		if( Distance < PREC )
		{
			return 0.0f;
		}
		Res = glm::min(Res, K * Distance / t);
		t += Distance;
	}
	return Res;
}
