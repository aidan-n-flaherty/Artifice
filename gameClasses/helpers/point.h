#ifndef _POINT_H_
#define _POINT_H_

#include <cstdlib>
#include <cmath>

class Point
{
private:
    double x;
    double y;

public:
    Point(){};
    Point(double x, double y) : x(x), y(y) {};

    double getX() const { return x; }

    double getY() const { return y; }

    void setX(double x) { this->x = x; }

    void setY(double y) { this->y = y; }
    
    void set(double x, double y) { this->x = x; this->y = y; }

    double distance(const Point &other) const;

    Point closest(const Point& dimensions, const Point &other) const;

    void constrain(const Point& dimensions);

    Point normalized(double val);

    Point& operator+=(const Point &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point& operator-=(const Point &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Point operator+(const Point &other) {
         return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point &other) {
         return Point(x - other.x, y - other.y);
    }

    double operator*(const Point &other) {
         return x * other.x + y * other.y;
    }
};

#endif