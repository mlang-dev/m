
enum ID {
    ID1,
    ID2,
    ID3
};

#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);

EXPORT int get_id(enum ID id){
    int ret_id;
    switch(id){
        case ID1:
            ret_id = 100;
            break;
        case ID2:
            ret_id = 200;
            break;
        default:
            ret_id = 300;
            break;
    }
    return ret_id;
}

EXPORT void _start()
{
    print("hello: %d", get_id(ID2));
}
// struct Point2D {
//     double x;
//     double y;
// };

// struct Point2D f()
// {
//     struct Point2D xy = { 10.0, 20.0 };
//     return xy;
// }

// struct Point2D f()
// {
//     struct Point2D point = { 10.0, 20.0 };
//     return point;
// }
