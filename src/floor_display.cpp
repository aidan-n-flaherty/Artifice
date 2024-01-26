#include "floor_display.h"
#include "game_interface.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>
#include "../GameLogic/gameClasses/game.h"
#include "../GameLogic/gameClasses/events/battle_event.h"

using namespace godot;

FloorDisplay::FloorDisplay(GameInterface* gameInterface) : gameInterface(gameInterface) {}

void FloorDisplay::_draw() {
    if(!gameInterface || !gameInterface->getGame() || !gameInterface->getCompleteGame() || !gameInterface->getCurrentGame()) return;

    std::shared_ptr<Game> game = gameInterface->getGame();
    std::shared_ptr<Game> current = gameInterface->getCurrentGame();
    std::shared_ptr<Game> complete = gameInterface->getCompleteGame();

    SubViewport* viewport = cast_to<SubViewport>(get_parent());
    Node3D* cameraPivot = cast_to<Node3D>(viewport->get_parent());

    int pixels = 10;

    double rootX = cameraPivot->get_position().x - viewport->get_size().x/2.0/pixels;
    double rootY = cameraPivot->get_position().z - viewport->get_size().y/2.0/pixels + 100;

    draw_rect(Rect2(0, 0, viewport->get_size().x, viewport->get_size().y), Color(0.0, 0.0, 0.0, 0.75));

    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            double x = rootX + i * game->getSettings()->width;
            double y = rootY + j * game->getSettings()->height;

            for(const auto& pair : (gameInterface->simulatingFuture() ? current->getOutposts() : game->getOutposts())) {
                if(pair.second->getOwnerID() != gameInterface->getUserGameID()) continue;

                double x1 = pair.second->getPositionAt(getDiff()).getX();
                double y1 = pair.second->getPositionAt(getDiff()).getY();

                draw_circle(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels, Color(0.0, 0.0, 0.0));
            }
        }
    }

    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            double x = rootX + i * game->getSettings()->width;
            double y = rootY + j * game->getSettings()->height;

            for(const auto& pair : (gameInterface->simulatingFuture() ? current->getOutposts() : game->getOutposts())) {
                if(pair.second->getOwnerID() != gameInterface->getUserGameID()) continue;

                double x1 = pair.second->getPositionAt(getDiff()).getX();
                double y1 = pair.second->getPositionAt(getDiff()).getY();

                draw_circle(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels - 5, Color(1.0, 1.0, 1.0));
            }
        }
    }

    /*for(const auto& pair : current->getOutposts()) {
        double x1 = pair.second->getPositionAt(getDiff()).getX();
        double y1 = pair.second->getPositionAt(getDiff()).getY();

        int shield = pair.second->getShieldAt(getDiff());
        int maxShield = pair.second->getMaxShield();

        for(int i = 0; i < maxShield; i += 10) {
            draw_arc(Vector2(x1 - x, y1 - y) * pixels, 300 + i * 3, UtilityFunctions::deg_to_rad(135), UtilityFunctions::deg_to_rad(135 + 10 * 18), 32, Color(0.0, 0.0, 0.0, 0.2), 20.0, true);
            draw_arc(Vector2(x1 - x, y1 - y) * pixels, 300 + i * 3, UtilityFunctions::deg_to_rad(135), UtilityFunctions::deg_to_rad(135 + std::max(0, std::min(10, shield - i)) * 18), 32, Color(0.0, 0.0, 0.0), 20.0, true);
        }
    }*/

    Player* p = gameInterface->simulatingFuture() ? current->getPlayer(gameInterface->getUserGameID()) : game->getPlayer(gameInterface->getUserGameID());

    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            double x = rootX + i * game->getSettings()->width;
            double y = rootY + j * game->getSettings()->height;

            if(gameInterface->isDragging() && gameInterface->getSelected()) {
                double x1 = gameInterface->getSelected()->getPositionAt(getSimulatedDiff()).getX();
                double y1 = gameInterface->getSelected()->getPositionAt(getSimulatedDiff()).getY();
                double deltaX = gameInterface->getMouse().getX() - x1;
                double deltaY = gameInterface->getMouse().getY() - y1;
                double deltaMag = sqrt(deltaX * deltaX + deltaY * deltaY);

                draw_arc(Vector2(x1 - x, y1 - y) * pixels, 8, 0, UtilityFunctions::deg_to_rad(360), 32, Color(0.0, 0.0, 0.0), 3.0, true);
                draw_line(Vector2(x1 - x, y1 - y) * pixels + 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(gameInterface->getMouse().getX() - x, gameInterface->getMouse().getY() - y) * pixels, Color(0.0, 0.0, 0.0), 5.0);
                draw_line(Vector2(gameInterface->getMouse().getX() - x, gameInterface->getMouse().getY() - y) * pixels, Vector2(gameInterface->getMouse().getX() + 1.5 * cos(atan2(deltaY, deltaX) + 3.14159 * 3.0/4.0) - x, gameInterface->getMouse().getY() + 1.5 * sin(atan2(deltaY, deltaX) + 3.14159 * 3.0/4.0) - y) * pixels, Color(0.0, 0.0, 0.0), 5.0);
                draw_line(Vector2(gameInterface->getMouse().getX() - x, gameInterface->getMouse().getY() - y) * pixels, Vector2(gameInterface->getMouse().getX() + 1.5 * cos(atan2(deltaY, deltaX) - 3.14159 * 3.0/4.0) - x, gameInterface->getMouse().getY() + 1.5 * sin(atan2(deltaY, deltaX) - 3.14159 * 3.0/4.0) - y) * pixels, Color(0.0, 0.0, 0.0), 5.0);
            }

            for(const auto& pair : game->getVessels()) {
                if(p && !p->withinRange(pair.second, getDiff())) continue;

                Point position = pair.second->getPositionAt(getDiff());

                double x1 = position.getX();
                double y1 = position.getY();
                double deltaX = position.closest(pair.second->getTargetPos()).getX() - x1;
                double deltaY = position.closest(pair.second->getTargetPos()).getY() - y1;
                double deltaMag = sqrt(deltaX * deltaX + deltaY * deltaY);

                if(pair.second->getOrigin()) draw_line(Vector2(x1 - x, y1 - y) * pixels - 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(position.closest(pair.second->getOrigin()->getPosition()).getX() - x, position.closest(pair.second->getOrigin()->getPosition()).getY() - y) * pixels, Color(0.0, 0.0, 0.0, 0.5), 3.0);
                draw_arc(Vector2(x1 - x, y1 - y) * pixels, 8, 0, UtilityFunctions::deg_to_rad(360), 32, Color(0.0, 0.0, 0.0), 3.0, true);
                draw_line(Vector2(x1 - x, y1 - y) * pixels + 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(position.closest(pair.second->getTargetPos()).getX() - x, position.closest(pair.second->getTargetPos()).getY() - y) * pixels, Color(0.0, 0.0, 0.0), 5.0);
            }

            for(const auto& pair : game->getVessels()) {
                if(p && !p->withinRange(pair.second, getDiff())) continue;

                double referenceTime = game->getTime();

                for(BattleEvent* battle : game->nextBattles(pair.first)) {
                    double x1 = pair.second->getPositionAt(battle->getTimestamp() - referenceTime).getX();
                    double y1 = pair.second->getPositionAt(battle->getTimestamp() - referenceTime).getY();
                    
                    const BattleEvent* simulatedBattle = complete->simulatedBattle(battle->getID());

                    Color c;
                    if(simulatedBattle && simulatedBattle->getVictor() != -1) {
                        if(simulatedBattle->getVictor() == gameInterface->getUserGameID()) c = Color(0.0, 0.75, 0.0);
                        else c = Color(0.75, 0.0, 0.0);
                    } else {
                        c = Color(0.75, 0.75, 0.75);
                    }

                    draw_circle(Vector2(x1 - x, y1 - y) * pixels, 10, Color(0.0, 0.0, 0.0));
                    draw_circle(Vector2(x1 - x, y1 - y) * pixels, 9, c);
                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, 5, 0, UtilityFunctions::deg_to_rad(360), 32, Color(0.0, 0.0, 0.0), 0.5, true);

                    for(int i = 45; i < 360; i += 90) {
                        double x2 = x1 + 0.3 * cos(UtilityFunctions::deg_to_rad(i)), x3 = x1 + cos(UtilityFunctions::deg_to_rad(i));
                        double y2 = y1 + 0.3 * sin(UtilityFunctions::deg_to_rad(i)), y3 = y1 + sin(UtilityFunctions::deg_to_rad(i));

                        draw_line(Vector2(x2 - x, y2 - y) * pixels, Vector2(x3 - x, y3 - y) * pixels, Color(0.0, 0.0, 0.0), 0.5, true);
                    }
                }
            }
        }
    }
}