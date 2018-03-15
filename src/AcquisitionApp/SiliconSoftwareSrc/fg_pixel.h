#ifndef FG_PIXEL_H
#define FG_PIXEL_H

#include <stdint.h>
#include <fgrab_define.h>
#include <stdexcept>

size_t bytesPerPixel(uint32_t format){

    switch(format){
    case FG_GRAY:	return 1;
    case FG_GRAY16:	return 2;
    case FG_COL24:	return 3;
    case FG_COL32:	return 4;
    case FG_COL30:	return 5;
    case FG_COL48:	return 6;
    default: throw std::invalid_argument("Pixel type is not known");
    }
}

// returns the bit with according to the selected image format
int bitsPerPixel(const int format)
{
    switch(format){
    case FG_GRAY: return 8;
    case FG_GRAY16: return 16;
    case FG_COL24:
    case FG_COL48: return 24;
    default: throw std::invalid_argument("Pixel type is not known");
    };
}
#endif