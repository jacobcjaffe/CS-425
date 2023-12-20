
#ifndef __COLOR_H__
#define __COLOR_H__

#define HOST     __host__
#define DEVICE   __device__
#define BOTH     HOST DEVICE

struct Color {
    Byte r;
    Byte g;
    Byte b;

    Color() = default;

    BOTH Color(Byte r, Byte g, Byte b) : r(r), g(g), b(b) 
        { /* Empty */ }

    BOTH Color(Byte grey) : r(grey), g(grey), b(grey)
        { /* Empty */ }
};

#undef BOTH
#undef HOST
#undef DEVICE

#endif // __COLOR_H__