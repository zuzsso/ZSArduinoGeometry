#include <zs_imageometry.h>
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
            ImageCoordinate(int x, int y) : x(x), y(y)
            {
            }

            void scale(float factor)
            {
                if (factor == 0)
                {
                    ZS::SerialPort::writeErrorToSerialPort("Only scale factors != 0 allowed ");
                    return;
                }

                this->x = (int)floor(x * factor);
                this->y = (int)floor(y * factor);
            }

            int getX() const
            {
                return this->x;
            }

            int getY() const
            {
                return this->y;
            }
        };

        class ImageLine
        {
        private:
            ImageCoordinate start;
            ImageCoordinate end;

        public:
            ImageLine(ImageCoordinate start, ImageCoordinate end) : start(start), end(end)
            {
            }

            void scale(double factor)
            {
                this->start.scale(factor);
                this->end.scale(factor);
            }
        };

        class Circle
        {
        private:
            ZS::ImaGeometry::ImageCoordinate center;
            int radiusPx;

        public:
            Circle(ZS::ImaGeometry::ImageCoordinate center, int radiusPx) : center(center), radiusPx(radiusPx)
            {
            }

            ZS::ImaGeometry::ImageCoordinate getCenter()
            {
                return center;
            }

            int getRadiusPx()
            {
                return radiusPx;
            }
        };

        class Rectangle
        {
        private:
            ZS::ImaGeometry::ConvexArea *myRect;
            ImageCoordinate c;
            int widthPx;
            int heightPx;

        public:
            Rectangle(ImageCoordinate c, int widthPx, int heightPx) : c(c), widthPx(widthPx), heightPx(heightPx)
            {
                int x = c.getX();
                int y = c.getY();
                std::vector<ImageCoordinate> recint = {
                    ImageCoordinate(x, y),
                    ImageCoordinate(x + widthPx, y),
                    ImageCoordinate(x + widthPx, y + heightPx),
                    ImageCoordinate(x, y + heightPx)};

                myRect = new ZS::ImaGeometry::ConvexArea(recint);
            }

            ~Rectangle()
            {
                delete myRect;
            }

            ImageCoordinate getMinImageCoordinate()
            {
                return c;
            }

            ImageCoordinate getMaxImageCoordinate()
            {
                return ImageCoordinate(c.getX() + widthPx, c.getY() + heightPx);
            }
        };

        class ConvexArea
        {
        private:
            std::vector<ImageCoordinate> vertices;

            void checkConvexity()
            {
                int n = vertices.size();

                int sign = 0; // 0 means 'not initialized'

                for (int i = 0; i < n; i++)
                {
                    const ImageCoordinate a = vertices[i];
                    const ImageCoordinate b = vertices[(i + 1) % n];
                    const ImageCoordinate c = vertices[(i + 2) % n];

                    int dx1 = b.getX() - a.getX();
                    int dy1 = b.getY() - a.getY();
                    int dx2 = c.getX() - b.getX();
                    int dy2 = c.getY() - b.getY();

                    int cross = (dx1 * dy2) - (dy1 * dx2);

                    if (cross != 0)
                    {
                        if (sign == 0)
                        {
                            // Initialize
                            sign = (cross > 0) ? 1 : -1;
                        }
                        else
                        {
                            bool differenceCheck1 = (cross > 0) && (sign < 0);
                            bool differenceCheck2 = (cross < 0) && (sign > 0);

                            if (differenceCheck1 || differenceCheck2)
                            {
                                ZS::SerialPort::runtimeException(
                                    "The provided list of vertices do not define a convex area");
                            }
                        }
                    }
                }
            }

        public:
            ConvexArea(std::vector<ImageCoordinate> &vertices) : vertices(vertices)
            {
                if (vertices.size() < 3)
                {
                    String s =
                        "At least three vertices are needed to build a convex area. Povided: " + String(vertices.size());
                    ZS::SerialPort::runtimeException(s);
                }

                checkConvexity();
            }

            void scale(float scaleFactor)
            {
                int n = vertices.size();
                for (int i = 0; i < n; i++)
                {
                    vertices[i].scale(scaleFactor);
                }
            }

            bool containsPoint(const ImageCoordinate &p) const
            {
                int n = vertices.size();

                int sign = 0;
                for (int i = 0; i < n; i++)
                {
                    const ImageCoordinate &a = vertices[i];
                    const ImageCoordinate &b = vertices[(i + 1) % n];

                    int dx1 = b.getX() - a.getX();
                    int dy1 = b.getY() - a.getY();
                    int dx2 = p.getX() - a.getX();
                    int dy2 = p.getY() - a.getY();

                    int cross = dx1 * dy2 - dy1 * dx2;

                    if (cross != 0)
                    {
                        if (sign == 0)
                            sign = (cross > 0) ? 1 : -1;
                        else if ((cross > 0 && sign < 0) || (cross < 0 && sign > 0))
                            return false;
                    }
                }
                return true;
            }

            Rectangle getBoundingBox()
            {
                int minX = INT_MAX;
                int minY = INT_MAX;
                int maxX = INT_MIN;
                int maxY = INT_MIN;
                for (const ImageCoordinate &p : vertices)
                {
                    if (p.getX() < minX)
                    {
                        minX = p.getX();
                    }
                    if (p.getX() > maxX)
                    {
                        maxX = p.getX();
                    }
                    if (p.getY() < minY)
                    {
                        minY = p.getY();
                    }
                    if (p.getY() > maxY)
                    {
                        maxY = p.getY();
                    }
                }

                return Rectangle(ImageCoordinate(minX, minY), maxX - minX + 1, maxY - minY + 1);
            }
        };
    }
}
