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

#include "event_loop.hpp"

#include <core/exception.hpp>
#include <core/exceptionmacros.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

// cspell:ignore disrumpere interactor interactors jaxis jball jhat jbutton jdevice xrel yrel
// cspell:ignore JOYBUTTONDOWN JOYBUTTONUP JOYAXISMOTION JOYHATMOTION JOYBALLMOTION JOYDEVICEADDED JOYDEVICEREMOVED
// cspell:ignore WILLENTERBACKGROUND WILLENTERFOREGROUND DIDENTERBACKGROUND SYSWMEVENT JOYBATTERYUPDATED
// cspell:ignore DIDENTERFOREGROUND MOUSEMOTION CONTROLLERAXISMOTION CONTROLLERBUTTONDOWN CONTROLLERBUTTONUP
// cspell:ignore CONTROLLERDEVICEADDED CONTROLLERDEVICEREMOVED CONTROLLERDEVICEREMAPPED CONTROLLERTOUCHPADDOWN
// cspell:ignore CONTROLLERTOUCHPADMOTION CONTROLLERTOUCHPADUP CONTROLLERSENSORUPDATE CONTROLLERSTEAMHANDLEUPDATED
// cspell:ignore FINGERDOWN FINGERUP FINGERMOTION DOLLARGESTURE DOLLARRECORD MULTIGESTURE CLIPBOARDUPDATE DROPCOMPLETE
// cspell:ignore AUDIODEVICEADDED AUDIODEVICEREMOVED SENSORUPDATE POLLSENTINEL MOUSEBUTTONUP

