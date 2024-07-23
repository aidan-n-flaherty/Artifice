#include "game_settings.h"
#include "string"
#include "unordered_map"
#include <tuple>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "gameObjects/specialist.h"

void GameSettings::loadDefaults() {
    GameSettings::simulationSpeed = 60 * 60; // each hour is a second
    GameSettings::factoryDensity = 0.5;
    GameSettings::resourcesToWin = 200;
    GameSettings::outpostsToWin = -1;
    GameSettings::gameMode = MINING;
    GameSettings::eloKValue = 32;
    GameSettings::defaultSonar = 60;
    GameSettings::defaultMaxShield = 20;
    GameSettings::baseFireRate = 2 * 60 * 60;
    GameSettings::fireRate = 1.0;
    GameSettings::fireRange = 0.5;
    GameSettings::costPerMine = 50;
    GameSettings::outpostsPerPlayer = 10;
    GameSettings::width = 200;
    GameSettings::height = 200;
    GameSettings::number_of_teams = -1;
    GameSettings::startTime = -1;
    GameSettings::resourceReductionAmount = 0.2;
    GameSettings::activeHours.clear();
    for(int i = 0; i < 24; i++) GameSettings::activeHours.insert(i);
    GameSettings::specialistBans.clear();

    GameSettings::specialistDescriptions = {
        { QUEEN, "If you lose your Queen, you lose the game. Maximum shield charge of the Queen's outpost is increased by 20. All hired specialists spawn at the Queen's outpost, and cannot be hired in transit. If you acquire another Queen, she becomes a Princess."},
        { PRINCESS, "The sonar range of the local outpost is increased by 50% of the base value for the first Princess, 25% for the second Princess, etc. If you lose your Queen, the nearest Princess will replace her."},
        { PIRATE, "A Pirate can target an enemy submarine in transit. Movement speed is 2x the base value. Upon a successful attack, the Pirate returns at 4x base movement speed to the nearest friendly outpost."},
        { ADMIRAL, "Movement speed is 2x the base value. Increases speed of all your subs that aren't carrying specialists by 50% for the first Admiral, 25% for the next Admiral, etc." },
        { NAVIGATOR, "A submarine carrying a Navigator can be redirected during transit." },
        { ASSASSIN, "Kills any enemy specialists it encounters in combat." },
        { INFILTRATOR, "Drains all shield charge from any outpost it attacks." },
        { LIEUTENANT, "Moves 50% faster than the base movement speed. When directly participating in combat, kill 5 extra enemy units." },
        { THIEF, "15% of the enemy's units (rounded up) are converted to yours when directly participating in combat." },
        { INSPECTOR, "While at an outpost, the local shield is always fully charged." },
        { MARTYR, "When participating in combat, destroy all outposts, submarines, and specialists, allied and enemy alike, within a range of 20% the default sonar value." },
        { REVERED_ELDER, "When directly participating in combat and the opponent does not have another Revered Elder, all other specialist effects are negated (including your own)." },
        { SABOTEUR, "Redirect any victorious enemy submarine to its owner's nearest outpost." },
        { SENTRY, "While at an outpost, remove 5% of units from the highest-unit enemy vessel every 2 hours. Range is 50% of the outpost where the Sentry resides." },
        { SMUGGLER, "When traveling to an outpost that you own, speed is 3 times the base value." },
        { DIPLOMAT, "If any of your specialists are captured and in the sonar range of the diplomat, they are forcibly released." },
        { FOREMAN, "When at a factory, 6 more units are produced." },
        { HELMSMAN, "Movement speed is 2x the base value." },
        { HYPNOTIST, "You take control of any captured specialists at the Hypnotist's outpost." },
        { INTELLIGENCE_OFFICER, "The sonar range of all your outposts are increased by 25% of the base value. All types of outposts outside your sonar range are revealed." },
        { TINKERER, "Maximum unit capacity is increased by 3 times the maximum shield charge at the Tinkerer's outpost's. Local shield decreases by 3 units per hour, per Tinkerer." },
        { ENGINEER, "25% of any of your units lost to combat are regenerated. An additional 25% are regenerated when directly participating in combat." },
        { GENERAL, "Moves 50% faster than the base movement speed. When one of your specialists participates in combat, kills 10 enemy units after specialist phase." },
        { KING, "After specialist phase, kill 1 enemy unit for every 4 units you have. For each stacked King, the number of units required increases by 2. Shields on all outposts are reduced by 20, except for the King's outpost, where it is increased by 20." },
        { MINISTER_OF_ENERGY, "Maximum unit capacity is raised by 300, while production is reduced by 1 every cycle." },
        { SECURITY_CHIEF, "All shield charges are raised by 10, and an additional 10 shield charge is added to the Security Chief's outpost." },
        { TYCOON, "Unit production speed is increased by 50%. When at a factory, 3 additional units are produced." },
        { WAR_HERO, "20 enemy units are killed when the War Hero is participating in battle." }
    };

    GameSettings::playerColors = {
        { 230/255.0, 25/255.0, 75/255.0},
        { 60/255.0, 180/255.0, 75/255.0},
        { 255/255.0, 225/255.0, 25/255.0},
        { 0/255.0, 130/255.0, 200/255.0},
        { 245/255.0, 130/255.0, 48/255.0},
        { 70/255.0, 240/255.0, 240/255.0},
        { 240/255.0, 50/255.0, 230/255.0},
        { 250/255.0, 190/255.0, 212/255.0},
        { 0/255.0, 128/255.0, 128/255.0},
        { 220/255.0, 190/255.0, 255/255.0}
    };
    GameSettings::outpostNames = {
        "Adama",
        "Adain",
        "Adria",
        "Ahab",
        "Amberg",
        "Aranorin",
        "Ararin",
        "Askersund",
        "Atlantis",
        "Atyrau",
        "Bahat",
        "Benthic",
        "Billante",
        "Brackenforge",
        "Brighthold",
        "Calhamer",
        "Cebu",
        "Chongos",
        "Clarkrye",
        "Clarlayna",
        "Conchile",
        "Coralore",
        "Coralville",
        "Corvid",
        "Corvus",
        "Draebanor",
        "Darkpeak",
        "Dunebrook",
        "Dunkel",
        "Ehodran",
        "Enki",
        "Fam",
        "Fate",
        "Feld",
        "Gabler",
        "Gandhi",
        "Garfield",
        "Gomiris",
        "Goss",
        "Hernae",
        "Ianven",
        "Ilythyrra",
        "Iyizarid",
        "Justinia",
        "Jutugia",
        "Kanola",
        "Kiesling",
        "King",
        "Khodour",
        "Kyburz",
        "Lecit",
        "Lomello",
        "Magellan",
        "Magpie",
        "Mazu",
        "McGuire",
        "Mirlenas",
        "Myre",
        "Naija",
        "Nautilus",
        "Nemo",
        "Neptune",
        "Nuxinon",
        "Noel",
        "Oaktown",
        "October",
        "Olomud",
        "Orivae",
        "Paricia",
        "Phillips",
        "Poseidon",
        "Poteca",
        "Proteus",
        "Raven",
        "Riviern",
        "Roberts",
        "Rocs",
        "Ronus",
        "Rokovoko",
        "Savinka",
        "Senusha",
        "Slinate",
        "Sophutria",
        "Suijin",
        "Tamir",
        "Tangaroa",
        "Thunderbird",
        "Tempest",
        "Tiberius",
        "Tometz",
        "Torshavn",
        "Twilight",
        "Vintgar",
        "Vimarile",
        "Votara",
        "Wallace",
        "Warminster",
        "Wilkes",
        "Whuqsu",
        "Wohlwend",
        "Woodridge",
        "Yamato"
    };
}

