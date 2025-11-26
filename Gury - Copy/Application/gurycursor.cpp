
#include <G3D/GImage.h>

#include "gurycursor.h"

HCURSOR CreateCursorFromBitmap(HBITMAP hBitmap, int xHotspot, int yHotspot) {
    ICONINFO iconInfo = { 0 };

    iconInfo.fIcon = FALSE;
    iconInfo.xHotspot = xHotspot;
    iconInfo.yHotspot = yHotspot;
    iconInfo.hbmMask = hBitmap;
    iconInfo.hbmColor = hBitmap;

    return CreateIconIndirect(&iconInfo);
}

HBITMAP RBX::cursify::pngToBitmap(std::string pngPath)
{
    GImage image(pngPath);
    BinaryOutput o;

    image.encode(GImage::BMP, o);

    return CreateBitmap(image.width, image.height, 1, 24, image._byte);
}

HCURSOR RBX::cursify::createCursor(std::string cursorPathPng)
{
    return CreateCursorFromBitmap(pngToBitmap(cursorPathPng), 0, 0);
}
