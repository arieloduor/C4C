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
    PTR,
};



class PointerType
{
public:
    bool is_ptr = false;
    DataType base_type;
    int ptr_no = 0;
    //bool is_set = false;

    void set(bool is_ptr,DataType base_type,int ptr_no)
    {
        this->is_ptr = is_ptr;
        this->base_type = base_type;
        this->ptr_no = ptr_no;
        //this->is_set = true;
    }
};


#endif