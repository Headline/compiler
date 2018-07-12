#ifndef H_DEBUG // double include protection
#define H_DEBUG

/*
 * Platform dependent definitions
 */
#if defined __WIN32 || defined __WIN64
#define PLATFORM_WINDOWS
#endif

#if defined __APPLE__
#define PLATFORM_APPLE
#endif

#if defined __linux__
#define PLATFORM_LINUX
#endif

#define COMPILER_VERSION "$$version$"

/* Forces the test file test.ds to be used when parameterless, this is 
 * so things play nice with vs debugger
 */
//#define DEFAULT_TEST_FILE

/* Global debug which enables all debug printing across the compiler */
//#define DEBUG_ALL

/* Individual debug information for every piece of the compiler. 
 * Uncomment one to allow debug prints for it's critical operations
 */

///#define SCANNER_DEBUG
///#define TOKENIZER_DEBUG
///#define PARSER_DEBUG

#ifdef DEBUG_ALL
	#define SCANNER_DEBUG
	#define TOKENIZER_DEBUG
	#define PARSER_DEBUG
#endif // DEBUG_ALL


#endif // H_DEBUG