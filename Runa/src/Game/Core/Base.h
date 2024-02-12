#pragma once

#include "PlatformDetection.h"
#include <memory>

#ifdef RUNA_DEBUG
#if defined(RUNA_PLATFORM_WINDOWS)
#define RUNA_DEBUGBREAK() __debugbreak()
#elif defined(RUNA_PLATFORM_LINUX)
#include <signal.h>
#define RUNA_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define RUNA_ENABLE_ASSERTS
#else
#define RUNA_DEBUGBREAK()
#endif

#define RUNA_EXPAND_MACRO(x) x
#define RUNA_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define RUNA_BIND_EVENT_FN(fn) [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Runa
{
	template <typename T>
	using Scope = std::unique_ptr<T>;
	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args &&...args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;
	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args &&...args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
