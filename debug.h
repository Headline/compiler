#ifndef H_DEBUG // double include protection
#define H_DEBUG

/* Forces the test file test.ds to be used when parameterless, this is 
 * so things play nice with vs debugger
 */
#define DEFAULT_TEST_FILE

/* Global debug which enables all debug printing across the compiler */
#define DEBUG_ALL

/* Individual debug information for every piece of the compiler. 
 * Uncomment one to allow debug prints for it's critical operations
 */

///#define SCANNER_DEBUG

#ifdef DEBUG_ALL
	#define SCANNER_DEBUG
#endif // DEBUG_ALL


#endif // H_DEBUG