// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "coord/phys.h"
#include "curve/curve.h"


namespace openage::gamestate {
class GameEntity;

namespace system {

class Move {
public:
	/**
     * Move a game entity to a destination from a move command.
     *
     * @param entity Game entity.
     * @param start_time Start time of change.
     *
     * @return Runtime of the change in simulation time.
     */
	static const curve::time_t move_command(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                        const curve::time_t &start_time);

	/**
     * Move a game entity to a destination.
     *
     * @param entity Game entity.
     * @param destination Destination coordinates.
     * @param start_time Start time of change.
     *
     * @return Runtime of the change in simulation time.
     */
	static const curve::time_t move_default(const std::shared_ptr<gamestate::GameEntity> &entity,
	                                        const coord::phys3 &destination,
	                                        const curve::time_t &start_time);
};

} // namespace system
} // namespace openage::gamestate
