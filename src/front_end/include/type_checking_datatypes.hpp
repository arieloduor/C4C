#ifndef C4C_TYPE_CHECKING_DATATYPES_H
#define C4C_TYPE_CHECKING_DATATYPES_H

#include <string>
#include <map>
#include "ast.hpp"


enum class DataType
{
    FUNCTION = 1,
    I32,
    I64,
    U32,
    U64,
    F32,
    F64,
};


#endif