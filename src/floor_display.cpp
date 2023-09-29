#include "floor_display.h"
#include "game_interface.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>
#include "../GameLogic/gameClasses/game.h"
#include "../GameLogic/gameClasses/events/battle_event.h"

using namespace godot;

FloorDisplay::FloorDisplay(GameInterface* game) : game(game) {}

void FloorDisplay::_draw() {
    if(!game || !game->getGame() || !game->getCompleteGame()) return;

    std::shared_ptr<Game> current = game->getGame();
    std::shared_ptr<Game> complete = game->getCompleteGame();

    Node3D* cameraPivot = cast_to<Node3D>(get_parent()->get_parent());
    double x = cameraPivot->get_position().x - 50;
    double y = cameraPivot->get_position().z - 130 + 100;

    /*for(const auto& pair : current->getOutposts()) {
        double x1 = pair.second->getPositionAt(getDiff()).getX();
        double y1 = pair.second->getPositionAt(getDiff()).getY();

        int shield = pair.second->getShieldAt(getDiff());
        int maxShield = pair.second->getMaxShield();

        for(int i = 0; i < maxShield; i += 10) {
            draw_arc(Vector2(x1 - x, y1 - y) * 10, 300 + i * 3, UtilityFunctions::deg_to_rad(135), UtilityFunctions::deg_to_rad(135 + 10 * 18), 32, Color(0.0, 0.0, 0.0, 0.2), 20.0, true);
            draw_arc(Vector2(x1 - x, y1 - y) * 10, 300 + i * 3, UtilityFunctions::deg_to_rad(135), UtilityFunctions::deg_to_rad(135 + std::max(0, std::min(10, shield - i)) * 18), 32, Color(0.0, 0.0, 0.0), 20.0, true);
        }
    }*/

    for(const auto& pair : current->getVessels()) {
        double x1 = pair.second->getPositionAt(getDiff()).getX();
        double y1 = pair.second->getPositionAt(getDiff()).getY();
        double deltaX = pair.second->getTargetPos().getX() - x1;
        double deltaY = pair.second->getTargetPos().getY() - y1;
        double deltaMag = sqrt(deltaX * deltaX + deltaY * deltaY);

        if(pair.second->getOrigin()) draw_line(Vector2(x1 - x, y1 - y) * 10 - 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(pair.second->getOrigin()->getPosition().getX() - x, pair.second->getOrigin()->getPosition().getY() - y) * 10, Color(1.0, 1.0, 1.0, 0.5), 3.0);
        draw_arc(Vector2(x1 - x, y1 - y) * 10, 8, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 1.0, 1.0), 3.0, true);
        draw_line(Vector2(x1 - x, y1 - y) * 10 + 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(pair.second->getTargetPos().getX() - x, pair.second->getTargetPos().getY() - y) * 10, Color(1.0, 1.0, 1.0), 5.0);
    }

    for(const auto& pair : current->getVessels()) {
        double referenceTime = current->getTime();

        for(BattleEvent* battle : current->nextBattles(pair.first)) {
            double x1 = pair.second->getPositionAt(battle->getTimestamp() - referenceTime).getX();
            double y1 = pair.second->getPositionAt(battle->getTimestamp() - referenceTime).getY();
            
            const BattleEvent* simulatedBattle = complete->simulatedBattle(battle->getID());

            Color c;
            if(simulatedBattle && simulatedBattle->getVictor() != -1) {
                if(simulatedBattle->getVictor() == game->getUserGameID()) c = Color(0.0, 0.75, 0.0);
                else c = Color(0.75, 0.0, 0.0);
            } else {
                c = Color(0.75, 0.75, 0.75);
            }

            draw_circle(Vector2(x1 - x, y1 - y) * 10, 10, Color(1.0, 1.0, 1.0));
            draw_circle(Vector2(x1 - x, y1 - y) * 10, 9, c);
            draw_arc(Vector2(x1 - x, y1 - y) * 10, 5, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 1.0, 1.0), 0.5, true);

            for(int i = 45; i < 360; i += 90) {
                double x2 = x1 + 0.3 * cos(UtilityFunctions::deg_to_rad(i)), x3 = x1 + cos(UtilityFunctions::deg_to_rad(i));
                double y2 = y1 + 0.3 * sin(UtilityFunctions::deg_to_rad(i)), y3 = y1 + sin(UtilityFunctions::deg_to_rad(i));

                draw_line(Vector2(x2 - x, y2 - y) * 10, Vector2(x3 - x, y3 - y) * 10, Color(1.0, 1.0, 1.0), 0.5, true);
            }
        }
    }
}