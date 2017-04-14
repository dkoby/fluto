/*
 *
 */
#define MAJOR  0
#define MINOR  0
#define BUILD  1

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

#define VERSION_STRING QUOTEME(MAJOR.MINOR.BUILD)

    
/*
 * TODO
 *     * port to windows
 *     * websocket support
 *
 * 20161223 v0.0.1 initial version
 */

