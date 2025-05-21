/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "../interactor.hpp"

#include <core/thread/timer.hpp>

#include <SDL2/SDL.h>

#include <map>
#include <ranges>

namespace sight::io::joystick::detail
{

class event_loop
{
public:

    /**
     * @brief Return the singleton instance of the event loop, returns nullptr if singleton is not created
     *
     * @return std::shared_ptr<event_loop> : The singleton instance of the event loop
     */
    static std::shared_ptr<event_loop> instance();

    /**
     * @brief Create a new instance of the event loop, if it does not exist
     *
     * @return std::shared_ptr<event_loop>
     */
    static std::shared_ptr<event_loop> make();

    /**
     * @brief Add an interactor to the event loop
     *
     * @param _interactor : The interactor to add
     */
    void add_interactor(interactor* const _interactor);

    /**
     * @brief Remove an interactor to the event loop
     *
     * @param _interactor : The interactor to remove
     * @param _finalize : If true, we will assert the interactor has already be removed from the event loop
     */
    void remove_interactor(const interactor* const _interactor, bool _finalize = false);

    /**
     * @brief
     *
     * @return std::vector<std::shared_ptr<const device> > : The list of connected controllers
     */
    std::vector<std::shared_ptr<const device> > devices() const;

    /**
     * @brief Set joystick alias. This is a global setting and will be applied to all interactors.
     *
     * @param _id Joystick ID
     * @param _alias Joystick alias (left, right)
     */
    void set_joystick_alias(std::int32_t _id, joystick_t _alias);

    /**
     * @brief Stop the event loop and close all the connected controllers, in the current thread
     *
     * @warning This function is only used for a specific case (unexpected exception/abort in CPPUnit). This is mostly
     *          to avoid locking of hardware resources. It is not recommended to use this function in normal operation.
     *
     */
    void internal_stop();

protected:

    event_loop();
    virtual ~event_loop();

private:

    using direction_t      = axis_direction_event::direction_t;
    using axis_direction_t = std::pair<axis_t, direction_t>;
    using axis_aliases_t   = std::map<std::uint8_t, axis_direction_t>;

    struct concrete_device final : public device
    {
        /// Real SDL joystick handle
        SDL_Joystick* const joystick {nullptr};

        /// This allows to discriminate triggers from regular axis
        const std::vector<std::optional<std::int16_t> > initial_axis_state;

        /// Axis alias mapping and the upward direction
        const axis_aliases_t axis_aliases;
    };

    /// Non-copyable
    /// @{
    event_loop(const event_loop&)            = delete;
    event_loop& operator=(const event_loop&) = delete;
    /// @}

    /**
     * @brief This function is called by the timer to poll the SDL events and forward them to the interactors
     *
     */
    static void loop();

    /**
     * @brief perform the mapping of the axis index to the axis alias + direction
     *
     * @param _axis_aliases
     * @param _axis_index
     * @param _direction
     * @return axis_direction_t
     */
    static axis_direction_t map_axis(
        const axis_aliases_t& _axis_aliases,
        std::uint8_t _axis_index,
        direction_t _direction
    );

    /**
     * @brief Remove a joystick from the list of connected controllers
     *
     * @param _id : The joystick id
     */
    void remove_joystick(SDL_JoystickID _id);

    /**
     * @brief Add a joystick to the list of connected controllers and retrieve its properties
     *
     * @param _index SDL joystick index
     *
     * @return SDL_JoystickID : The joystick id
     */
    SDL_JoystickID add_joystick(int _index);

    /// This protects adding interactors outside the main thread
    mutable std::recursive_mutex m_mutex;

    /// Connected controllers
    std::map<SDL_JoystickID, std::shared_ptr<const concrete_device> > m_controllers;

    /// List of axis directions so we can convert analogical axis position to direction
    std::map<std::pair<SDL_JoystickID, std::uint8_t>, axis_direction_event::direction_t> m_axis_directions;

    /// List of interactors that are interested in joystick events
    std::vector<interactor*> m_interactors;

    /// Timer used to poll the SDL events and forward them to the interactors
    sight::core::thread::timer::sptr m_timer;

    /// If true, the joystick events are sent repeatedly, until the joystick comes back to the centered position
    std::map<std::pair<SDL_JoystickID, std::uint8_t>, std::shared_ptr<axis_direction_event> > m_auto_repeat_directions;
};

} //namespace sight::io::joystick::detail
