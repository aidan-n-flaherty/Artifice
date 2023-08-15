
#include "helpers/point.h"

#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

enum Mode {
    MINING,
    CONQUEST
};

class GameSettings
{
public:
    static double simulationSpeed;
    static int resourcesToWin;
    static Mode gameMode;
    static int eloKValue;
    static int defaultSonar;
    static int defaultMaxShield;
    static int fireRate;
    static int width;
    static int height;
};

#endif