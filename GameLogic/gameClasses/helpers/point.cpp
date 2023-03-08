#include "point.h"

double Point::distance(const Point &other) const {
    return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

Point Point::normalized(double val) {
    double dist = distance(Point(0, 0));
    return dist == 0 ? Point(0, 0) : Point(getX() * val/dist, getY() * val/dist);
}

Point Point::movedTowards(const Point &dimensions, const Point &other, double distance) const {
    Point p(x, y);
    p += (p.closest(dimensions, other) - p).normalized(distance);

    p.constrain(dimensions);

    return p;
}

void Point::moveTowards(const Point &dimensions, const Point &other, double distance) {
    *this += (closest(dimensions, other) - *this).normalized(distance);

    constrain(dimensions);
}

void Point::constrain(const Point &dimensions) {
    while(x >= dimensions.getX()) x -=  dimensions.getX();
    while(x < 0) x += dimensions.getX();
    while(y >= dimensions.getY()) y -= dimensions.getY();
    while(y < 0) y += dimensions.getY();
}

Point Point::closest(const Point &dimensions, const Point &other) const {
    Point selected = other;

    double minDist = distance(other);

    for(int i = 0; i < 3; i++) {
        Point point(other.getX() + copysignf(dimensions.getX(), getX() - other.getX()),
                     other.getY() + copysignf(dimensions.getY(), getY() - other.getY()));
        
        double newDist = distance(point);
        if(newDist < minDist) {
            minDist = newDist;
            selected = point;
        }
    }

    return selected;
}