namespace sight::io::joystick::detail
{

/// This is the recommended value for the dead zone.
/// @note we may need to change this value depending on the joystick, for now this is hardcoded.
static constexpr std::int16_t DEFAULT_AXIS_DEAD_ZONE      = 8000;
static constexpr std::int16_t DEFAULT_WIDE_AXIS_DEAD_ZONE = DEFAULT_AXIS_DEAD_ZONE * 2;

/// 10 milliseconds should be fine.
static constexpr std::uint16_t DEFAULT_REFRESH_RATE = 10;

// This will protect the weak reference
static std::mutex s_mutex;

// This will keep a weak reference to the instance.
static std::weak_ptr<event_loop> s_weak_instance;

/// This is used for debugging purposes only
[[maybe_unused]] static constexpr std::string_view event_name(std::uint32_t _event_type)
{
    switch(_event_type)
    {
        case SDL_QUIT:
            return "SDL_QUIT";

        case SDL_APP_TERMINATING:
            return "SDL_APP_TERMINATING";

        case SDL_APP_LOWMEMORY:
            return "SDL_APP_LOWMEMORY";

        case SDL_APP_WILLENTERBACKGROUND:
            return "SDL_APP_WILLENTERBACKGROUND";

        case SDL_APP_DIDENTERBACKGROUND:
            return "SDL_APP_DIDENTERBACKGROUND";

        case SDL_APP_WILLENTERFOREGROUND:
            return "SDL_APP_WILLENTERFOREGROUND";

        case SDL_APP_DIDENTERFOREGROUND:
            return "SDL_APP_DIDENTERFOREGROUND";

        case SDL_LOCALECHANGED:
            return "SDL_LOCALECHANGED";

        case SDL_DISPLAYEVENT:
            return "SDL_DISPLAYEVENT";

        case SDL_WINDOWEVENT:
            return "SDL_WINDOWEVENT";

        case SDL_SYSWMEVENT:
            return "SDL_SYSWMEVENT";

        case SDL_KEYDOWN:
            return "SDL_KEYDOWN";

        case SDL_KEYUP:
            return "SDL_KEYUP";

        case SDL_TEXTEDITING:
            return "SDL_TEXTEDITING";

        case SDL_TEXTINPUT:
            return "SDL_TEXTINPUT";

        case SDL_KEYMAPCHANGED:
            return "SDL_KEYMAPCHANGED";

        case SDL_TEXTEDITING_EXT:
            return "SDL_TEXTEDITING_EXT";

        case SDL_MOUSEMOTION:
            return "SDL_MOUSEMOTION";

        case SDL_MOUSEBUTTONDOWN:
            return "SDL_MOUSEBUTTONDOWN";

        case SDL_MOUSEBUTTONUP:
            return "SDL_MOUSEBUTTONUP";

        case SDL_MOUSEWHEEL:
            return "SDL_MOUSEWHEEL";

        case SDL_JOYAXISMOTION:
            return "SDL_JOYAXISMOTION";

        case SDL_JOYBALLMOTION:
            return "SDL_JOYBALLMOTION";

        case SDL_JOYHATMOTION:
            return "SDL_JOYHATMOTION";

        case SDL_JOYBUTTONDOWN:
            return "SDL_JOYBUTTONDOWN";

        case SDL_JOYBUTTONUP:
            return "SDL_JOYBUTTONUP";

        case SDL_JOYDEVICEADDED:
            return "SDL_JOYDEVICEADDED";

        case SDL_JOYDEVICEREMOVED:
            return "SDL_JOYDEVICEREMOVED";

        case SDL_JOYBATTERYUPDATED:
            return "SDL_JOYBATTERYUPDATED";

        case SDL_CONTROLLERAXISMOTION:
            return "SDL_CONTROLLERAXISMOTION";

        case SDL_CONTROLLERBUTTONDOWN:
            return "SDL_CONTROLLERBUTTONDOWN";

        case SDL_CONTROLLERBUTTONUP:
            return "SDL_CONTROLLERBUTTONUP";

        case SDL_CONTROLLERDEVICEADDED:
            return "SDL_CONTROLLERDEVICEADDED";

        case SDL_CONTROLLERDEVICEREMOVED:
            return "SDL_CONTROLLERDEVICEREMOVED";

        case SDL_CONTROLLERDEVICEREMAPPED:
            return "SDL_CONTROLLERDEVICEREMAPPED";

        case SDL_CONTROLLERTOUCHPADDOWN:
            return "SDL_CONTROLLERTOUCHPADDOWN";

        case SDL_CONTROLLERTOUCHPADMOTION:
            return "SDL_CONTROLLERTOUCHPADMOTION";

        case SDL_CONTROLLERTOUCHPADUP:
            return "SDL_CONTROLLERTOUCHPADUP";

        case SDL_CONTROLLERSENSORUPDATE:
            return "SDL_CONTROLLERSENSORUPDATE";

        case SDL_CONTROLLERSTEAMHANDLEUPDATED:
            return "SDL_CONTROLLERSTEAMHANDLEUPDATED";

        case SDL_FINGERDOWN:
            return "SDL_FINGERDOWN";

        case SDL_FINGERUP:
            return "SDL_FINGERUP";

        case SDL_FINGERMOTION:
            return "SDL_FINGERMOTION";

        case SDL_DOLLARGESTURE:
            return "SDL_DOLLARGESTURE";

        case SDL_DOLLARRECORD:
            return "SDL_DOLLARRECORD";

        case SDL_MULTIGESTURE:
            return "SDL_MULTIGESTURE";

        case SDL_CLIPBOARDUPDATE:
            return "SDL_CLIPBOARDUPDATE";

        case SDL_DROPFILE:
            return "SDL_DROPFILE";

        case SDL_DROPTEXT:
            return "SDL_DROPTEXT";

        case SDL_DROPBEGIN:
            return "SDL_DROPBEGIN";

        case SDL_DROPCOMPLETE:
            return "SDL_DROPCOMPLETE";

        case SDL_AUDIODEVICEADDED:
            return "SDL_AUDIODEVICEADDED";

        case SDL_AUDIODEVICEREMOVED:
            return "SDL_AUDIODEVICEREMOVED";

        case SDL_SENSORUPDATE:
            return "SDL_SENSORUPDATE";

        case SDL_RENDER_TARGETS_RESET:
            return "SDL_RENDER_TARGETS_RESET";

        case SDL_RENDER_DEVICE_RESET:
            return "SDL_RENDER_DEVICE_RESET";

        case SDL_POLLSENTINEL:
            return "SDL_POLLSENTINEL";

        case SDL_USEREVENT:
            return "SDL_USEREVENT";

        default:
            return "Unknown event type";
    }
}

//------------------------------------------------------------------------------

event_loop::axis_direction_t event_loop::map_axis(
    const axis_aliases_t& _axis_aliases,
    std::uint8_t _axis_index,
    direction_t _direction
)
{
    if(const auto& found = _axis_aliases.find(_axis_index); found != _axis_aliases.end())
    {
        if(found->second.second == axis_direction_event::direction_t::up)
        {
            // No inversion for the up direction
            return {found->second.first, _direction};
        }

        if(found->second.second == axis_direction_event::direction_t::down)
        {
            // We need to inverse the direction
            if(_direction == axis_direction_event::direction_t::up)
            {
                // Inversion for the up direction
                return {found->second.first, axis_direction_event::direction_t::down};
            }

            if(_direction == axis_direction_event::direction_t::down)
            {
                // Inversion for the down direction
                return {found->second.first, axis_direction_event::direction_t::up};
            }

            // Centered direction
            return {found->second.first, axis_direction_event::direction_t::centered};
        }
    }

    return {axis_t::unknown, axis_direction_event::direction_t::centered};
}

//------------------------------------------------------------------------------

event_loop::event_loop()
{
    // Initialize SDL2
    // SDL_INIT_EVERYTHING otherwise some joysticks are not detected or some buttons are not working
    SIGHT_THROW_IF("could not initialize SDL2: " << SDL_GetError(), SDL_Init(SDL_INIT_EVERYTHING) < 0);

    const auto number_of_joysticks = SDL_NumJoysticks();
    SIGHT_THROW_IF("could not get the number of joysticks: " << SDL_GetError(), number_of_joysticks < 0);

    // Initial lookup for already connected controllers
    for(int i = 0 ; i < number_of_joysticks ; ++i)
    {
        add_joystick(i);
    }

    // Start the event loop
    m_timer = sight::core::thread::get_default_worker()->create_timer();
    m_timer->set_duration(std::chrono::milliseconds(DEFAULT_REFRESH_RATE));
    m_timer->set_function(&loop);
    m_timer->start();
}

event_loop::~event_loop()
{
    if(m_timer)
    {
        // Be sure to be on the main thread.
        sight::core::thread::get_default_worker()->post_task<void>(
            [this]
            {
                this->internal_stop();
            }).get();
    }
}

//------------------------------------------------------------------------------

void event_loop::loop()
{
    const auto instance = event_loop::instance();

    // Normally this should not happen, but it could, if there are still posted "loop" task, in the main thread,
    // while the event_loop instance is already destroyed.
    if(!instance)
    {
        return;
    }

    SDL_Event event;

    while(SDL_PollEvent(&event) != 0)
    {
        switch(event.type)
        {
            case SDL_QUIT:
            {
                std::unique_lock lock(instance->m_mutex);
                instance->m_timer->stop();
                instance->m_timer.reset();
            }
                return;

            case SDL_JOYAXISMOTION:
            {
                std::unique_lock lock(instance->m_mutex);

                // Get the current axis direction to check if we must send the axis_direction_even or not
                // Rules are:
                // 1. If the axis is centered (in the dead zone), we must not send any event, but remove it
                // 2. If the axis is not centered:
                //      a. If the axis is not already in the list or the direction is different,
                //         we must add it and send the event
                const std::pair<SDL_JoystickID, std::uint8_t> axis_id {event.jaxis.which, event.jaxis.axis};
                const auto stored_axis_direction =
                    [&instance, &axis_id]() -> std::optional<axis_direction_event::direction_t>
                    {
                        if(const auto found = instance->m_axis_directions.find(axis_id);
                           found != instance->m_axis_directions.end())
                        {
                            return std::make_optional(found->second);
                        }

                        return std::nullopt;
                    }();

                const auto device = instance->m_controllers[event.jaxis.which];

                // Do not act as a seismographic sensor for the axis if the initial state is centered
                // (to avoid triggers)
                const bool regular_axis = std::abs(device->initial_axis_state[event.jaxis.axis].value_or(0))
                                          <= DEFAULT_AXIS_DEAD_ZONE;

                if(regular_axis && std::abs(event.jaxis.value) < DEFAULT_AXIS_DEAD_ZONE)
                {
                    // If we are in the dead zone, send a "centered" event if needed (if we were not already centered)
                    if(stored_axis_direction && *stored_axis_direction != axis_direction_event::direction_t::centered)
                    {
                        instance->m_axis_directions.insert_or_assign(
                            axis_id,
                            axis_direction_event::direction_t::centered
                        );

                        const auto axis_alias = map_axis(
                            device->axis_aliases,
                            event.jaxis.axis,
                            axis_direction_event::direction_t::centered
                        );

                        const auto sight_event = axis_direction_event {
                            joystick_event {
                                .device    = device,
                                .timestamp = event.jaxis.timestamp,
                                .count     = 0
                            },
                            event.jaxis.axis,
                            axis_alias.second,
                            axis_alias.first
                        };

                        for(const auto& interactor : instance->m_interactors)
                        {
                            interactor->joystick_axis_direction_event(sight_event);
                        }

                        // Remove the auto-repeat
                        if(const auto found = instance->m_auto_repeat_directions.find(axis_id);
                           found != instance->m_auto_repeat_directions.end())
                        {
                            found->second.reset();
                        }
                    }

                    break;
                }

                const auto direction =
                    (event.jaxis.value < -DEFAULT_WIDE_AXIS_DEAD_ZONE)
                    ? axis_direction_event::direction_t::down
                    : (event.jaxis.value > DEFAULT_WIDE_AXIS_DEAD_ZONE)
                    ? axis_direction_event::direction_t::up
                    : axis_direction_event::direction_t::centered;

                const auto axis_alias = map_axis(device->axis_aliases, event.jaxis.axis, direction);

                if(direction != axis_direction_event::direction_t::centered
                   && (!stored_axis_direction || *stored_axis_direction != direction))
                {
                    // If the axis is not already in the list, or the direction is not the same, insert the axis
                    // direction in the list and send the event
                    instance->m_axis_directions.insert_or_assign(axis_id, direction);

                    const auto sight_event = axis_direction_event {
                        joystick_event {
                            .device    = device,
                            .timestamp = event.jaxis.timestamp,
                            .count     = 0
                        },
                        event.jaxis.axis,
                        axis_alias.second,
                        axis_alias.first
                    };

                    for(const auto& interactor : instance->m_interactors)
                    {
                        interactor->joystick_axis_direction_event(sight_event);
                    }

                    // Set the axis_direction_event of the auto-repeat, but not for triggers
                    if(regular_axis)
                    {
                        instance->m_auto_repeat_directions.insert_or_assign(
                            axis_id,
                            std::make_shared<axis_direction_event>(sight_event)
                        );
                    }
                }

                // Send the axis motion event
                const axis_motion_event sight_event {
                    joystick_event {
                        .device    = device,
                        .timestamp = event.jaxis.timestamp,
                        .count     = 0
                    },
                    event.jaxis.axis,
                    event.jaxis.value,
                    axis_alias.first
                };

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_axis_motion_event(sight_event);
                }

                break;
            }

            case SDL_JOYHATMOTION:
            {
                std::unique_lock lock(instance->m_mutex);
                const auto device = instance->m_controllers[event.jhat.which];
                const hat_motion_event sight_event {
                    joystick_event {
                        .device    = device,
                        .timestamp = event.jhat.timestamp,
                        .count     = 0
                    },
                    event.jhat.hat,
                    hat_motion_event::position_t(event.jhat.value)
                };

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_hat_motion_event(sight_event);
                }

                break;
            }

            case SDL_JOYBALLMOTION:
            {
                std::unique_lock lock(instance->m_mutex);
                const auto device = instance->m_controllers[event.jball.which];
                const ball_motion_event sight_event {
                    joystick_event {
                        .device    = device,
                        .timestamp = event.jball.timestamp,
                        .count     = 0
                    },
                    event.jball.ball,
                    event.jball.xrel,
                    event.jball.yrel
                };

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_ball_motion_event(sight_event);
                }

                break;
            }

