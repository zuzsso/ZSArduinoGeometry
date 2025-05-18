#ifndef __ZS_GEOMETRY__
#define __ZS_GEOMETRY__

#include <zs_serial_port_utils.h>
#include <vector>

namespace ZS
{
    namespace ImaGeometry
    {
        class ImageCoordinate
        {
        private:
            int x;
            int y;

        public:
            ImageCoordinate(int x, int y) : x(x), y(y) {};
            void scale(float factor);
            int getX() const;
            int getY() const;
        };

        class ImageLine
        {
        private:
            ImageCoordinate start;
            ImageCoordinate end;

        public:
            ImageLine(ImageCoordinate start, ImageCoordinate end) : start(start), end(end) {};

            void scale(double factor);
        };

        class Circle
        {
        private:
            ZS::ImaGeometry::ImageCoordinate center;
            int radiusPx;

        public:
            Circle(ZS::ImaGeometry::ImageCoordinate center, int radiusPx) : center(center), radiusPx(radiusPx) {};
            ZS::ImaGeometry::ImageCoordinate getCenter();
            int getRadiusPx();
        };

        class ConvexArea
        {
        private:
            std::vector<ImageCoordinate> vertices;

            void checkConvexity();

        public:
            ConvexArea(std::vector<ImageCoordinate> &vertices) : vertices(vertices) {};

            void scale(float scaleFactor);

            bool containsPoint(const ImageCoordinate &p) const;

            Rectangle getBoundingBox();
        };

        class Rectangle
        {
        private:
            ZS::ImaGeometry::ConvexArea *myRect;
            ImageCoordinate c;
            int widthPx;
            int heightPx;

        public:
            Rectangle(ImageCoordinate c, int widthPx, int heightPx) : c(c), widthPx(widthPx), heightPx(heightPx) {};
            ~Rectangle();
            ImageCoordinate getMinImageCoordinate();
            ImageCoordinate getMaxImageCoordinate();
        };
    }
}

#endif
