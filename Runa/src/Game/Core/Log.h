#pragma once

#include "Game/Core/Base.h"

#define RUNA_ENABLE_ASSERTS
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Runa
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger> &GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}
//
// template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
// inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
//{
//	return os << glm::to_string(vector);
//}
//
// template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
// inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
//{
//	return os << glm::to_string(matrix);
//}
//
// template <typename OStream, typename T, glm::qualifier Q>
// inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
//{
//	return os << glm::to_string(quaternion);
//}

// Core log macros (unused since I do not intend to subdivide my code into Core and Client so far)
#define RUNA_CORE_TRACE(...) ::Runa::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RUNA_CORE_INFO(...) ::Runa::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RUNA_CORE_WARN(...) ::Runa::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RUNA_CORE_ERROR(...) ::Runa::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RUNA_CORE_CRITICAL(...) ::Runa::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define RUNA_TRACE(...) ::Runa::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RUNA_INFO(...) ::Runa::Log::GetClientLogger()->info(__VA_ARGS__)
#define RUNA_WARN(...) ::Runa::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RUNA_ERROR(...) ::Runa::Log::GetClientLogger()->error(__VA_ARGS__)
#define RUNA_CRITICAL(...) ::Runa::Log::GetClientLogger()->critical(__VA_ARGS__)
