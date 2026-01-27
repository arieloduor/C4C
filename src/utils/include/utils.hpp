#ifndef C4C_UTILS_H
#define C4C_UTILS_H


#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <optional>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <cfloat>
#include <cmath>
#include <assert.h>


#ifndef DEBUG_PRINT
#define DEBUG_PRINT(x,y) std::cout << (x) << (y) << std::endl
#endif


#ifndef DEBUG_PANIC
#define DEBUG_PANIC(x) DEBUG_PRINT((x),""); std::exit(5)
#endif

#endif
