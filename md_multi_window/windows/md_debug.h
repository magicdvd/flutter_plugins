#ifndef MD_DEBUG_LOG_H_
#define MD_DEBUG_LOG_H_

#include <iostream>

#define MD_DEBUG //开启调试，注释后则关闭

#ifdef MD_DEBUG
#define DEBUG_LOG(x) std::cout << "[DEBUG] " << x << std::endl
#else
#define DEBUG_LOG(x)
#endif

#endif  // MD_DEBUG_LOG_H_