void GameSettings::addSetting(const char* type, const void* value){
    if(value == nullptr) return;

    if(strcmp(type, "simulationSpeed") == 0) simulationSpeed = std::min(7200.0, std::max(1.0, *(double*)value));
    else if(strcmp(type, "fireRate") == 0) fireRate = std::min(2.0, std::max(0.5, *(double*)value));
    else if(strcmp(type, "fireRange") == 0) fireRange = std::min(1.0, std::max(0.25, *(double*)value));
    else if(strcmp(type, "factoryDensity") == 0) factoryDensity = std::min(0.8, std::max(0.2, *(double*)value));
    else if(strcmp(type, "resourcesToWin") == 0) resourcesToWin = std::min(400, std::max(50, int(std::lround(*(double*)value))));
    else if(strcmp(type, "gameMode") == 0) gameMode = strcmp((char*)value, "CONQUEST") == 0 ? Mode::CONQUEST : strcmp((char*)value, "ELIMINATION") == 0 ? Mode::ELIMINATION : Mode::MINING;
    else if(strcmp(type, "defaultSonar") == 0) defaultSonar = std::min(150, std::max(50, int(std::lround(*(double*)value))));
    else if(strcmp(type, "defaultMaxShield") == 0) defaultMaxShield = std::min(40, std::max(10, int(std::lround(*(double*)value))));
    else if(strcmp(type, "costPerMine") == 0) costPerMine = std::min(100, std::max(25, int(std::lround(*(double*)value))));
    else if(strcmp(type, "outpostsPerPlayer") == 0) outpostsPerPlayer = std::min(20, std::max(5, int(std::lround(*(double*)value))));
    else if(strcmp(type, "number_of_teams") == 0) number_of_teams = int(std::lround(*(double*)value));
    else if(strcmp(type, "resourceReductionAmount") == 0) resourceReductionAmount = std::min(1.0, std::max(0.0, *(double*)value));
    else if(strcmp(type, "activeHours") == 0) {
        std::string s((char*)value);

        if(s != "") {
            activeHours.clear();

            std::istringstream is( s );
            int h;
            while (is >> h) activeHours.insert(abs(h) % 24);
        }
    } else if(strcmp(type, "specialistBans") == 0) {
        std::string s((char*)value);

        if(s != "") {
            specialistBans.clear();

            std::istringstream is( s );
            int typeNum;
            while (is >> typeNum) {
                try {
                    specialistBans.insert(SpecialistType(typeNum));
                } catch(...) {
                    continue;
                }
            }
        }
    }
}

double GameSettings::gameToClientTime(double timestamp) const {
    if(activeHours.size() == 0 || activeHours.size() == 24 || startTime <= 0 || timestamp < startTime) return timestamp;

    double skippedTime = 0;

    int startHour = int(startTime / 3600.0);
    
    if(activeHours.find(startHour % 24) == activeHours.end()) {
        timestamp += (startHour + 1) * 3600 - startTime;
        startHour++;
    }
    
    int endHour = int(timestamp / 3600.0);
    
    int count = (endHour - startHour)/activeHours.size();
    
    for(int i = 0; i < count; i++) {
        skippedTime += 3600 * (24 - activeHours.size());
        endHour += (24 - activeHours.size());
    }

    int latestHour = startHour + ((endHour - startHour)/24) * 24;

    while(latestHour <= endHour) {
        if(activeHours.find(latestHour % 24) == activeHours.end()) {
            skippedTime += 3600;
            endHour++;
        }

        latestHour++;
    }

    return timestamp + skippedTime;
}

double GameSettings::clientToGameTime(double timestamp) const {
    if(activeHours.size() == 0 || activeHours.size() == 24 || startTime <= 0 || timestamp < startTime) return timestamp;

    double skippedTime = 0;

    int startHour = int(startTime / 3600.0);
    int endHour = int(timestamp / 3600.0);

    for(int i = 0; i < (endHour - startHour)/24; i++) skippedTime += 3600 * (24 - activeHours.size());

    int latestHour = startHour + ((endHour - startHour)/24) * 24;

    while(latestHour <= endHour) {
        if(activeHours.find(latestHour % 24) == activeHours.end()) {
            skippedTime += 3600;
        }

        latestHour++;
    }

    if(activeHours.find(startHour % 24) == activeHours.end()) {
        skippedTime -= startTime - startHour * 3600;
    }

    if(activeHours.find(endHour % 24) == activeHours.end()) {
        skippedTime -= (endHour + 1) * 3600 - timestamp;
    }

    return timestamp - skippedTime;
}

bool GameSettings::clientIsPaused(double timestamp) const {
    if(activeHours.size() == 0 || activeHours.size() == 24 || startTime <= 0 || timestamp < startTime) return false;

    return activeHours.find(int(timestamp / 3600.0) % 24) == activeHours.end();
}