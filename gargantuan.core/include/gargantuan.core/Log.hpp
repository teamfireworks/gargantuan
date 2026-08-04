#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>

#define LOG_BASE(logger, category, ...) logger(gargantuan::LogCategory::category, __VA_ARGS__)

#define LOG_TRACE(category, ...) LOG_BASE(SDL_LogTrace, category, __VA_ARGS__)
#define LOG_VERBOSE(category, ...) LOG_BASE(SDL_LogVerbose, category, __VA_ARGS__)
#define LOG_DEBUG(category, ...) LOG_BASE(SDL_LogDebug, category, __VA_ARGS__)
#define LOG_INFO(category, ...) LOG_BASE(SDL_LogInfo, category, __VA_ARGS__)
#define LOG_WARN(category, ...) LOG_BASE(SDL_LogWarn, category, __VA_ARGS__)
#define LOG_ERROR(category, ...) LOG_BASE(SDL_LogError, category, __VA_ARGS__)
#define LOG_CRITICAL(category, ...) LOG_BASE(SDL_LogCritical, category, __VA_ARGS__)

namespace gargantuan {
	enum LogCategory : int { App = SDL_LOG_CATEGORY_CUSTOM + 1, Lua };

	enum class AnsiCode : int {
		Reset,
		Bold,
		Dim,
		Black = 30,
		Red,
		Green,
		Yellow,
		Blue,
		Magenta,
		Cyan,
		White,
		Default = 39,
		BgBlack,
		BgRed,
		BgGreen,
		BgYellow,
		BgBlue,
		BgMagenta,
		BgCyan,
		BgDefault = 49,
	};

	struct LogContext {
		bool EnableAnsi;
		bool EnablePretty;
	};

	SDL_LogOutputFunction GetLogOutputFunction(LogContext *logContext);
}
