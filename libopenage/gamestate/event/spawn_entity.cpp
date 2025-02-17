// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "spawn_entity.h"

#include "coord/phys.h"
#include "gamestate/component/internal/activity.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/ownership.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/definitions.h"
#include "gamestate/entity_factory.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/manager.h"

// TODO: Testing
#include "assets/mod_manager.h"

namespace openage::gamestate::event {

static const std::vector<nyan::fqon_t> aoe1_test_entities = {
	"aoe1_base.data.game_entity.generic.chariot_archer.chariot_archer.ChariotArcher",
	"aoe1_base.data.game_entity.generic.bowman.bowman.Bowman",
	"aoe1_base.data.game_entity.generic.hoplite.hoplite.Hoplite",
	"aoe1_base.data.game_entity.generic.temple.temple.Temple",
	"aoe1_base.data.game_entity.generic.academy.academy.Academy",
};
static const std::vector<nyan::fqon_t> aoe2_test_entities = {
	"aoe2_base.data.game_entity.generic.knight.knight.Knight",
	"aoe2_base.data.game_entity.generic.monk.monk.Monk",
	"aoe2_base.data.game_entity.generic.archer.archer.Archer",
	"aoe2_base.data.game_entity.generic.castle.castle.Castle",
	"aoe2_base.data.game_entity.generic.barracks.barracks.Barracks",
};
static const std::vector<nyan::fqon_t> swgb_test_entities = {
	"swgb_base.data.game_entity.generic.trooper.trooper.Trooper",
	"swgb_base.data.game_entity.generic.force_knight.force_knight.ForceKnight",
	"swgb_base.data.game_entity.generic.bounty_hunter.bounty_hunter.BountyHunter",
	"swgb_base.data.game_entity.generic.command_center.command_center.CommandCenter",
	"swgb_base.data.game_entity.generic.heavy_weapons_factory.heavy_weapons_factory.HeavyWeaponsFactory",
};

Spawner::Spawner(const std::shared_ptr<openage::event::EventLoop> &loop) :
	EventEntity(loop) {
}

size_t Spawner::id() const {
	return 0;
}

std::string Spawner::idstr() const {
	return "spawner";
}


SpawnEntityHandler::SpawnEntityHandler(const std::shared_ptr<openage::event::EventLoop> &loop,
                                       const std::shared_ptr<gamestate::EntityFactory> &factory) :
	OnceEventHandler("game.spawn_entity"),
	loop{loop},
	factory{factory} {
}

void SpawnEntityHandler::setup_event(const std::shared_ptr<openage::event::Event> & /* event */,
                                     const std::shared_ptr<openage::event::State> & /* state */) {
	// TODO
}

void SpawnEntityHandler::invoke(openage::event::EventLoop & /* loop */,
                                const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                const std::shared_ptr<openage::event::State> &state,
                                const curve::time_t &time,
                                const param_map &params) {
	auto gstate = std::dynamic_pointer_cast<gamestate::GameState>(state);

	auto nyan_db = gstate->get_nyan_db();

	auto game_entities = nyan_db->get_obj_children_all("engine.util.game_entity.GameEntity");

	// TODO: Remove hardcoded test entity references
	static std::vector<nyan::fqon_t> test_entities; // declared static so we only have to do this once
	if (test_entities.empty()) {
		auto modpack_ids = gstate->get_mod_manager()->get_load_order();
		for (auto &modpack_id : modpack_ids) {
			if (modpack_id == "aoe1_base") {
				test_entities.insert(test_entities.end(),
				                     aoe1_test_entities.begin(),
				                     aoe1_test_entities.end());
			}
			else if (modpack_id == "aoe2_base") {
				test_entities.insert(test_entities.end(),
				                     aoe2_test_entities.begin(),
				                     aoe2_test_entities.end());
			}
			else if (modpack_id == "swgb_base") {
				test_entities.insert(test_entities.end(),
				                     swgb_test_entities.begin(),
				                     swgb_test_entities.end());
			}
		}
	}

	static uint8_t index = 0;
	nyan::fqon_t nyan_entity = test_entities.at(index);
	++index;
	if (index >= test_entities.size()) {
		index = 0;
	}

	// Create entity
	auto entity = this->factory->add_game_entity(this->loop, gstate, nyan_entity);

	// Setup components
	auto entity_pos = std::dynamic_pointer_cast<component::Position>(
		entity->get_component(component::component_t::POSITION));

	auto pos = params.get("position", gamestate::WORLD_ORIGIN);
	entity_pos->set_position(time, pos);
	entity_pos->set_angle(time, coord::phys_angle_t::from_int(315));

	auto entity_owner = std::dynamic_pointer_cast<component::Ownership>(
		entity->get_component(component::component_t::OWNERSHIP));
	entity_owner->set_owner(time, params.get("owner", 0));

	auto activity = std::dynamic_pointer_cast<component::Activity>(
		entity->get_component(component::component_t::ACTIVITY));
	activity->init(time);
	entity->get_manager()->run_activity_system(time);

	// TODO: Select the unit when it's created
	// very dumb but it gets the job done
	auto select_cb = params.get("select_cb", std::function<void(entity_id_t id)>{});
	select_cb(entity->get_id());

	gstate->add_game_entity(entity);
}

curve::time_t SpawnEntityHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                                      const std::shared_ptr<openage::event::State> & /* state */,
                                                      const curve::time_t &at) {
	return at;
}

} // namespace openage::gamestate::event
