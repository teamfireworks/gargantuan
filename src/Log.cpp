#include "gargantuan/Log.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <ctime>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

namespace gargantuan {
	static constexpr const char *ANSI_ESC = "\x1B";

	std::string FormatAnsiCode(const AnsiCode &code) {
		return std::string(ANSI_ESC) + "[" + std::to_string((int)code) + "m";
	};

	std::string GetCurrentTimestamp() {
		auto now = std::chrono::system_clock::now();
		auto timeNow = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

		std::tm tm_now{};
#if defined(_WIN32)
		localtime_s(&tm_now, &timeNow);
#else
		localtime_r(&timeNow, &tm_now);
#endif

		std::ostringstream ss;
		ss << "(" << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3)
		   << ms.count() << ")";
		return ss.str();
	}

	std::ostream &GetOutputStream(SDL_LogPriority priority) {
		return priority >= SDL_LOG_PRIORITY_ERROR ? std::cerr : std::cout;
	}

	std::vector<AnsiCode> StylePriority(SDL_LogPriority priority) {
		switch (priority) {
		case SDL_LOG_PRIORITY_TRACE:
			return {AnsiCode::Black, AnsiCode::Dim};
		case SDL_LOG_PRIORITY_VERBOSE:
			return {AnsiCode::Black};
		case SDL_LOG_PRIORITY_DEBUG:
			return {AnsiCode::Magenta};
		case SDL_LOG_PRIORITY_INFO:
			return {AnsiCode::Cyan};
		case SDL_LOG_PRIORITY_WARN:
			return {AnsiCode::Yellow, AnsiCode::Bold};
		case SDL_LOG_PRIORITY_ERROR:
			return {AnsiCode::Red};
		case SDL_LOG_PRIORITY_CRITICAL:
			return {AnsiCode::Red, AnsiCode::Bold};
		default:
			return {AnsiCode::Dim, AnsiCode::Bold};
		}
	}

	std::vector<AnsiCode> StyleCategory(int category) {
		switch (category) {
		case LogCategory::App:
		case SDL_LOG_CATEGORY_APPLICATION:
			return {AnsiCode::BgCyan, AnsiCode::Blue, AnsiCode::Bold};
		case LogCategory::Lua:
			return {AnsiCode::BgBlue, AnsiCode::White, AnsiCode::Bold};
		default:
			return {AnsiCode::BgBlack, AnsiCode::Dim, AnsiCode::Bold};
		}
	}

	std::string StringifyPaddedPriority(SDL_LogPriority priority) {
		switch (priority) {
		case SDL_LOG_PRIORITY_TRACE:
			return "[TRACE]   ";
		case SDL_LOG_PRIORITY_VERBOSE:
			return "[VERBOSE] ";
		case SDL_LOG_PRIORITY_DEBUG:
			return "[DEBUG]   ";
		case SDL_LOG_PRIORITY_INFO:
			return "[INFO]    ";
		case SDL_LOG_PRIORITY_WARN:
			return "[WARN]    ";
		case SDL_LOG_PRIORITY_ERROR:
			return "[ERROR]   ";
		case SDL_LOG_PRIORITY_CRITICAL:
			return "[CRITICAL]";
		case SDL_LOG_PRIORITY_COUNT:
			return "[COUNT]   ";
		default:
			return "[INVALID] ";
		}
	}

	std::string StringifyCategory(int category) {
		switch (category) {
		case LogCategory::App:
		case SDL_LOG_CATEGORY_APPLICATION:
			return "App";
		case LogCategory::Lua:
			return "Lua";
		default:
			return "SDL";
		}
	}

	std::optional<std::string> StringifySubcategory(int category) {
		switch (category) {
		case SDL_LOG_CATEGORY_ERROR:
			return "Error";
		case SDL_LOG_CATEGORY_ASSERT:
			return "Assert";
		case SDL_LOG_CATEGORY_SYSTEM:
			return "System";
		case SDL_LOG_CATEGORY_AUDIO:
			return "Audio";
		case SDL_LOG_CATEGORY_VIDEO:
			return "Video";
		case SDL_LOG_CATEGORY_RENDER:
			return "Render";
		case SDL_LOG_CATEGORY_INPUT:
			return "Input";
		case SDL_LOG_CATEGORY_TEST:
			return "Test";
		case SDL_LOG_CATEGORY_GPU:
			return "Gpu";
		default:
			return {};
		}
	}

	void PrettyOutput(void *contextUserdata, int category, SDL_LogPriority priority, const char *message) {
		auto logContext = static_cast<LogContext>(contextUserdata);

		std::ostringstream output;

		if (logContext.EnableAnsi) {
			for (auto code : StylePriority(priority)) {
				output << FormatAnsiCode(code);
			}
		};
		output << StringifyPaddedPriority(priority);
		if (logContext.EnableAnsi) output << FormatAnsiCode(AnsiCode::Reset);

		output << " ";

		if (logContext.EnableAnsi) {
			for (auto code : StyleCategory(category)) {
				output << FormatAnsiCode(code);
			}
			output << " ";
		};
		output << StringifyCategory(category);
		if (logContext.EnableAnsi) {
			output << " " << FormatAnsiCode(AnsiCode::Reset) << " ";
		} else {
			output << ": ";
		};

		output << message << " ";
		if (logContext.EnableAnsi) output << FormatAnsiCode(AnsiCode::Black) << FormatAnsiCode(AnsiCode::Dim);
		output << GetCurrentTimestamp();
		if (logContext.EnableAnsi) output << FormatAnsiCode(AnsiCode::Reset);

		GetOutputStream(priority) << output.str() << std::endl;
	}

	std::string StringifyPriority(SDL_LogPriority priority) {
		switch (priority) {
		case SDL_LOG_PRIORITY_TRACE:
			return "trace";
		case SDL_LOG_PRIORITY_DEBUG:
			return "debug";
		case SDL_LOG_PRIORITY_INFO:
			return "info";
		case SDL_LOG_PRIORITY_WARN:
			return "warn";
		case SDL_LOG_PRIORITY_ERROR:
			return "error";
		case SDL_LOG_PRIORITY_CRITICAL:
			return "critical";
		case SDL_LOG_PRIORITY_COUNT:
			return "count";
		default:
			return "invalid";
		}
	}

	void JsonOutput(void *contextUserdata, int category, SDL_LogPriority priority, const char *message) {
		using json = nlohmann::json;
		auto logContext = static_cast<LogContext>(contextUserdata);

		json output;
		output["category"] = StringifyCategory(category);
		if (auto subcategory = StringifySubcategory(category); subcategory.has_value()) {
			output["subcategory"] = subcategory;
		}
		output["level"] = StringifyPriority(priority);
		output["message"] = message;

		GetOutputStream(priority) << output.dump() << std::endl;
	}

	SDL_LogOutputFunction GetLogOutputFunction(LogContext *logContext) {
		if (logContext->EnablePretty) {
			return PrettyOutput;
		} else {
			return JsonOutput;
		}
	}

	void OutputLog(void *_unused, int category, SDL_LogPriority priority, const char *message) {
		std::ostringstream log;

		switch (category) {
		case SDL_LOG_CATEGORY_APPLICATION:
			log << "Gargantuan";
			break;
		default:
			log << "SDL";
			break;
		}

		log << "[";
		switch (priority) {
		case SDL_LOG_PRIORITY_CRITICAL:
			log << "Critical";
			break;
		case SDL_LOG_PRIORITY_ERROR:
			log << "Error";
			break;
		case SDL_LOG_PRIORITY_WARN:
			log << "Warn";
			break;
		case SDL_LOG_PRIORITY_INFO:
			log << "Info";
			break;
		case SDL_LOG_PRIORITY_DEBUG:
			log << "Debug";
			break;
		case SDL_LOG_PRIORITY_VERBOSE:
			log << "Verbose";
			break;
		case SDL_LOG_PRIORITY_TRACE:
			log << "Trace";
			break;
		case SDL_LOG_PRIORITY_COUNT:
			log << "Count";
			break;
		case SDL_LOG_PRIORITY_INVALID:
			log << "Invalid";
			break;
		}
		log << "] ";
		log << message;

		auto &output = priority >= SDL_LOG_PRIORITY_ERROR ? std::cerr : std::cout;
		output << log.str() << std::endl;
	};
}
