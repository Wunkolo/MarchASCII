#include <cstdio>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <thread>

#include "Shapes.hpp"
#include "Operations.hpp"
#include "March.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_INLINE
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define WIDTH (80)
#define HEIGHT (40) 
constexpr glm::vec2 Resolution(WIDTH, HEIGHT);

#define DELAY 16

constexpr char Shades[10] = ".:*oe&#%@";

static std::size_t Tick = 0;

glm::vec4 Scene(const glm::vec3& Point)
{
	const std::float_t Phase = 1 / static_cast<std::float_t>(DELAY);
	const std::float_t Wave = powf(sinf(Phase * (glm::pi<std::float_t>() / 2.0f)), 2.0f);

	glm::vec4 Query = glm::vec4(0, 1, 1, Shapes::Floor(Point));

	Query = Operations::Union(
		glm::vec4(1, 0, 0, Shapes::Box(
			glm::rotate(Point - glm::vec3(0, 10, 0), Phase * glm::half_pi<std::float_t>(), glm::vec3(1.0f)),
			glm::vec3(8, 8, 8)
		)),
		Query
	);

	Query = Operations::Subtract(
		glm::vec4(0, 0, 1, Shapes::Sphere(
			Point - glm::vec3(0.0f, 10.0f, 0.0f),
			10.0f
		)),
		Query
	);

	Query = Operations::Union(
		glm::vec4(1, 1, 0, Shapes::Torus(
			glm::rotate(Point - glm::vec3(0, 5, 0), Phase * glm::pi<std::float_t>() / 2.0f, glm::vec3(1.0f)),
			1.5f,
			15.0f
		)),
		Query
	);

	constexpr std::float_t Radius = 25.0f;
	constexpr std::size_t Count = 3;
	for( std::size_t i = 0; i < Count; i++ )
	{
		const std::float_t Completion = i / static_cast<std::float_t>(Count);
		Query = Operations::Union(
			glm::vec4(0, 1, 0, Shapes::Box(
				Point - glm::vec3(Radius * glm::cos(2.0f * glm::pi<std::float_t>() * Completion), 5.0f, Radius * glm::sin(2.0f * glm::pi<std::float_t>() * Completion)),
				glm::vec3(3, 10, 3)
			)),
			Query
		);
	}

	Query = Operations::Subtract(
		glm::vec4(1, 0, 1, Shapes::Torus(
			Point - glm::vec3(0, 8, 0),
			3.0f,
			25.0f
		)),
		Query
	);

	Query = Operations::Subtract(
		glm::vec4(1, 0, 1, Shapes::Sphere(
			Point - glm::vec3(glm::cos(glm::half_pi<std::float_t>() * Phase) * 25.0f, 2.0f, glm::sin(glm::half_pi<std::float_t>() * Phase) * 25.0f),
			9.0f
		)),
		Query
	);

	Query = Operations::Union(
		glm::vec4(1, 1, 1, Shapes::Sphere(
			Point - glm::vec3(glm::cos(glm::half_pi<std::float_t>() * Phase) * 25.0f, 2.0f, glm::sin(glm::half_pi<std::float_t>() * Phase) * 25.0f),
			3.0f
		)),
		Query
	);

	return Query;
}

struct RenderState
{
	char Screen[WIDTH * HEIGHT];
	std::uint16_t ScreenAttr[WIDTH * HEIGHT];
	glm::vec3 LightDir;
	glm::vec3 EyePos;
	glm::vec3 EyeAt;
	glm::mat3 ViewMatrix;
	std::float_t FOV;
};

