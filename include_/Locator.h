
#include <map>
#include <vector>
#include <iostream>

#include "Point.h"
#include "Pose.h"

class Locator {
public:

    Locator(float fieldLength, float fieldWidth);

    void run(std::vector<Pose> poses, double heading);
    void print();

    Point getPos() const {
        return _pos;
    }

    bool newPos(bool set = false) {
        if (_newPos) {
            _newPos = set;
            return true;
        }
        return false;
    }

    Pose getTagPose() const {
        return _t1Pose;
    }

    void getTagPoses(Pose& t1, Pose& t2) const {
        t1 = _t1Pose;
        t2 = _t2Pose;
    }

private:

    std::map<int, Point> _tagPoints;

    Pose _t1Pose;
    Pose _t2Pose;

    Point _pos = {0.0, 0.0};
    bool _newPos;

    float _fieldWidth;
    float _fieldLength;

    bool triangulate(std::vector<Pose> poses);
    bool calculate(std::vector<Pose> poses, double headingRad);

};