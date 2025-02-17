// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/queue.h"
#include "gamestate/component/internal/commands/base_command.h"
#include "gamestate/component/internal_component.h"

namespace openage {

namespace event {
class EventLoop;
} // namespace event

namespace gamestate::component {

class CommandQueue : public InternalComponent {
public:
	/**
	 * Creates an Ownership component.
	 *
	 * @param loop Event loop that all events from the component are registered on.
	 */
	CommandQueue(const std::shared_ptr<openage::event::EventLoop> &loop);

	component_t get_type() const override;

	/**
	 * Adds a command to the queue.
	 *
	 * @param time Time at which the command is added.
	 * @param command New command.
	 */
	void add_command(const curve::time_t &time,
	                 const std::shared_ptr<command::Command> &command);

	/**
	 * Get the command queue.
	 *
	 * @return Command queue.
	 */
	const curve::Queue<std::shared_ptr<command::Command>> &get_queue() const;

	/**
	 * Get the command in the front of the queue.
	 *
	 * @param time Time at which the command is retrieved.
	 *
	 * @return Command in the front of the queue or nullptr if the queue is empty.
	 */
	std::shared_ptr<command::Command> pop_command(const curve::time_t &time);

private:
	/**
	 * Command queue.
	 */
	curve::Queue<std::shared_ptr<command::Command>> command_queue;
};

} // namespace gamestate::component
} // namespace openage
