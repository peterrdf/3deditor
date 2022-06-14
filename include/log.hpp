#ifndef _WEBGL_SERVER_LOG_HPP_
#define _WEBGL_SERVER_LOG_HPP_

#ifdef _USE_BOOST
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;

// ----------------------------------------------------------------------------
// Log support
class CWebGLServerLog
{

public: // Methods

	// ------------------------------------------------------------------------
	// ctor
	CWebGLServerLog()
	{
	}

	// ------------------------------------------------------------------------
	// Initialization 
	static void Init(const char * szLogFileName)
	{
		logging::add_file_log
		(
			keywords::file_name = szLogFileName,			
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::format = "[%TimeStamp%], [%ThreadID%]: %Message%",
			keywords::auto_flush = true
		);

		logging::core::get()->set_filter
		(
			/*
			* LOG LEVEL
			*/
			logging::trivial::severity >= logging::trivial::debug
		);

		logging::add_common_attributes();
	}

	// ------------------------------------------------------------------------
	// Severity logger
	static src::severity_logger<severity_level> & GetSeverityLogger()
	{
		static src::severity_logger<severity_level> severityLogger;

		return severityLogger;
	}	
};

#define LOG_INIT(logFile) CWebGLServerLog::Init(logFile)
#define LOG_TRACE(message) BOOST_LOG_SEV(CWebGLServerLog::GetSeverityLogger(), trace) << message
#define LOG_DEBUG(message) BOOST_LOG_SEV(CWebGLServerLog::GetSeverityLogger(), debug) << message
#define LOG_INFO(message) BOOST_LOG_SEV(CWebGLServerLog::GetSeverityLogger(), info) << message
#define LOG_WARNING(message) BOOST_LOG_SEV(CWebGLServerLog::GetSeverityLogger(), warning) << message
#define LOG_ERROR(message) BOOST_LOG_SEV(CWebGLServerLog::GetSeverityLogger(), error) << message
#define LOG_FATAL(message) BOOST_LOG_SEV(CWebGLServerLog::GetSeverityLogger(), fatal) << message
#else
#define LOG_INIT(logFile) void(0)
#define LOG_TRACE(message) void(0)
#define LOG_DEBUG(message) void(0)
#define LOG_INFO(message) void(0)
#define LOG_WARNING(message) void(0)
#define LOG_ERROR(message) void(0)
#define LOG_FATAL(message) void(0)
#endif //_USE_BOOST

#endif // _WEBGL_SERVER_LOG_HPP_
