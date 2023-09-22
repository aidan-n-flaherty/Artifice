#include "battle_event.h"
#include ".././game_settings.h"

void BattleEvent::specialistPhase(Game* game) {
    int unitsA = a->getUnits();
    int unitsB = b->getUnits();

    if(a->controlsSpecialist(SpecialistType::REVERED_ELDER) && !b->controlsSpecialist(SpecialistType::REVERED_ELDER)) {
        addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s Revered Elder removes all specialists from combat.");
        return;
    }
    if(b->controlsSpecialist(SpecialistType::REVERED_ELDER) && !a->controlsSpecialist(SpecialistType::REVERED_ELDER)) {
        addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s Revered Elder removes all specialists from combat.");
        return;
    }

    for(int i = 0; i < std::max(a->specialistCount(SpecialistType::THIEF), b->specialistCount(SpecialistType::THIEF)); i++) {
        int stolenA = -1, stolenB = -1;

        if(i < a->specialistCount(SpecialistType::THIEF)) stolenB = b->removeUnits(int(0.15 * unitsB));

        if(i < b->specialistCount(SpecialistType::THIEF)) stolenA = a->removeUnits(int(0.15 * unitsA));

        if(stolenA >= 0) {
            addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s Thief steals " + std::to_string(stolenA) + " units");
            b->addUnits(stolenA);
        }
        
        if(stolenB >= 0) {
            addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s Thief steals " + std::to_string(stolenB) + " units");
            a->addUnits(stolenB);
        }
    }

    for(int i = 0; i < a->specialistCount(SpecialistType::LIEUTENANT); i++) {
        addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s Lieutenant kills " + std::to_string(b->removeUnits(5)) + " units");
    }
    
    for(int i = 0; i < b->specialistCount(SpecialistType::LIEUTENANT); i++) {
        addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s Lieutenant kills " + std::to_string(a->removeUnits(5)) + " units");
    }

    for(int i = 0; i < a->specialistCount(SpecialistType::LIEUTENANT); i++) {
        addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s Lieutenant kills " + std::to_string(a->removeUnits(5)) + " units");
    }

    for(int i = 0; i < b->specialistCount(SpecialistType::LIEUTENANT); i++) {
        addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s Lieutenant kills " + std::to_string(b->removeUnits(5)) + " units");
    }

    if(!a->getSpecialists().empty()) {
        for(int i = 0; i < a->ownerSpecialistCount(SpecialistType::GENERAL); i++) {
            addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s General kills " + std::to_string(b->removeUnits(10)) + " units");
        }
    }

    if(!b->getSpecialists().empty()) {
        for(int i = 0; i < b->ownerSpecialistCount(SpecialistType::GENERAL); i++) {
            addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s General kills " + std::to_string(a->removeUnits(10)) + " units");
        }
    }

    for(int i = 0; i < a->specialistCount(SpecialistType::LIEUTENANT); i++) {
        addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s War Hero kills " + std::to_string(b->removeUnits(20)) + " units");
    }

    for(int i = 0; i < b->specialistCount(SpecialistType::LIEUTENANT); i++) {
        addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s War Hero kills " + std::to_string(a->removeUnits(20)) + " units");
    }

    Vessel* v1 = dynamic_cast<Vessel*>(a);
    Vessel* v2 = dynamic_cast<Vessel*>(b);

    Outpost* o1 = dynamic_cast<Outpost*>(a);
    Outpost* o2 = dynamic_cast<Outpost*>(b);

    if(v1 && v2) {
        if(v1->controlsSpecialist(SpecialistType::SABOTEUR)) {
            v2->returnHome();
        }

        if(v2->controlsSpecialist(SpecialistType::SABOTEUR)) {
            v1->returnHome();
        }

        if(v1->controlsSpecialist(SpecialistType::DOUBLE_AGENT) || v2->controlsSpecialist(SpecialistType::DOUBLE_AGENT)) {
            // change ownership of all specialists
            for(Specialist* specialist : v1->getSpecialists()) {
                if(v1->hasOwner()) v1->getOwner()->removeSpecialist(specialist);
                if(v2->hasOwner()) v2->getOwner()->addSpecialist(specialist);
            }

            for(Specialist* specialist : v2->getSpecialists()) {
                if(v2->hasOwner()) v2->getOwner()->removeSpecialist(specialist);
                if(v1->hasOwner()) v1->getOwner()->addSpecialist(specialist);
            }

            v1->setUnits(0);
            v2->setUnits(0);

            // swap owners
            Player* v2Owner = v2->getOwner();
            v2->setOwner(v1->getOwner());
            v1->setOwner(v2Owner);
        }
    }

    if(v1 && o2) {
        if(v1->controlsSpecialist(SpecialistType::INFILTRATOR)) o2->removeShield(o2->getShield());
    }

    if(v2 && o1) {
        if(v2->controlsSpecialist(SpecialistType::INFILTRATOR)) o1->removeShield(o2->getShield());
    }
}