            case SDL_JOYBUTTONDOWN:
            {
                std::unique_lock lock(instance->m_mutex);
                const auto device = instance->m_controllers[event.jbutton.which];
                const button_event sight_event {
                    joystick_event {
                        .device    = device,
                        .timestamp = event.jbutton.timestamp,
                        .count     = 0
                    },
                    event.jbutton.button,
                    button_event::state_t(event.jbutton.state)
                };

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_button_pressed_event(sight_event);
                }

                break;
            }

            case SDL_JOYBUTTONUP:
            {
                std::unique_lock lock(instance->m_mutex);
                const auto device = instance->m_controllers[event.jbutton.which];
                const button_event sight_event {
                    joystick_event {
                        .device    = device,
                        .timestamp = event.jbutton.timestamp,
                        .count     = 0
                    },
                    event.jbutton.button,
                    button_event::state_t(event.jbutton.state)
                };

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_button_released_event(sight_event);
                }

                break;
            }

            case SDL_JOYDEVICEADDED:
            {
                // Add a new controller
                std::unique_lock lock(instance->m_mutex);
                const auto joystick_id                    = instance->add_joystick(event.jdevice.which);
                const auto device                         = instance->m_controllers[joystick_id];
                const joystick_event sight_event {.device = device, .timestamp = event.jdevice.timestamp, .count = 0};

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_added_event(sight_event);
                }

                break;
            }

            case SDL_JOYDEVICEREMOVED:
            {
                // Remove a controller
                std::unique_lock lock(instance->m_mutex);

                const auto device                         = instance->m_controllers[event.jdevice.which];
                const joystick_event sight_event {.device = device, .timestamp = event.jdevice.timestamp, .count = 0};

                for(const auto& interactor : instance->m_interactors)
                {
                    interactor->joystick_removed_event(sight_event);
                }

                instance->remove_joystick(event.jdevice.which);

                break;
            }

            default:
                SIGHT_DEBUG(
                    "SDL event type: "
                    << event.type
                    << " ("
                    << event_name(event.type)
                    << ")"
                );
                break;
        }
    }

    // Process auto-repeat events
    {
        std::unique_lock lock(instance->m_mutex);

        const auto timestamp = SDL_GetTicks();

        for(auto& [axis_id, auto_repeat] : instance->m_auto_repeat_directions)
        {
            const auto delay =
                [] (std::uint32_t _count) constexpr->std::uint32_t
            {
                if(_count < 2)
                {
                    return 500;
                }

                if(_count < 4)
                {
                    return 200;
                }

                if(_count < 8)
                {
                    return 100;
                }

                if(_count < 16)
                {
                    return 50;
                }

                if(_count < 32)
                {
                    return 25;
                }

                return 0;
            };

            // If skip if auto repeat is not set or it is not the time to repeat
            if(!auto_repeat
               || timestamp - auto_repeat->timestamp < delay(auto_repeat->count))
            {
                continue;
            }

            // Update the timestamp
            auto_repeat = std::make_shared<axis_direction_event>(
                axis_direction_event {
                    joystick_event {
                        .device    = auto_repeat->device,
                        .timestamp = timestamp,
                        .count     = auto_repeat->count + 1
                    },
                    auto_repeat->axis,
                    auto_repeat->value,
                    auto_repeat->axis_alias
                });

            for(const auto& interactor : instance->m_interactors)
            {
                interactor->joystick_axis_direction_event(*auto_repeat);
            }
        }
    }
}

