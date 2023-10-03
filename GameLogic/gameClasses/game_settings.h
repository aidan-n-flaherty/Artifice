
#include "helpers/point.h"
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>
#include "gameObjects/specialist.h"

#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

enum Mode {
    MINING,
    CONQUEST
};

class GameSettings
{
private:
    static bool tmp;

public:
    static int currentlySimulating;
    static double simulationSpeed;
    static int resourcesToWin;
    static Mode gameMode;
    static int eloKValue;
    static int defaultSonar;
    static int defaultMaxShield;
    static int fireRate;
    static int costPerMine;
    static int width;
    static int height;
    static std::unordered_map<SpecialistType, std::string> specialistDescriptions;
    static std::vector<std::tuple<double, double, double>> playerColors;

    static void reset();

    static bool reset(int gameID) {
        if(gameID != currentlySimulating) {
            currentlySimulating = gameID;
            reset();

            return true;
        }

        return false;
    }
};

#endif