void Scanline(std::size_t Start, std::size_t Quantum, RenderState& State)
{
	for( std::size_t Row = Start; Row < HEIGHT; Row += Quantum )
	{
		char* CurChar = State.Screen + Row * WIDTH;
		std::uint16_t* CurAttr = State.ScreenAttr + Row * WIDTH;
		for( std::size_t x = 0; x < WIDTH; x++ )
		{
			const glm::vec2 PixelCoord(x, Row);

			const glm::vec2 ScreenCoord = glm::vec2(1, -1) * (-Resolution + 2.0f * PixelCoord) / Resolution.y;

			const glm::vec3 RayDir = State.ViewMatrix * glm::normalize(glm::vec3(ScreenCoord, -1.0f / State.FOV));

			bool Hit = false;

			const glm::vec4 Query = March(State.EyePos, RayDir, &Hit);

			if( Hit )
			{
				const glm::vec3 Point = glm::fma<glm::vec3>(glm::vec3(Query.w), RayDir, State.EyePos);
				const glm::vec3 Normal = CalcNormal(Point);
				const glm::vec3 Light = State.LightDir;
				const glm::vec3 View = -RayDir;
				const glm::vec3 Half = glm::normalize(Light + View);
				const glm::vec3 Reflect = glm::reflect(Light, Normal);

				const std::float_t NdotL = glm::dot(Normal, Light);
				const std::float_t NdotH = glm::dot(Normal, Half);
				const std::float_t NdotV = glm::dot(Normal, View);
				const std::float_t VdotR = glm::dot(Reflect, View);

				// Diffuse
				const std::float_t Diffuse = glm::clamp(NdotL, 0.0f, 1.0f);
				const std::float_t Fresnel = glm::pow<std::float_t>(glm::clamp<std::float_t>(1.0f + NdotV, 0.0f, 1.0f), 2.0f);
				const std::float_t Specularity = NdotL > 0 ? glm::pow(NdotH, 64.0f) : 0.0f;
				const std::float_t Shadow = SoftShadow(Point, Light, 0.5f, 25.0f, 10.0f);

				std::float_t Shading = Diffuse + Specularity;

				Shading *= Shadow;
				Shading = glm::pow(Shading, 1.0f / 2.2f); // Gamma

				Shading = glm::clamp(Shading, 0.0f, 1.0f);
				*CurChar++ = Shades[static_cast<std::size_t>(Shading * sizeof(Shades - 1))];
				*CurAttr = 0;

				const glm::bvec3 Color = glm::greaterThan(glm::vec3(Query), glm::vec3(0.5f));

				*CurAttr |= Color.r ? 0x1 : 0;
				*CurAttr |= Color.b ? 0x4 : 0;
				*CurAttr |= Color.g ? 0x2 : 0;
				*CurAttr |= Specularity > (2.0f / 3.0f) ? ((*CurAttr << 4) | 0x0080) : 0;
				*CurAttr |= Diffuse > 0.125f ? 0x0008 : 0;

				CurAttr++;
			}
			else
			{
				*CurChar++ = ' ';
				*CurAttr++ = 0;
			}
		}
	}
}

int main()
{
	RenderState State;
	State.LightDir = glm::normalize(glm::vec3(1, 1, 0));
	State.EyeAt = glm::vec3(0, 5, 0);
	State.FOV = glm::tan(glm::radians(60.0f) / 2.0f);
	const static std::size_t Cores = std::thread::hardware_concurrency();
	std::vector<std::thread> Threads;
	Threads.reserve(Cores);
	
	char Screen[(WIDTH * 12 + 1) * HEIGHT + 1] = {};
	do
	{
		const std::float_t Phase = Tick / static_cast<std::float_t>(DELAY);
		State.EyePos = glm::vec3(35 * glm::sin(Phase * glm::pi<std::float_t>() / 10.0f), 10, 35 * glm::cos(Phase * glm::pi<std::float_t>() / 10.0f));
		State.ViewMatrix = glm::inverse(glm::lookAt(
			State.EyePos,
			State.EyeAt,
			glm::vec3(0.0f, 1.0f, 0.0f)
		));
		for( std::size_t i = 0; i < Cores; i++ )
		{
			Threads[i] = std::thread(Scanline, i, Cores, std::ref(State));
		}
		for( std::size_t i = 0; i < Cores; i++ )
		{
			Threads[i].join();
		}

		printf("\ec");
		for( std::size_t y = 0; y < HEIGHT; y++ )
		{
			char *Row = Screen + y * (WIDTH * 12 + 1);
			for( std::size_t x = 0; x < WIDTH; x++ )
			{
				Row += sprintf(
					Row,
					"\e[1;3%u;%03um%c",
					State.ScreenAttr[x + y * WIDTH] % 8,
					State.ScreenAttr[x + y * WIDTH] & 0x80 ? (100 + State.ScreenAttr[x + y * WIDTH] % 8) : 49,
					State.Screen[x + y * WIDTH]
				);
			}
			*Row++ = '\n';
		}
		puts(Screen);
		fflush(stdout);
		std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
		Tick++;
	}
	while( Tick < 1000 );
	return 0;
}