void BattleEvent::postSpecialistPhase(Game* game) {
    int unitsA = a->getUnits();
    int unitsB = b->getUnits();

    if(a->controlsSpecialist(SpecialistType::REVERED_ELDER) != b->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    if(b->ownerControlsSpecialist(SpecialistType::KING)) {
        addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s King kills " + std::to_string(a->removeUnits(int(unitsB / 3.0))) + " units");
    }

    if(a->ownerControlsSpecialist(SpecialistType::KING)) {
        addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s King kills " + std::to_string(b->removeUnits(int(unitsA / 3.0))) + " units");
    }
}

void BattleEvent::victorySpecialistPhase(Game* game) {
    if(a->controlsSpecialist(SpecialistType::REVERED_ELDER) != b->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    if(a->getOwnerID() == getVictor() && a->ownerControlsSpecialist(SpecialistType::ENGINEER)) {
        int lostUnits = startingUnitsA - a->getUnits();

        int restored = a->controlsSpecialist(SpecialistType::ENGINEER) ? int(0.5 * lostUnits) : int(0.25 * lostUnits);

        a->addUnits(restored);
        addMessage(a->getOwnerID(), a->getOwner()->getName() + "'s Engineer restores " + std::to_string(restored) + " units");
    }

    if(b->getOwnerID() == getVictor() && b->ownerControlsSpecialist(SpecialistType::ENGINEER)) {
        int lostUnits = startingUnitsA - b->getUnits();

        int restored = b->controlsSpecialist(SpecialistType::ENGINEER) ? int(0.5 * lostUnits) : int(0.25 * lostUnits);

        b->addUnits(restored);
        addMessage(b->getOwnerID(), b->getOwner()->getName() + "'s Engineer restores " + std::to_string(restored) + " units");
    }
}

void BattleEvent::defeatSpecialistPhase(Game* game) {
    if(a->controlsSpecialist(SpecialistType::REVERED_ELDER) != b->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;
}

void BattleEvent::postCombatSpecialistPhase(Game* game) {
    if(a->controlsSpecialist(SpecialistType::REVERED_ELDER) != b->controlsSpecialist(SpecialistType::REVERED_ELDER)) return;

    if(a->controlsSpecialist(SpecialistType::MARTYR)) {
        int range = GameSettings::defaultSonar/5;

        for(const auto &v : game->getVessels()) {
            if(a->distance(v.second->getPosition()) <= range) game->removeVessel(v.second);
        }

        for(const auto &o : game->getOutposts()) {
            if(a->distance(o.second->getPosition()) <= range) {
                o.second->setType(OutpostType::BROKEN);
                o.second->setUnits(0);

                while(!o.second->getSpecialists().empty()) game->removeSpecialist(o.second->getSpecialists().front());
            }
        }

        game->removeVessel(game->getVessel(a->getID()));
    }

    if(b->controlsSpecialist(SpecialistType::MARTYR)) {
        int range = GameSettings::defaultSonar/5;

        for(const auto &v : game->getVessels()) {
            if(b->distance(v.second->getPosition()) <= range) game->removeVessel(v.second);
        }

        for(const auto &o : game->getOutposts()) {
            if(b->distance(o.second->getPosition()) <= range) {
                o.second->setType(OutpostType::BROKEN);
                o.second->setUnits(0);

                while(!o.second->getSpecialists().empty()) game->removeSpecialist(o.second->getSpecialists().front());
            }
        }

        game->removeVessel(game->getVessel(b->getID()));
    }

    // to ensure order invariance, we have to check if both vessels have assassins first
    bool removeSpecialistsA = b->controlsSpecialist(SpecialistType::ASSASSIN);
    bool removeSpecialistsB = a->controlsSpecialist(SpecialistType::ASSASSIN);

    if(removeSpecialistsA) while(!a->getSpecialists().empty()) game->removeSpecialist(a->getSpecialists().front());
    if(removeSpecialistsB) while(!b->getSpecialists().empty()) game->removeSpecialist(b->getSpecialists().front());
}