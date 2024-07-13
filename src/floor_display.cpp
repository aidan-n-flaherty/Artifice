#include "floor_display.h"
#include "game_interface.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/font_variation.hpp>
#include <godot_cpp/classes/theme_db.hpp>
#include <godot_cpp/classes/theme.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>
#include "../GameLogic/gameClasses/game.h"
#include "../GameLogic/gameClasses/events/battle_event.h"
#include "../GameLogic/gameClasses/gameObjects/outpost.h"
#include "../GameLogic/gameClasses/events/outpost_range_event.h"

using namespace godot;

FloorDisplay::FloorDisplay(GameInterface* gameInterface) : gameInterface(gameInterface) {}

void FloorDisplay::_draw() {
    if(!gameInterface || !gameInterface->getGame() || !gameInterface->getCompleteGame() || !gameInterface->getCurrentGame() || !gameInterface->getFullGame()) return;

    std::shared_ptr<Game> game = gameInterface->getGame();
    std::shared_ptr<Game> current = gameInterface->getCurrentGame();
    std::shared_ptr<Game> complete = gameInterface->getCompleteGame();
    std::shared_ptr<Game> full = gameInterface->getFullGame();

    SubViewport* viewport = cast_to<SubViewport>(get_parent());
    Node3D* cameraPivot = cast_to<Node3D>(viewport->get_parent());

    int pixels = 10;

    double rootX = cameraPivot->get_position().x - viewport->get_size().x/2.0/pixels;
    double rootY = cameraPivot->get_position().z - viewport->get_size().y/2.0/pixels + 100;

    // draw_arc(const Vector2 &center, double radius, double start_angle, double end_angle, int32_t point_count, const Color &color, double width = -1.0, bool antialiased = false)

    if(gameInterface->getUserGameID() >= 0 && !gameInterface->getFinished()) draw_rect(Rect2(0, 0, viewport->get_size().x, viewport->get_size().y), Color(0.0, 0.0, 0.9, 1.0));

    Player* p = gameInterface->simulatingFuture() ? current->getPlayer(gameInterface->getUserGameID()) : game->getPlayer(gameInterface->getUserGameID());

    std::shared_ptr<Game> visibilityGame = gameInterface->simulatingFuture() ? current : game;

    if(gameInterface->getUserGameID() >= 0 && !gameInterface->getFinished()) {
        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                double x = rootX + i * game->getSettings()->width;
                double y = rootY + j * game->getSettings()->height;

                for(const auto& pair : (gameInterface->simulatingFuture() ? current->getOutposts() : game->getOutposts())) {
                    if(pair.second->getOwnerID() != gameInterface->getUserGameID() && (!pair.second->hasOwner() || !complete->teamGame() || pair.second->getOwner()->getTeamID() != p->getTeamID())) continue;

                    double x1 = pair.second->getPositionAt(getDiff()).getX();
                    double y1 = pair.second->getPositionAt(getDiff()).getY();

                    //draw_circle(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels, Color(0.0, 0.0, 0.4));
                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels - 10, 0, UtilityFunctions::deg_to_rad(360), 32, Color(0.0, 0.0, 0.4), 20, false);
                }
            }
        }

        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                double x = rootX + i * game->getSettings()->width;
                double y = rootY + j * game->getSettings()->height;

                for(const auto& pair : (gameInterface->simulatingFuture() ? current->getOutposts() : game->getOutposts())) {
                    if(pair.second->getOwnerID() != gameInterface->getUserGameID() && (!pair.second->hasOwner() || !complete->teamGame() || pair.second->getOwner()->getTeamID() != p->getTeamID())) continue;

                    double x1 = pair.second->getPositionAt(getDiff()).getX();
                    double y1 = pair.second->getPositionAt(getDiff()).getY();

                    //draw_circle(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels - 20, Color(0.0, 0.0, 0.0));
                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, (pair.second->getSonarRange() * pixels - 20)/2, 0, UtilityFunctions::deg_to_rad(360), 32, Color(0.0, 0.0, 0.0), (pair.second->getSonarRange() * pixels - 20), false);
                }
            }
        }

        if(complete->teamGame()) {
            for(int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++) {
                    double x = rootX + i * game->getSettings()->width;
                    double y = rootY + j * game->getSettings()->height;

                    for(const auto& pair : (gameInterface->simulatingFuture() ? current->getOutposts() : game->getOutposts())) {
                        if(pair.second->getOwnerID() != gameInterface->getUserGameID()) continue;

                        double x1 = pair.second->getPositionAt(getDiff()).getX();
                        double y1 = pair.second->getPositionAt(getDiff()).getY();

                        //draw_circle(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels - 20, Color(1.0, 1.0, 0.01));
                        draw_arc(Vector2(x1 - x, y1 - y) * pixels, (pair.second->getSonarRange() * pixels - 20)/2, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 1.0, 0.01), (pair.second->getSonarRange() * pixels - 20), false);
                    }
                }
            }
        }
    }

    //if an outpost is selected, draw a circle with a different transparent color around the outpost signifying its sonar range 
    //need
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            double x = rootX + i * game->getSettings()->width;
            double y = rootY + j * game->getSettings()->height;

            for(const auto& pair : game->getOutposts()) {
                //if !selected, continue
                if(!(gameInterface->getSelected())) continue;
                if(pair.second->getID() != gameInterface->getSelected()->getID()) continue;

                double x1 = pair.second->getPositionAt(getDiff()).getX();
                double y1 = pair.second->getPositionAt(getDiff()).getY();
                //if the outpost is selected, have it semi-transparent
                draw_arc(Vector2(x1 - x, y1 - y) * pixels, pair.second->getSonarRange() * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,1.0,0.25),10.0,true);
            }
        }
    }

    const OutpostRangeEvent* e = complete->nextFireEvent(game->getTime() + getDiff());
    double fireOpacity = -1.0;
    double fireInterval = game->getSettings()->fireRate * game->getSettings()->baseFireRate / game->getSettings()->simulationSpeed;
    if(e) {
        fireOpacity = std::max(10.0 * std::abs(game->getTime() + getDiff() - (e->getTimestamp() - fireInterval / 2.0)) / (fireInterval / 2.0) - 9.0, 0.0);
        if(fireOpacity > 1.0) fireOpacity = 0.0;
    }

    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            double x = rootX + i * game->getSettings()->width;
            double y = rootY + j * game->getSettings()->height;

            for(const auto& pair : game->getOutposts()) {
                if(p && !visibilityGame->withinRange(p, pair.second, getDiff())) continue;

                if(!pair.second->controlsSpecialist(SpecialistType::SENTRY) && !pair.second->controlsSpecialist(SpecialistType::MARTYR) && !pair.second->controlsSpecialist(SpecialistType::CHANCELLOR)) continue;

                double x1 = pair.second->getPositionAt(getDiff()).getX();
                double y1 = pair.second->getPositionAt(getDiff()).getY();

                if(fireOpacity > 0 && (pair.second->controlsSpecialist(SpecialistType::SENTRY) || pair.second->controlsSpecialist(SpecialistType::CHANCELLOR))) {
                    double maxRange = -1.0;
                    if(pair.second->controlsSpecialist(SpecialistType::SENTRY)) maxRange = std::max(maxRange, double(pair.second->getFireRange()));
                    if(pair.second->controlsSpecialist(SpecialistType::CHANCELLOR)) maxRange = std::max(maxRange, double(game->getSettings()->defaultSonar));

                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, (maxRange * pixels)/2, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 0.0, 1.0, 0.2 * fireOpacity), maxRange * pixels, false);
                }

                if(pair.second->controlsSpecialist(SpecialistType::SENTRY)) draw_arc(Vector2(x1 - x, y1 - y) * pixels, pair.second->getFireRange() * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,0.0,0.25),6.0,false);
                if(pair.second->controlsSpecialist(SpecialistType::MARTYR)) draw_arc(Vector2(x1 - x, y1 - y) * pixels, game->getSettings()->defaultSonar/5.0 * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,0.0,0.25),6.0,false);
                if(pair.second->controlsSpecialist(SpecialistType::CHANCELLOR)) draw_arc(Vector2(x1 - x, y1 - y) * pixels, game->getSettings()->defaultSonar * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,0.0,0.25),6.0,false);
            }

            for(const auto& pair : game->getVessels()) {
                if(p && !visibilityGame->withinRange(p, pair.second, getDiff())) continue;

                if(!pair.second->controlsSpecialist(SpecialistType::MARTYR) && !pair.second->controlsSpecialist(SpecialistType::CHANCELLOR) && !pair.second->controlsSpecialist(SpecialistType::MARAUDER)) continue;

                double x1 = pair.second->getPositionAt(getDiff()).getX();
                double y1 = pair.second->getPositionAt(getDiff()).getY();

                if(fireOpacity > 0 && (pair.second->controlsSpecialist(SpecialistType::MARAUDER) || pair.second->controlsSpecialist(SpecialistType::CHANCELLOR))) {
                    double maxRange = -1.0;
                    if(pair.second->controlsSpecialist(SpecialistType::MARAUDER)) maxRange = std::max(maxRange, double(game->getSettings()->defaultSonar * game->getSettings()->fireRange));
                    if(pair.second->controlsSpecialist(SpecialistType::CHANCELLOR)) maxRange = std::max(maxRange, double(game->getSettings()->defaultSonar));

                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, (maxRange * pixels)/2, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 0.0, 1.0, 0.2 * fireOpacity), maxRange * pixels, false);
                }

                if(pair.second->controlsSpecialist(SpecialistType::MARTYR)) draw_arc(Vector2(x1 - x, y1 - y) * pixels, game->getSettings()->defaultSonar/5.0 * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,0.0,0.25),6.0,false);
                if(pair.second->controlsSpecialist(SpecialistType::CHANCELLOR)) draw_arc(Vector2(x1 - x, y1 - y) * pixels, game->getSettings()->defaultSonar * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,0.0,0.25),6.0,false);
                if(pair.second->controlsSpecialist(SpecialistType::MARAUDER)) draw_arc(Vector2(x1 - x, y1 - y) * pixels, game->getSettings()->defaultSonar * game->getSettings()->fireRange * pixels, 0, UtilityFunctions::deg_to_rad(360), 64, Color(1.0,0.0,0.25),6.0,false);
            }
        }
    }

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

                draw_arc(Vector2(x1 - x, y1 - y) * pixels, 8, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 1.0, 1.0), 3.0, false);
                draw_line(Vector2(x1 - x, y1 - y) * pixels + 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(gameInterface->getMouse().getX() - x, gameInterface->getMouse().getY() - y) * pixels, Color(1.0, 1.0, 1.0), 5.0);
                draw_line(Vector2(gameInterface->getMouse().getX() - x, gameInterface->getMouse().getY() - y) * pixels, Vector2(gameInterface->getMouse().getX() + 1.5 * cos(atan2(deltaY, deltaX) + 3.14159 * 3.0/4.0) - x, gameInterface->getMouse().getY() + 1.5 * sin(atan2(deltaY, deltaX) + 3.14159 * 3.0/4.0) - y) * pixels, Color(1.0, 1.0, 1.0), 5.0);
                draw_line(Vector2(gameInterface->getMouse().getX() - x, gameInterface->getMouse().getY() - y) * pixels, Vector2(gameInterface->getMouse().getX() + 1.5 * cos(atan2(deltaY, deltaX) - 3.14159 * 3.0/4.0) - x, gameInterface->getMouse().getY() + 1.5 * sin(atan2(deltaY, deltaX) - 3.14159 * 3.0/4.0) - y) * pixels, Color(1.0, 1.0, 1.0), 5.0);
            }

            Player* player = complete->getPlayer(gameInterface->getUserGameID());
            if(player) {
                for(const auto& pair : player->getVesselPaths()) {
                    double timestamp = pair.first;

                    if(timestamp < game->getTime() + getDiff()) continue;

                    const auto& linePositions = pair.second;

                    Point position = linePositions.first;

                    double x1 = position.getX();
                    double y1 = position.getY();

                    draw_dashed_line(Vector2(x1 - x, y1 - y) * pixels, Vector2(position.closest(linePositions.second).getX() - x, position.closest(linePositions.second).getY() - y) * pixels, Color(1.0, 1.0, 0.25), 4.0, 10.0);
                }
            }

            double fullDiff = getCurrent() - full->getTime();

            for(const auto& pair : game->getVessels()) {
                Vessel* v = full->getVessel(pair.first);

                if(gameInterface->simulatingFuture() && v && p && !full->withinRange(p, v, fullDiff)) continue;

                if(pair.second->getDisabled()) continue;

                if(p && !visibilityGame->withinRange(p, pair.second, getDiff())) continue;

                Point position = pair.second->getPositionAt(getDiff());

                double x1 = position.getX();
                double y1 = position.getY();
                double deltaX = position.closest(pair.second->getTargetPos()).getX() - x1;
                double deltaY = position.closest(pair.second->getTargetPos()).getY() - y1;
                double deltaMag = sqrt(deltaX * deltaX + deltaY * deltaY);

                if(pair.second->getOrigin()) draw_line(Vector2(x1 - x, y1 - y) * pixels - 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(position.closest(pair.second->getOrigin()->getPosition()).getX() - x, position.closest(pair.second->getOrigin()->getPosition()).getY() - y) * pixels, Color(1.0, 1.0, 0.25), 4.0);
                draw_arc(Vector2(x1 - x, y1 - y) * pixels, 8, 0, UtilityFunctions::deg_to_rad(360), 32, Color(1.0, 1.0, 1.0), 3.0, false);
                draw_line(Vector2(x1 - x, y1 - y) * pixels + 8 * Vector2(deltaX / deltaMag, deltaY / deltaMag), Vector2(position.closest(pair.second->getTargetPos()).getX() - x, position.closest(pair.second->getTargetPos()).getY() - y) * pixels, Color(1.0, 1.0, 1.0), 5.0);
            }

            for(const auto& pair : game->getVessels()) {
                Vessel* v = full->getVessel(pair.first);

                if(gameInterface->simulatingFuture() && v && p && !full->withinRange(p, v, fullDiff)) continue;

                if(pair.second->getDisabled()) continue;

                if(p && !visibilityGame->withinRange(p, pair.second, getDiff())) continue;

                double referenceTime = game->getTime();

                for(BattleEvent* battle : game->nextBattles(pair.first)) {
                    double x1 = pair.second->getPositionAt(battle->getTimestamp() - referenceTime).getX();
                    double y1 = pair.second->getPositionAt(battle->getTimestamp() - referenceTime).getY();
                    
                    const BattleEvent* simulatedBattle = complete->simulatedBattle(battle->getID());

                    Color c;
                    if(simulatedBattle && (simulatedBattle->getVictor() != -1 || simulatedBattle->isFriendly())) {
                        if(simulatedBattle->getVictor() == gameInterface->getUserGameID() || simulatedBattle->isFriendly()) c = Color(0.0, 0.75, 1.0);
                        else c = Color(0.75, 0.0, 1.0);
                    } else {
                        c = Color(0.75, 0.75, 1.0);
                    }

                    //draw_circle(Vector2(x1 - x, y1 - y) * pixels, 5, Color(0.0, 0.0, 0.0));
                    //draw_circle(Vector2(x1 - x, y1 - y) * pixels, 9, c);
                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, 5, 0, UtilityFunctions::deg_to_rad(360), 16, Color(1.0, 1.0, 1.0), 5, false);
                    draw_arc(Vector2(x1 - x, y1 - y) * pixels, 4, 0, UtilityFunctions::deg_to_rad(360), 16, c, 5, false);
                    //draw_arc(Vector2(x1 - x, y1 - y) * pixels, 5, 0, UtilityFunctions::deg_to_rad(360), 16, Color(1.0, 1.0, 1.0), 0.5, false);

                    /*for(int i = 45; i < 360; i += 90) {
                        double x2 = x1 + 0.3 * cos(UtilityFunctions::deg_to_rad(i)), x3 = x1 + cos(UtilityFunctions::deg_to_rad(i));
                        double y2 = y1 + 0.3 * sin(UtilityFunctions::deg_to_rad(i)), y3 = y1 + sin(UtilityFunctions::deg_to_rad(i));

                        draw_line(Vector2(x2 - x, y2 - y) * pixels, Vector2(x3 - x, y3 - y) * pixels, Color(1.0, 1.0, 1.0), 0.5, false);
                    }*/
                }
            }
        }
    }

    /*Ref<Font> font = ResourceLoader::get_singleton()->load("res://resources/fonts/FontVariation.tres");

    for(const auto& pair : game->getPlayers()) {
        PositionalObject* center = pair.second->getSpawnLocation();

        if(!center || !dynamic_cast<Outpost*>(center) || pair.second->getOutposts().empty()) continue;

        std::pair<double, double> averageLocation = std::make_pair<double, double>(0, 0);

        for(Outpost* o : pair.second->getOutposts()) {
            averageLocation = std::make_pair<double, double>(averageLocation.first + center->getPositionAt(getDiff()).closest(o->getPosition()).getX(), averageLocation.second + center->getPositionAt(getDiff()).closest(o->getPosition()).getY());
        }

        Point location = Point(complete->getSettings(), averageLocation.first / pair.second->getOutposts().size(), averageLocation.second / pair.second->getOutposts().size());
        location.constrain();

        // void draw_string(const Ref<Font> &font, const Vector2 &pos, const String &text, HorizontalAlignment alignment = (HorizontalAlignment)0, double width = -1, int32_t font_size = 16, const Color &modulate = Color(1, 1, 1, 1), BitField<TextServer::JustificationFlag> justification_flags = (BitField<TextServer::JustificationFlag>)3, TextServer::Direction direction = (TextServer::Direction)0, TextServer::Orientation orientation = (TextServer::Orientation)0) const;

        double x1 = location.getX();
        double y1 = location.getY();

        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                double x = rootX + i * game->getSettings()->width;
                double y = rootY + j * game->getSettings()->height;
    
                draw_set_transform(Vector2(x1 - x, y1 - y) * pixels, 3.141592, Vector2(1, 1));
                draw_string(font, Vector2(-400, 0), String(pair.second->getName().c_str()), HorizontalAlignment::HORIZONTAL_ALIGNMENT_CENTER, 800, 200, Color(0.0, 0.0, 1.0, 1.0), 3, TextServer::Direction::DIRECTION_AUTO, TextServer::Orientation::ORIENTATION_HORIZONTAL);
            }
        }
    }*/
}