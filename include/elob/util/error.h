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
} error_t;

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
 * @brief Internal error name pointer.
 * Do not access this in your application.
 */
const char* _error_h_currentErrorName;
/**
 * @brief Internal error message pointer.
 * Do not access this in your application.
 */
const char* _error_h_currentErrorMessage;
/**
 * @brief Internal error line variable.
 * Do not access this in your application.
 */
int _error_h_currentErrorLine;
/**
 * @brief Internal error file name pointer.
 * Do not access this in your application.
 */
const char* _error_h_currentErrorFile;

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
 * This macro specifically allows the error code to be processed furtherly in a variable.
 * 
 * @param errorCodeVariable Error code variable to write the error code to. This can also be a declaration (i.e. with leading data type).
 */
#define catchErrorCode(errorCodeVariable) \
	_closeTry \
	errorCodeVariable = _error_h_setjmpReturnValue; \
	if(_error_h_setjmpReturnValue)

/**
 * @brief Catch macro used for try-catch block implementation. Please refer to the @ref error-handling section for more information.
 * 
 * This macro specifically allows the error code and the error name to be processed furtherly in a variable.
 * 
 * @param errorCodeVariable Error code variable to write the error code to. This can also be a declaration (i.e. with leading data type).
 * @param errorNameVariable Error name variable to write the error name string pointer to. 
 * The type of this variable has to be `const char*`. This can also be a declaration (i.e. with leading data type).
 */
#define catchErrorName(errorCodeVariable, errorNameVariable) \
	_closeTry \
	errorCodeVariable = _error_h_setjmpReturnValue; \
	errorNameVariable = _error_h_currentErrorName; \
	if(_error_h_setjmpReturnValue)

/**
 * @brief Catch macro used for try-catch block implementation. Please refer to the @ref error-handling section for more information.
 * 
 * This macro specifically allows the error code and the error name to be processed furtherly in a variable.
 * 
 * @param errorCodeVariable Error code variable to write the error code to. This can also be a declaration (i.e. with leading data type).
 * @param errorNameVariable Error name variable to write the error name string pointer to. 
 * The type of this variable has to be `const char*`. This can also be a declaration (i.e. with leading data type).
 * @param errorMessageVariable Error message variable to write the error message string pointer to.
 * The type of this variable has to be `const char*`. This can also be a declaration (i.e. with leading data type).
 */
#define catchErrorMessage(errorCodeVariable, errorNameVariable, errorMessageVariable) \
	_closeTry \
	errorCodeVariable = _error_h_setjmpReturnValue; \
	errorNameVariable = _error_h_currentErrorName; \
	errorMessageVariable = _error_h_currentErrorMessage; \
	if(_error_h_setjmpReturnValue)

/**
 * @brief Thrwos an error with the specified error code.
 * Please refer to the @ref error-handling section for more information.
 * 
 * @param error The error code you want to throw.
 */
#define throw(error) _error_h_throw(error, #error, 0, __LINE__, __FILE__)

/**
 * @brief Throws an error with the specified error code and message.
 * Please refer to the @ref error-handling section for more information.
 * 
 * @param error The error code you want to throw.
 * @param msg The error message you want to throw.
 */
#define throwMessage(error, msg) _error_h_throw(error, #error, msg, __LINE__, __FILE__)

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
 * @brief Internal error throw function. Do not use in your code, use @ref throw (or similar) instead.
 */
void _error_h_throw(unsigned int errorCode, const char* errorName, const char* errorMessage, int line, const char* file);

/**
 * @brief Internal uncaught error handler. Called automatically, do not use in your code.
 */
void _error_h_uncaughtErrorHandler();

#endif /* ERROR_H_ */