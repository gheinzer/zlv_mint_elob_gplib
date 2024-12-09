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
	ERR_INVALID_STR
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
 * @brief Rudimentary try-catch-implementation in C.
 * 
 * This is an example of the usage of this block:
 * @code{.c}
 * try {
 *     // Put some error-prone code here
 * 	   // Throw an error using the throw or the throwMessage macro
 * } catch {
 *     // Error handler
 *     // Called if the above error handler has not caught anything
 * }
 * @endcode
 * 
 * If you want to process the error code further,
 * you can use the @ref catchErrorCode macro:
 * @code{.c}
 * try {
 *     // Put some error-prone code here
 * 	   // Throw an error using the throw or the throwMessage macro
 * } catchErrorCode(error_t error) {
 *     // Error handler
 *     // The error code is now saved in the variable 'error'.
 * }
 * @endcode
 * 
 * In the same way, you can use @ref catchErrorName and @ref catchErrorMessage
 * @code{.c}
 * try { } catchErrorName(error_t error, const char* errorName) { }
 * // or
 * try { } catchErrorMessage(error_t error, const char* errorName, const char* errorMessage) { }
 * @endcode
 * 
 * If you don't wrap error-prone code in a try-catch wrapper, thrown errors
 * are caught globally and the execution of your application stops. The only way to resume the execution of the 
 * program in such a case is resetting the microcontroller.
 * 
 * This implementation should be safe to use multiple times in the same scope. Nested
 * try-catch blocks are also allowed.
 * 
 * Note that any errors thrown in the `catch` block are handled by the higher-level error handler, i.e. a
 * higher-level try-catch-block or the global error handler. Any error thrown in the `catch` block are
 * handled the same way as errors thrown right outside the `catch` block.
 * 
 * Any code after `throw` or `throwMessage` is unreachable, i.e. will never by executed.
 * 
 * Under the hood, this works with the global @ref _error_h_currentJmpBuf variable. It always stores
 * a pointer to the the `jmp_buf` of the currently active try-catch-block, or the `jmp_buf` of the global
 * error handler. If you now create a new try-catch-block, @ref _error_h_currentJmpBuf is updated while
 * storing the previous value in `_error_h_previousJmpBuf`. If you now close the block with the `catch`
 * macro, @ref _error_h_currentJmpBuf is reset to the previous state and any further errors go to the
 * higher-level try-catch-block (or the global error handler if no higher-level try-catch-block exists).
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
 * @brief Catch macro used for try-catch block implementation. Please refer to the documentation
 * of @ref try for more information.
 * 
 */
#define catch \
	_closeTry \
	if(_error_h_setjmpReturnValue)

/**
 * @brief Catch macro used for try-catch block implementation. Please refer to the documentation
 * of @ref try for more information.
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
 * @brief Catch macro used for try-catch block implementation. Please refer to the documentation
 * of @ref try for more information.
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
 * @brief Catch macro used for try-catch block implementation. Please refer to the documentation
 * of @ref try for more information.
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
 * 
 * @param error The error code you want to throw.
 */
#define throw(error) _error_h_throw(error, #error)

/**
 * @brief Throws an error with the specified error code and message.
 * 
 * @param error The error code you want to throw.
 * @param msg The error message you want to throw.
 */
#define throwMessage(error, msg) _error_h_throwWithMessage(error, #error, msg)

/**
 * @brief Initializes the error library. Call this before using any functions
 * or macros of this library.
 */
void error_init();

#endif /* ERROR_H_ */