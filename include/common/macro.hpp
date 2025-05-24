#ifndef MACRO_H
#define MACRO_H

/* Print debug information */
#ifdef DEBUG
#define printDebug(...) \
    do { \
        std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " (" << __func__ << "): "; \
        (std::cout << ... << __VA_ARGS__) << std::endl; \
    } while (0)
#else
#define printDebug(...)  // Do nothing
#endif

#endif //MACRO_H
