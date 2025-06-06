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
            ConvexArea *recint;
            ImageCoordinate p0;            

        public:
            Rectangle(ImageCoordinate c, int widthPx, int heightPx);
            ~Rectangle();            
            std::vector<ImageLine> getEdges();
            ImageCoordinate getOrigin();
            void scale(float scaleFactor);
            ImageCoordinate getMinImageCoordinate();
            ImageCoordinate getMaxImageCoordinate();
        };

        class ConvexArea
        {
        private:
            std::vector<ImageCoordinate> vertices;
            void checkConvexity();

        public:
            ConvexArea(const std::vector<ImageCoordinate> &vertices);
            void scale(float scaleFactor);
            bool containsPoint(const ImageCoordinate &p) const;
            Rectangle getBoundingBox();
            std::vector<ImageCoordinate> getVertices();
            std::vector<ImageLine> getEdges(); 
            ImageCoordinate getMinImageCoordinate();
            ImageCoordinate getMaxImageCoordinate();
        };
    }
}

#endif