//------------------------------------------------------------------------------

SDL_JoystickID event_loop::add_joystick(int _index)
{
    // No need to add the joystick if it is already added
    if(const auto already_open = SDL_JoystickGetDeviceInstanceID(_index);
       already_open >= 0 && m_controllers.contains(already_open))
    {
        // Joystick already added
        return already_open;
    }

    auto* const joystick   = SDL_JoystickOpen(_index);
    const auto joystick_id = SDL_JoystickInstanceID(joystick);

    std::string joystick_name;
    if(const auto* const name = SDL_JoystickName(joystick); name != nullptr)
    {
        joystick_name = name;
    }
    else
    {
        SIGHT_ERROR(
            "Could not get the name of the controller `"
            + std::to_string(joystick_id)
            + "`: "
            + SDL_GetError()
        );
    }

    std::string joystick_path;
    if(const auto* const path = SDL_JoystickPath(joystick); path != nullptr)
    {
        joystick_path = path;
    }
    else
    {
        SIGHT_ERROR(
            "Could not get the path of the controller `"
            + std::to_string(joystick_id)
            + "`: "
            + SDL_GetError()
        );
    }

    std::string joystick_guid(34, '\0');

    if(const auto& guid = SDL_JoystickGetGUID(joystick);
       std::any_of(std::begin(guid.data), std::end(guid.data), [](auto _i){return _i != 0;}))
    {
        SDL_JoystickGetGUIDString(guid, joystick_guid.data(), int(joystick_guid.size()));

        // Resize the string to remove the trailing null characters
        joystick_guid.resize(std::strlen(joystick_guid.data()));
    }
    else
    {
        SIGHT_ERROR(
            "Could not get the guid of the controller `"
            + std::to_string(joystick_id)
            + "`: "
            + SDL_GetError()
        );
    }

    const auto joystick_axes = SDL_JoystickNumAxes(joystick);
    SIGHT_ERROR_IF(
        "Could not get the number of axes the controller `"
        + std::to_string(joystick_id)
        + "`: "
        + SDL_GetError(),
        joystick_axes < 0
    );

    // Compute the initial axis state, so we can distinguish between trigger and axis
    std::vector<std::optional<std::int16_t> > initial_axis_state;
    initial_axis_state.reserve(std::size_t(joystick_axes));

    for(std::int32_t i = 0 ; i < joystick_axes ; ++i)
    {
        std::int16_t initial_state = 0;

        if(SDL_JoystickGetAxisInitialState(joystick, i, &initial_state) == SDL_bool::SDL_TRUE)
        {
            initial_axis_state.emplace_back(std::make_optional(initial_state));
        }
        else
        {
            // Last ressort, we get the axis value. Generally triggers are not centered, so we can use this value
            SDL_JoystickUpdate();

            const std::int16_t axis_value = SDL_JoystickGetAxis(joystick, i);

            if(std::abs(axis_value) > DEFAULT_AXIS_DEAD_ZONE)
            {
                initial_axis_state.emplace_back(std::make_optional(axis_value));
            }
            else
            {
                initial_axis_state.emplace_back(std::nullopt);
            }
        }
    }

    const std::int32_t joystick_buttons = SDL_JoystickNumButtons(joystick);
    SIGHT_ERROR_IF(
        "Could not get the number of buttons the controller `"
        + std::to_string(joystick_id)
        + "`: "
        + SDL_GetError(),
        joystick_buttons < 0
    );

    const std::int32_t joystick_hats = SDL_JoystickNumHats(joystick);
    SIGHT_ERROR_IF(
        "Could not get the number of hats the controller `"
        + std::to_string(joystick_id)
        + "`: "
        + SDL_GetError(),
        joystick_hats < 0
    );

    const std::int32_t joystick_balls = SDL_JoystickNumBalls(joystick);
    SIGHT_ERROR_IF(
        "Could not get the number of balls the controller `"
        + std::to_string(joystick_id)
        + "`: "
        + SDL_GetError(),
        joystick_balls < 0
    );

    std::string joystick_serial;
    if(const auto* const serial = SDL_JoystickGetSerial(joystick); serial != nullptr)
    {
        joystick_serial = serial;
    }

    // Compute the alias
    // For now, it is hardcoded...
#ifdef _WIN32
    static constexpr std::int32_t s_LEFT  = 0;
    static constexpr std::int32_t s_RIGHT = 1;
#else
    static constexpr std::int32_t s_LEFT  = 1;
    static constexpr std::int32_t s_RIGHT = 0;
#endif

    // Search if we already have a controller with the same alias
    const auto left_found = std::ranges::any_of(
        m_controllers,
        [](const auto& _controller)
        {
            return _controller.second->alias == joystick_t::left;
        });

    const auto right_found = std::ranges::any_of(
        m_controllers,
        [](const auto& _controller)
        {
            return _controller.second->alias == joystick_t::right;
        });

    joystick_t alias = joystick_t::unknown;

    // Map axis aliases, also hard coded for now
    axis_aliases_t axis_aliases;

    if(joystick_name.find("SpaceMouse") != std::string::npos)
    {
        // It should be mapped to the first controller by a manual/auto configuration
        if(!left_found && (right_found || _index == s_LEFT))
        {
            alias = joystick_t::left;
        }
        else if(!right_found && (left_found || _index == s_RIGHT))
        {
            alias = joystick_t::right;
        }

        axis_aliases[0] = {axis_t::tx, axis_direction_event::direction_t::right};
        axis_aliases[1] = {axis_t::ty, axis_direction_event::direction_t::down};
        axis_aliases[2] = {axis_t::tz, axis_direction_event::direction_t::backward};
        axis_aliases[3] = {axis_t::ry, axis_direction_event::direction_t::down};
        axis_aliases[4] = {axis_t::rx, axis_direction_event::direction_t::left};
        axis_aliases[5] = {axis_t::rz, axis_direction_event::direction_t::left};
    }
    else
    {
        axis_aliases[0] = {axis_t::tx, axis_direction_event::direction_t::right};
        axis_aliases[1] = {axis_t::ty, axis_direction_event::direction_t::up};
        axis_aliases[2] = {axis_t::tz, axis_direction_event::direction_t::forward};
        axis_aliases[3] = {axis_t::rx, axis_direction_event::direction_t::right};
        axis_aliases[4] = {axis_t::ry, axis_direction_event::direction_t::up};
        axis_aliases[5] = {axis_t::rz, axis_direction_event::direction_t::right};
    }

    m_controllers.insert_or_assign(
        joystick_id,
        std::make_shared<concrete_device>(
            concrete_device {
            device {
                .index   = _index,
                .id      = joystick_id,
                .name    = joystick_name,
                .path    = joystick_path,
                .guid    = joystick_guid,
                .vendor  = SDL_JoystickGetVendor(joystick),
                .product = SDL_JoystickGetProduct(joystick),
                .version = SDL_JoystickGetProductVersion(joystick),
                .serial  = joystick_serial,
                .axes    = joystick_axes,
                .buttons = joystick_buttons,
                .hats    = joystick_hats,
                .balls   = joystick_balls,
                .alias   = alias
            },
            joystick,
            initial_axis_state,
            axis_aliases
        })
    );

    return joystick_id;
}

