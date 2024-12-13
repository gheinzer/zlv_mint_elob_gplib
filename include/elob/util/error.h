/**
 * @file error.h
 * @author Gabriel Heinzer (gabriel.heinzer@roche.com)
 * @brief Error library including a C-Implementation for a try-catch-block.
 */
#ifndef ERROR_H_
#define ERROR_H_

#include <setjmp.h>

/**
 * @brief List of possible errors in the library.
 * 
 * For your application, you can also use macros defining the errors instead. 
 * The error library will process them in the same way.
 * 
 * Notice that one numerical error code should only be used for one
 * error condition. Make sure you're not defining any errors which
 * already have their error number defined in this enumeration.
 */
typedef enum {
	/**
	 * @brief No error placeholder (`setjmp` returns 0 when not called from `longjmp`).
	 * 
	 */
	ERR_NONE = 0,
	/**
	 * @brief Occurs when a too long string is passed.
	 */
	ERR_STR_TOO_LONG,
	/**
	 * @brief Occurs when the passed bitrate is too low.
	 */
	ERR_BITRATE_TOO_LOW,
	/**
	 * @brief Occurs when an I2C transmissen error occurs.
	 */
	ERR_I2C_TRANSMISSION,
	/**
	 * @brief Occurs when a buffer overflow occurs, i.e. if an element is appended to a already full buffer.
	 */
	ERR_BUFFER_OVERFLOW,
	/**
	 * @brief Occurs when a received number is out of range.
	 */
	ERR_OUT_OF_RANGE,
	/**
	 * @brief Occurs when a string with an invalid format is passed.
	 */
	ERR_INVALID_STR,
	/**
	 * @brief Occurs when an invalid unit is passed. 
	 */
	ERR_INVALID_UNIT,
	/**
	 * @brief Occurs when the specified prefix can't possibly represent
	 * the number withuot violating the precision.
	 */
	ERR_PRECISION_VIOLATED
} ErrorCode_t;

/**
 * @brief Struct containing all error information.
 * 
 */
typedef struct {
	unsigned int code;
	const char* name;
	const char* message;
	const char* file;
	const char* functionName;
	unsigned int line;
} Error_t;

/**
 * @brief Internal pointer used for the setjmp library.
 * Do not acecss this in your application.
 */
jmp_buf* _error_h_currentJmpBuf;
/**
 * @brief Internal return value buffer.
 * Do not access this in your application.
 */
int _error_h_setjmpReturnValue;

/**
 * @brief Internal current error value.
 * Do no access this directly in your application. 
 */
Error_t _error_h_currentError;

/**
 * @brief Rudimentary try-catch-implementation in C.
 * 
 * Please refer to the @ref error-handling section for more information.
 */
#define try \
	{ \
		jmp_buf _error_h_localJmpBuf; \
		jmp_buf* _error_h_previousJmpBuf; \
		_error_h_previousJmpBuf = _error_h_currentJmpBuf; \
		_error_h_currentJmpBuf = &_error_h_localJmpBuf; \
		int _error_h_setjmpResult = setjmp(_error_h_localJmpBuf);\
		if(!_error_h_setjmpResult)

/**
 * @brief Internal macro for closing the `try` block.
 * Do not use this directly in your application. Use @ref catch instead.
 */
#define _closeTry \
		_error_h_setjmpReturnValue = _error_h_setjmpResult; \
		_error_h_currentJmpBuf = _error_h_previousJmpBuf; \
	}

/**
 * @brief Catch macro used for try-catch block implementation. Please refer to the @ref error-handling section for more information.
 * 
 */
#define catch \
	_closeTry \
	if(_error_h_setjmpReturnValue)

/**
 * @brief Catch macro used for try-catch block implementation. Please refer to the @ref error-handling section for more information.
 * 
 * This macro specifically allows the error information to be processed further.
 * 
 * @param errorVariable Error code variable to write the error struct to. This has to be a @ref Error_t variable.
 */
#define catchError(errorVariable) \
	_closeTry \
	errorVariable = _error_h_currentError; \
	if(_error_h_setjmpReturnValue)

/**
 * @brief Thrwos an error with the specified error code.
 * Please refer to the @ref error-handling section for more information.
 * 
 * @param error The error code you want to throw.
 */
#define throw(error) _error_h_throw(error, #error, 0, __LINE__, __FILE__, __FUNCTION__)

/**
 * @brief Throws an error with the specified error code and message.
 * Please refer to the @ref error-handling section for more information.
 * 
 * @param error The error code you want to throw.
 * @param msg The error message you want to throw.
 */
#define throwMessage(error, msg) _error_h_throw(error, #error, msg, __LINE__, __FILE__, __FUNCTION__)

/**
 * @brief Evaluates the specified @p condition and throws an @p error if the condition is `false`.
 * 
 * @param condition The condition to evaluate before throwing an error.
 * @param error The error code you want to throw. 
 */
#define assert(condition, error) if(!(condition)) { throw(error); }

/**
 * @brief Evaluates the specified @p condition and throws an @p error with a @p message if the condition is `false`.
 * 
 * @param condition The condition to evaluate before throwing an error.
 * @param error The error code you want to throw.
 * @param message The error message you want to pass with the error.
 */
#define assertMessage(condition, error, message) if(!(condition)) { throwMessage(error, message); }

/**
 * @brief Initializes the error library. Call this before using any functions
 * or macros of this library.
 * 
 * Please refer to the @ref error-handling section for more information.
 */
void error_init();

/**
 * @brief Re-throw an error struct. This can be used in a @ref catchError block to re-throw the error.
 * 
 * @param errorStruct The @ref Error_t struct containing the error information.
 */
void error_rethrow(Error_t errorStruct);

/**
 * @brief Prints the given error information to stderr.
 * 
 * @param errorStruct The @ref Error_t struct containing the error information.
 */
void error_print(Error_t errorStruct);

/**
 * @brief Internal error throw function. Do not use in your code, use @ref throw (or similar) instead.
 */
void _error_h_throw(unsigned int errorCode, const char* errorName, const char* errorMessage, int line, const char* file, const char* functionName);

/**
 * @brief Internal uncaught error handler. Called automatically, do not use in your code.
 */
void _error_h_uncaughtErrorHandler();

#endif /* ERROR_H_ */