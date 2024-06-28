
#include "helpers/point.h"
#include <list>
#include <string>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <unordered_set>

#ifndef _GAME_SETTINGS_H_
#define _GAME_SETTINGS_H_

enum Mode {
    MINING,
    CONQUEST,
    ELIMINATION
};

enum SpecialistType : unsigned int;

class GameSettings
{
public:
    GameSettings() { loadDefaults(); }

    double simulationSpeed;
    double factoryDensity;
    double resourceReductionAmount;
    int resourcesToWin;
    int outpostsToWin;
    Mode gameMode;
    int eloKValue;
    int defaultSonar;
    int defaultMaxShield;
    int baseFireRate; // immutable
    double fireRange;
    double fireRate;
    int costPerMine;
    int outpostsPerPlayer;
    int width; // immutable
    int height; // immutable
    double startTime; // immutable
    std::unordered_map<SpecialistType, std::string> specialistDescriptions;
    std::vector<std::tuple<double, double, double>> playerColors;
    std::vector<std::string> outpostNames;
    std::unordered_set<int> activeHours;
    std::unordered_set<SpecialistType> specialistBans;
    int number_of_teams;
    int giftPadding;

    void loadDefaults();

    void addSetting(const char* type, const void* value);

    double gameToClientTime(double timestamp) const;

    double clientToGameTime(double timestamp) const;

    bool clientIsPaused(double timestamp) const;
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