//------------------------------------------------------------------------------

void event_loop::remove_joystick(SDL_JoystickID _id)
{
    if(const auto found = m_controllers.find(_id);
       found != m_controllers.end())
    {
        SDL_JoystickClose(found->second->joystick);
        m_controllers.erase(found);
    }

    std::erase_if(
        m_axis_directions,
        [&_id](const auto& _axis_id)
        {
            return _axis_id.first.first == _id;
        });

    std::erase_if(
        m_auto_repeat_directions,
        [&_id](const auto& _axis_id)
        {
            return _axis_id.first.first == _id;
        });
}

//------------------------------------------------------------------------------

std::shared_ptr<event_loop> event_loop::instance()
{
    std::unique_lock lock(s_mutex);
    return s_weak_instance.lock();
}

//------------------------------------------------------------------------------

std::shared_ptr<event_loop> event_loop::make()
{
    std::unique_lock lock(s_mutex);

    // If the instance is still alive elsewhere, we can use it
    auto shared_instance = s_weak_instance.lock();

    if(shared_instance)
    {
        return shared_instance;
    }

    // If _make is true, we create a new instance, on the main thread
    shared_instance = sight::core::thread::get_default_worker()->post_task<std::shared_ptr<event_loop> >(
        []
        {
            struct concrete_event_loop final : public event_loop {};
            return std::make_shared<concrete_event_loop>();
        }).get();

    {
        // Keep a weak reference to the new instance for later use
        s_weak_instance = shared_instance;
    }

    return shared_instance;
}

