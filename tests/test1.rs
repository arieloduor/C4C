
native "C":
    fn putchar(i32 c)->i32
    fn exit(i32 status)->i32
:

fn add(i32 x,i32 y)->i32:
    return x + y
:

i32 character = 0

