
#include "helpers/point.h"
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>

#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

enum Mode {
    MINING,
    CONQUEST
};

enum SpecialistType : unsigned int;

class GameSettings
{
public:
    GameSettings() { loadDefaults(); }

    double simulationSpeed;
    double factoryDensity;
    int resourcesToWin;
    Mode gameMode;
    int eloKValue;
    int defaultSonar;
    int defaultMaxShield;
    int fireRate;
    int costPerMine;
    int outpostsPerPlayer;
    int width;
    int height;
    std::unordered_map<SpecialistType, std::string> specialistDescriptions;
    std::vector<std::tuple<double, double, double>> playerColors;

    void loadDefaults();

    /*void reset();

    bool reset(int gameID) {
        if(gameID != currentlySimulating) {
            currentlySimulating = gameID;
            reset();

            return true;
        }

        return false;
    }*/
};

#endif