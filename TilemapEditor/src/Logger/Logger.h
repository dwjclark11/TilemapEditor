#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"
#include <memory>

// Wrapper for spdlog
class Logger
{
private: 
	static std::shared_ptr<spdlog::logger> mLogger;
	
public:

	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return mLogger; }
};

#ifdef _DEBUG
	#define LOG_TRACE(...)		Logger::GetLogger()->trace(__VA_ARGS__);
	#define LOG_INFO(...)		Logger::GetLogger()->info(__VA_ARGS__);
	#define LOG_WARN(...)		Logger::GetLogger()->warn(__VA_ARGS__);
	#define LOG_ERROR(...)		Logger::GetLogger()->error(__VA_ARGS__);
	#define LOG_FATAL(...)		Logger::GetLogger()->fatal(__VA_ARGS__);
	#define TM_ASSERT(x, ...) { if (!(x)) {LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }

#elif NDEBUG // If we are not in debug, don't display debug information
	#define LOG_TRACE(...)		
	#define LOG_INFO(...)		
	#define LOG_WARN(...)		
	#define LOG_ERROR(...)		
	#define LOG_FATAL(...)		
	#define TM_ASSERT(x, ...) 
#endif