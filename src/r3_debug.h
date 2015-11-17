#ifndef R3_DEBUG_H
#define R3_DEBUG_H 

// #define DEBUG 1
#ifdef DEBUG

#define info(fmt, ...) \
            do { fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define debug(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#else
#define info(...);
#define debug(...);
#endif

#endif /* !DEBUG_H */
