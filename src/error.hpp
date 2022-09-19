#pragma once

#include <ostream>

/**
 * @brief Throws a runtime_error, giving some useful information.  
 * 
 * @param function the function where the error was triggered from.
 * @param file the file where the function is.
 * @param line the line of the error.
 * @param msg a detailed description of the error.
 * 
 * @throws std::runtime_error
 */
void gmlError(const char * function, const char * file, const int line, const char * msg);

/**
 * @brief Writes a warning message to the gmlWarnStream.
 * similar to @a gmlError.
 */
void gmlWarn(const char * function, const char * file, const int line, const char * msg);

/**
 * @brief Sets the Gml Warn Stream.
 * 
 * The default is @a std::cerr.
 */
void setGmlWarnStream(std::ostream &);

/**
 * @brief calls callback if the condition false;
 * @param condition the condition
 * @param callback the callback function, usually its @a gmlError,
 * 
 * @see gmlError
 */
void gmlCheck(bool condition,
                void (*callback)(const char * function, const char * file, const int line, const char * msg),
                const char * function,
                const char * file,
                const int line,
                const char * msg);

/**
 * @brief Similar to @a gmlCheck, but calls the @a orCallback if the condition is true.
 * 
 * @param orCallback the default callback.
 * @param orCallbackArg the default callback argument value.
 */
void gmlCheckOr(bool condition,
                void (*callback)(const char * function, const char * file, const int line, const char * msg),
                const char * function,
                const char * file,
                const int line,
                const char * msg,
                void (*orCallback)(void*),
                void*orCallbackArg);