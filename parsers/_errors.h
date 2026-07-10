#pragma once

#include <string>
#include <stdexcept>

namespace _err
{
	// Arguments
	const std::string _pointer = "Invalid pointer.";
	const std::string _std_object = "Invalid std object.";
	const std::string _argument = "Invalid argument.";
	const std::string _expression = "Expression is FALSE.";

	// File
	const std::string _file = "Invalid file.";
	const std::string _format = "Invalid format.";
	
	// Instances
	const std::string _instance = "Invalid instance.";

	// Functionality
	const std::string _not_supported = "Not supported.";

	// Unknown
	const std::string _internal = "Internal error.";
};

#define THROW_ERROR(message) \
	throw std::runtime_error(message)

#define VERIFY_EXPRESSION(expr) \
	if (!(expr)) THROW_ERROR(_err::_expression)

#define VERIFY_POINTER(pointer) \
	if (pointer == nullptr) THROW_ERROR(_err::_pointer)

#define VERIFY_INSTANCE(instance) \
	if (instance == 0) THROW_ERROR(_err::_instance)

#define THROW_ARGUMENT_ERROR() \
	THROW_ERROR(_err::_argument)

#define VERIFY_STLOBJ_IS_NOT_EMPTY(v) \
	if (v.empty()) THROW_ERROR(_err::_std_object)

#define VERIFY_STLOBJ_IS_EMPTY(v) \
	if (!v.empty()) THROW_ERROR(_err::_std_object)

#define THROW_NOT_SUPPORTED_ERROR() \
	THROW_ERROR(_err::_not_supported)

#define THROW_INVALID_FORMAT_ERROR() \
	THROW_ERROR(_err::_format)

#define THROW_INTERNAL_ERROR() \
	THROW_ERROR(_err::_internal)

#define LOG_THROW_ERROR(message) \
	logErr(message); \
	throw std::runtime_error(message)

#define LOG_THROW_ERROR_F(message, ...) \
	logErrf(message, __VA_ARGS__); \
	throw std::runtime_error(_string::format(message, __VA_ARGS__))