//------------------------------------------------------------------------------

std::vector<std::shared_ptr<const device> > event_loop::devices() const
{
    // Be sure to be on the main thread.
    return sight::core::thread::get_default_worker()->post_task<std::vector<std::shared_ptr<const device> > >(
        [this]
        {
            std::vector<std::shared_ptr<const device> > devices;

            std::unique_lock lock(m_mutex);

            std::ranges::transform(
                m_controllers,
                std::back_inserter(devices),
                [](const auto& _controller)
            {
                return _controller.second;
            });

            return devices;
        }).get();
}

//------------------------------------------------------------------------------

void event_loop::set_joystick_alias(std::int32_t _id, joystick_t _alias)
{
    std::unique_lock lock(m_mutex);

    if(auto found = m_controllers.find(_id);
       found != m_controllers.end())
    {
        found->second = std::make_shared<concrete_device>(
            concrete_device {
                device {
                    .index   = found->second->index,
                    .id      = found->second->id,
                    .name    = found->second->name,
                    .path    = found->second->path,
                    .guid    = found->second->guid,
                    .vendor  = found->second->vendor,
                    .product = found->second->product,
                    .version = found->second->version,
                    .serial  = found->second->serial,
                    .axes    = found->second->axes,
                    .buttons = found->second->buttons,
                    .hats    = found->second->hats,
                    .balls   = found->second->balls,
                    .alias   = _alias
                },
                found->second->joystick,
                found->second->initial_axis_state,
                found->second->axis_aliases
            });
    }
    else
    {
        SIGHT_ERROR("Could not find the controller with id: " << _id);
    }
}

//------------------------------------------------------------------------------

void event_loop::add_interactor(interactor* const  _interactor)
{
    std::unique_lock lock(m_mutex);

    // Append the interactor to the list
    m_interactors.push_back(_interactor);
}

//------------------------------------------------------------------------------

void event_loop::remove_interactor(const interactor* const  _interactor, [[maybe_unused]] bool _finalize)
{
    std::unique_lock lock(m_mutex);

    // Remove the interactor from the list
    [[maybe_unused]] const auto result = std::erase(m_interactors, _interactor);

    if(_finalize)
    {
        SIGHT_ASSERT("Interactor still registered", result == 0);
    }
    else
    {
        SIGHT_ASSERT("Interactor not found in the list of interactors", result != 0);
    }
}

//------------------------------------------------------------------------------

void event_loop::internal_stop()
{
    std::unique_lock lock(m_mutex);

    // Stop the timer and close all the connected controllers
    m_timer->stop();
    m_timer.reset();

    std::ranges::for_each(
        m_controllers,
        [](const auto& _controller)
        {
            SDL_JoystickClose(_controller.second->joystick);
        });

    SDL_Quit();
}

} //namespace sight::io::joystick::detail
