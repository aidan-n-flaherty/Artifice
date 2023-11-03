#ifndef _POINT_H_
#define _POINT_H_

#include <cstdlib>
#include <cmath>
#include "../game_settings.h"

class Point
{
private:
    double x = -1;
    double y = -1;

    bool invalid = false;

    GameSettings* settings = nullptr;

    double distance(const Point &other) const;

public:
    Point() : x(-1), y(-1), invalid(true) {};
    Point(GameSettings* settings, double x, double y) : settings(settings), x(x), y(y), invalid(false) { };

    bool isInvalid() { return invalid; }

    double getX() const { return x; }

    double getY() const { return y; }

    void setX(double x) { this->x = x; constrain(); }

    void setY(double y) { this->y = y; constrain(); }
    
    void set(double x, double y) { this->x = x; this->y = y; constrain(); }

    double closestDistance(const Point &other) const;

    Point closest(const Point &other) const;

    void moveTowards(const Point &other, double distance);

    Point movedTowards(const Point &other, double distance) const;

    void constrain();

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
         return Point(settings, x + other.x, y + other.y);
    }

    Point operator-(const Point &other) {
         return Point(settings, x - other.x, y - other.y);
    }

    double operator*(const Point &other) {
         return x * other.x + y * other.y;
    }

    Point operator*(double scalar) {
         return Point(settings, x * scalar, y * scalar);
    }

    bool operator==(const Point &other) {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point &other) {
        return x != other.x || y != other.y;
    }
};

#endif