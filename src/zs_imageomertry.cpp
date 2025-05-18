#include <zs_imageometry.h>
#include <zs_serial_port_utils.h>
#include <vector>

namespace ZS
{
    namespace ImaGeometry
    {

        ImageCoordinate::ImageCoordinate(int x, int y) : x(x), y(y) {};
        void ImageCoordinate::scale(float factor)
        {
            if (factor == 0)
            {
                ZS::SerialPort::writeErrorToSerialPort("Only scale factors != 0 allowed ");
                return;
            }

            this->x = (int)floor(x * factor);
            this->y = (int)floor(y * factor);
        }
        int ImageCoordinate::getX() const
        {
            return this->x;
        }

        int ImageCoordinate::getY() const
        {
            return this->y;
        }

        ImageLine::ImageLine(ImageCoordinate start, ImageCoordinate end) : start(start), end(end)
        {
        }

        void ImageLine::scale(double factor)
        {
            this->start.scale(factor);
            this->end.scale(factor);
        }

        ImageCoordinate ImageLine::getStart()
        {
            return this->start;
        }

        ImageCoordinate ImageLine::getEnd()
        {
            return this->end;
        }

        Circle::Circle(ImaGeometry::ImageCoordinate center, int radiusPx) : center(center), radiusPx(radiusPx)
        {
        }

        ImageCoordinate Circle::getCenter()
        {
            return center;
        }

        int Circle::getRadiusPx()
        {
            return radiusPx;
        }

        Rectangle::Rectangle(ImageCoordinate c, int widthPx, int heightPx) : c(c), widthPx(widthPx), heightPx(heightPx)
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

        Rectangle::~Rectangle()
        {
            delete myRect;
        }

        ImageCoordinate Rectangle::getMinImageCoordinate()
        {
            return c;
        }

        ImageCoordinate Rectangle::getMaxImageCoordinate()
        {
            return ImageCoordinate(c.getX() + widthPx, c.getY() + heightPx);
        }

        void ConvexArea::checkConvexity()
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

        ConvexArea::ConvexArea(std::vector<ImageCoordinate> &vertices) : vertices(vertices)
        {
            if (vertices.size() < 3)
            {
                String s =
                    "At least three vertices are needed to build a convex area. Povided: " + String(vertices.size());
                ZS::SerialPort::runtimeException(s);
            }

            checkConvexity();
        }

        void ConvexArea::scale(float scaleFactor)
        {
            int n = vertices.size();
            for (int i = 0; i < n; i++)
            {
                vertices[i].scale(scaleFactor);
            }
        }

        bool ConvexArea::containsPoint(const ImageCoordinate &p) const
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

        Rectangle ConvexArea::getBoundingBox()
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
    }
}
