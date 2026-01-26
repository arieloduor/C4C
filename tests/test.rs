@include "include_test/test1.rs" /* The include_test folder was created to test for absolute path finding*/

pub fn main()->i32:
    i32 character = 65
    while character <= 120:
        putchar(character)
        character = character + 1
    :
   
    exit(200)
    putchar(10)

    exit(2)
    return 10
:
