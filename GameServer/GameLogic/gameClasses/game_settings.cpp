#include "game_settings.h"
#include "string"
#include "unordered_map"
#include "gameObjects/specialist.h"

double GameSettings::simulationSpeed = 1;
int GameSettings::resourcesToWin = 200;
Mode GameSettings::gameMode = MINING;
int GameSettings::eloKValue = 32;
int GameSettings::defaultSonar = 100;
int GameSettings::defaultMaxShield = 10;
int GameSettings::fireRate = 2 * 60 * 60;
int GameSettings::costPerMine = 50;
int GameSettings::width = 1000;
int GameSettings::height = 1000;

std::unordered_map<SpecialistType, std::string> GameSettings::specialistDescriptions = {
    { QUEEN, "Adds 20 to her outpost's maximum shield charge. If you acquire another Queen, she becomes a Princess. Queen may periodically hire specialists."},
    { PRINCESS, "Increases her outpost's sonar range by 50%. If you lose control of your Queen, the closest Princess becomes the new Queen. "},
    { PIRATE, "Sub carrying Pirate can target another sub. When targeting a sub, travels 2x faster than ordinary subs, then travels at 4x normal speed to nearest friendly outpost."},
    { ADMIRAL, "Increases speed of all your subs that aren't carrying specialists by 50%. Admiral travels 2x faster than ordinary subs." },
    { NAVIGATOR, "Owner may change course of sub carrying Navigator.\nPromotes to: Admiral" },
    { ASSASSIN, "Kills all enemy specialists present when participating in combat." },
    { INFILTRATOR, "Drains 20 from the shield charge of any outpost it attacks." },
    { LIEUTENANT, "Destroys 5 enemy drillers when participating in combat, travels 50% faster than ordinary subs." },
    { THIEF, "Converts 15% of enemy's drillers (rounded up) to your side when attacking an outpost, or in sub to sub combat." },
    { DOUBLE_AGENT, "When participating in sub-to-sub combat, drillers on both subs are destroyed, subs swap ownership along with any specialists aboard, and combat ends." },
    { INSPECTOR, "Fully charges the shields of a friendly outpost upon arrival and after every combat while he's present." },
    { MARTYR, "Destroys all subs and outposts within Martyr's blast radius when participating in combat. Blast radius is 20% of standard sonar range." },
    { REVERED_ELDER, "Local effect: Specialists do not participate in combat, unless subs on both sides carry a Revered Elder" },
    { SABOTEUR, "Redirects enemy sub to its owner's nearest outpost when participating in sub-to-sub combat." },
    { SENTRY, "Fires on an enemy sub once every 2 hours while at an outpost. Each shot destroys 5% of drillers rounded up. Sentry's range is half of its outpost's sonar range. Target is chosen to maximize damage." },
    { SMUGGLER, "Travels 3x faster than ordinary subs while heading for one of its owner's outposts.\nPromotes to: Tycoon" },
    { DIPLOMAT, "Releases all captive specialists you own that are being held at an outpost within Diplomat's outpost's sonar range." },
    { FOREMAN, "Produces 6 additional drillers with each production cycle while at a factory.\nPromotes to: Engineer" },
    { HELMSMAN, "Travels 2x faster than ordinary subs." },
    { HYPNOTIST, "Takes control of all captured specialists present at his outpost.\nPromotes to: King" },
    { INTELLIGENCE_OFFICER, "Adds 25% to all your outposts' sonar range. Shows you the type of all outposts that are outside your sonar range." },
    { TINKERER, "Increases your electrical output by 3 times Tinkerer's outpost's maximum shield charge. Tinkerer's outpost's shield charge is drained at 3 units per hour.\nPromotes to: Minister of Energy" },
    { ENGINEER, "Repairs 25% of the drillers you lose in combat (rounded up) after each combat you win. Additional 25% are repaired after combat taking place where Engineer is present (rounded up)." },
    { GENERAL, "Travels 50% faster than ordinary subs. When one or more of your specialists is present where combat occurs, 10 enemy drillers are destroyed after specialist phase." },
    { KING, "Destroys 1 enemy driller for every 3 of your drillers that remain after specialist phase in every combat you are involved. Reduces max shield charge of all your outposts by 20, except at King's outpost, where it is increased by 20." },
    { MINISTER_OF_ENERGY, "Adds 300 to your electrical output. Your factories produce 1 driller less each production cycle." },
    { SECURITY_CHIEF, "Adds 10 to max shield charge of all your outposts. Adds 10 to max shield charge of Security Chief's outpost." },
    { TYCOON, "Speeds up your driller production rate by 50%. Produces 3 additional drillers with each production cycle while at a factory." },
    { WAR_HERO, "Destroys 20 enemy drillers when participating in combat." }
};