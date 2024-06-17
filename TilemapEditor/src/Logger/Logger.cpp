#include "Logger/Logger.h"

std::shared_ptr<spdlog::logger> Logger::mLogger;

void Logger::Init()
{
	/*
		Set the format of the logger messages
		[%T] time stamp
		%n time stamp
	*/

	// Set the Logger Patterns
	spdlog::set_pattern( "%^[%T] %n: %v%$" );
	mLogger = spdlog::stdout_color_mt( "TM_EDITOR" );
	mLogger->set_level( spdlog::level::trace );
}
