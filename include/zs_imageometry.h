#ifndef __ZS_GEOMETRY__
#define __ZS_GEOMETRY__

#include <zs_serial_port_utils.h>
#include <vector>

namespace ZS
{
    namespace ImaGeometry
    {
        class ConvexArea;

        class ImageCoordinate
        {
        private:
            int x;
            int y;

        public:
            ImageCoordinate(int x, int y);
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
            ImageLine(ImageCoordinate start, ImageCoordinate end);
            ImageCoordinate getStart();
            ImageCoordinate getEnd();

            void scale(double factor);
        };

        class Circle
        {
        private:
            ImageCoordinate center;
            int radiusPx;

        public:
            Circle(ImageCoordinate center, int radiusPx);
            ImageCoordinate getCenter();
            int getRadiusPx();
        };

        class Rectangle
        {
        private:
            ConvexArea *myRect;
            ImageCoordinate p0;
            int widthPx;
            int heightPx;

        public:
            Rectangle(ImageCoordinate c, int widthPx, int heightPx);
            ~Rectangle();
            ImageCoordinate getMinImageCoordinate();
            ImageCoordinate getMaxImageCoordinate();
            std::vector<ImageLine> getEdges();
            ImageCoordinate getOrigin();
        };

        class ConvexArea
        {
        private:
            std::vector<ImageCoordinate> vertices;
            void checkConvexity();

        public:
            ConvexArea(std::vector<ImageCoordinate> &vertices);
            void scale(float scaleFactor);
            bool containsPoint(const ImageCoordinate &p) const;
            Rectangle getBoundingBox();
        };
    }
}

#endif
