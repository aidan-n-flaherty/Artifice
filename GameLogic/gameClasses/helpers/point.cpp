#include "point.h"
#include "../game_settings.h"
#include <iostream>

double Point::distance(const Point &other) const {
    return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

double Point::closestDistance(const Point &other) const {
    Point o = closest(other);
    return distance(o);
}

Point Point::normalized(double val) {
    double dist = sqrt(x * x + y * y);
    return dist == 0 ? Point(settings, 0, 0) : Point(settings, getX() * val/dist, getY() * val/dist);
}

Point Point::movedTowards(const Point &other, double distance) const {
    Point p(settings, x, y);

    p += (p.closest(other) - p).normalized(distance);

    p.constrain();

    return p;
}

void Point::moveTowards(const Point &other, double distance) {
    *this += (closest(other) - *this).normalized(distance);

    constrain();
}

void Point::constrain() {
    while(x >= settings->width) x -= settings->width;
    while(x < 0) x += settings->width;
    while(y >= settings->height) y -= settings->height;
    while(y < 0) y += settings->height;
}

Point Point::closest(const Point &other) const {
    Point selected = other;

    double minDist = distance(other);

    for(int i = 1; i <= 3; i++) {
        Point point(settings, other.getX() + (i % 2) * copysignf(settings->width, getX() - other.getX()),
                     other.getY() + (i / 2) * copysignf(settings->height, getY() - other.getY()));
        
        double newDist = distance(point);
        if(newDist < minDist) {
            minDist = newDist;
            selected = point;
        }
    }

    return selected;
}
