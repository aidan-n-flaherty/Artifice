#include "point.h"
#include "../game_settings.h"

double Point::distance(const Point &other) const {
    return sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
}

Point Point::normalized(double val) {
    double dist = distance(Point(0, 0));
    return dist == 0 ? Point(0, 0) : Point(getX() * val/dist, getY() * val/dist);
}

Point Point::movedTowards(const Point &other, double distance) const {
    Point p(x, y);
    p += (p.closest(other) - p).normalized(distance);

    p.constrain();

    return p;
}

void Point::moveTowards(const Point &other, double distance) {
    *this += (closest(other) - *this).normalized(distance);

    constrain();
}

void Point::constrain() {
    while(x >= GameSettings::width) x -= GameSettings::width;
    while(x < 0) x += GameSettings::width;
    while(y >= GameSettings::height) y -= GameSettings::height;
    while(y < 0) y += GameSettings::height;
}

Point Point::closest(const Point &other) const {
    Point selected = other;

    double minDist = distance(other);

    for(int i = 0; i < 3; i++) {
        Point point(other.getX() + copysignf(GameSettings::width, getX() - other.getX()),
                     other.getY() + copysignf(GameSettings::height, getY() - other.getY()));
        
        double newDist = distance(point);
        if(newDist < minDist) {
            minDist = newDist;
            selected = point;
        }
    }

    return selected;
}
