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

#include <sight/io/joystick/config.hpp>

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace sight::io::joystick
{

namespace detail
{

class event_loop;

}

enum class joystick_t : std::uint8_t
{
    unknown = 0x00,
    left    = 0x01,
    right   = 0x02
};

enum class axis_t : std::uint8_t
{
    unknown = 0x00,
    rx      = 0x01,
    ry      = 0x02,
    rz      = 0x04,
    tx      = 0x08,
    ty      = 0x10,
    tz      = 0x20
};

/**
 * @brief Device information structure
 *
 */
struct SIGHT_IO_JOYSTICK_CLASS_API device
{
    const std::int32_t index {-1};
    const std::int32_t id {-1};
    const std::string name;
    const std::string path;
    const std::string guid;
    const std::uint16_t vendor {0};
    const std::uint16_t product {0};
    const std::uint16_t version {0};
    const std::string serial;
    const std::int32_t axes {0};
    const std::int32_t buttons {0};
    const std::int32_t hats {0};
    const std::int32_t balls {0};
    const joystick_t alias {joystick_t::unknown};
};

struct SIGHT_IO_JOYSTICK_CLASS_API joystick_event
{
    const std::shared_ptr<const sight::io::joystick::device> device {};
    const std::uint32_t timestamp {0};
    const std::uint32_t count {0};
};

struct SIGHT_IO_JOYSTICK_CLASS_API axis_motion_event final : public joystick_event
{
    const std::uint8_t axis {0xFF};
    const std::int16_t value {0};
    const axis_t axis_alias {axis_t::unknown};
};

struct SIGHT_IO_JOYSTICK_CLASS_API axis_direction_event final : public joystick_event
{
    const std::uint8_t axis {0xFF};
    const enum class direction_t : std::uint8_t
    {
        centered = 0x00,
        up       = 0x01,
        right    = up,
        forward  = up,
        down     = 0x02,
        left     = down,
        backward = down
    } value {direction_t::centered};
    const axis_t axis_alias {axis_t::unknown};
};

struct SIGHT_IO_JOYSTICK_CLASS_API hat_motion_event final : public joystick_event
{
    const std::uint8_t hat {0xFF};
    const enum class position_t : std::uint8_t
    {
        centered   = 0x00,
        up         = 0x01,
        right      = 0x02,
        down       = 0x04,
        left       = 0x08,
        right_up   = (right | up),
        right_down = (right | down),
        left_up    = (left | up),
        left_down  = (left | down)
    } value {position_t::centered};
};

struct SIGHT_IO_JOYSTICK_CLASS_API ball_motion_event final : public joystick_event
{
    const std::uint8_t ball {0xFF};
    const std::int16_t x {0};
    const std::int16_t y {0};
};

struct SIGHT_IO_JOYSTICK_CLASS_API button_event final : public joystick_event
{
    const std::uint8_t button {0xFF};
    const enum class state_t : std::uint8_t
    {
        released = 0x00,
        pressed  = 0x01
    } state {state_t::released};
};

/**
 * @brief This class is used to interact with the joystick devices.
 *
 * It provides a list of connected controllers and their properties.
 *
 * When a joystick event occurs, the callback function joystick_event() is called.
 */
class SIGHT_IO_JOYSTICK_CLASS_API interactor
{
protected:

    friend class detail::event_loop;

    SIGHT_IO_JOYSTICK_API virtual ~interactor();

    /**
     * @brief register the interactor to the event loop
     *
     * @{
     */
    SIGHT_IO_JOYSTICK_API void start_listening_joystick();
    SIGHT_IO_JOYSTICK_API void stop_listening_joystick() const;
    /// @}

    /**
     * @brief Return the list of connected controllers with their properties
     *
     * @return std::vector<std::shared_ptr<const device>> List of connected controllers
     */
    SIGHT_IO_JOYSTICK_API std::vector<std::shared_ptr<const device> > devices() const;

    /**
     * @brief Set joystick alias. This is a global setting and will be applied to all interactors.
     *
     * @param _id Joystick ID
     * @param _alias Joystick alias (left, right)
     */
    SIGHT_IO_JOYSTICK_API void set_joystick_alias(std::int32_t _id, joystick_t _alias) const;

    /**
     * @brief Callback function called when a joystick event occurs
     *
     * @{
     */
    SIGHT_IO_JOYSTICK_API virtual void joystick_axis_motion_event(const axis_motion_event& _event);

    SIGHT_IO_JOYSTICK_API virtual void joystick_hat_motion_event(const hat_motion_event& _event);

    SIGHT_IO_JOYSTICK_API virtual void joystick_ball_motion_event(const ball_motion_event& _event);

    SIGHT_IO_JOYSTICK_API virtual void joystick_button_pressed_event(const button_event& _event);

    SIGHT_IO_JOYSTICK_API virtual void joystick_button_released_event(const button_event& _event);

    SIGHT_IO_JOYSTICK_API virtual void joystick_added_event(const joystick_event& _event);

    SIGHT_IO_JOYSTICK_API virtual void joystick_removed_event(const joystick_event& _event);
    /// @}

    /**
     * @brief Callback function called when the joystick axis "direction" change.
     *
     * It uses motion events to determine the direction of the axis. Once the axis reaches a certain threshold, for the
     * first time, the direction is set to up or down. No more event will be sent until the axis is centered.
     *
     * @param _event: axis direction event
     */
    SIGHT_IO_JOYSTICK_API virtual void joystick_axis_direction_event(const axis_direction_event& _event);

    /**
     * @brief enum class to string conversion
     *
     * @{
     */
    static constexpr std::string_view to_string(joystick_t _joystick)
    {
        switch(_joystick)
        {
            case joystick_t::left:
                return "left";

            case joystick_t::right:
                return "right";

            default:
                return "unknown";
        }
    }

    //------------------------------------------------------------------------------

    static constexpr joystick_t to_joystick(const std::string_view& _alias)
    {
        if(constexpr auto value = to_string(joystick_t::left); _alias == value)
        {
            return joystick_t::left;
        }

        if(constexpr auto value = to_string(joystick_t::right); _alias == value)
        {
            return joystick_t::right;
        }

        return joystick_t::unknown;
    }

    //------------------------------------------------------------------------------

    static constexpr std::string_view to_string(axis_t _axis)
    {
        switch(_axis)
        {
            case axis_t::rx:
                return "rx";

            case axis_t::ry:
                return "ry";

            case axis_t::rz:
                return "rz";

            case axis_t::tx:
                return "tx";

            case axis_t::ty:
                return "ty";

            case axis_t::tz:
                return "tz";

            default:
                return "unknown";
        }
    }

    //------------------------------------------------------------------------------

    static constexpr axis_t to_axis(const std::string_view& _alias)
    {
        if(constexpr auto value = to_string(axis_t::rx); _alias == value)
        {
            return axis_t::rx;
        }

        if(constexpr auto value = to_string(axis_t::ry); _alias == value)
        {
            return axis_t::ry;
        }

        if(constexpr auto value = to_string(axis_t::rz); _alias == value)
        {
            return axis_t::rz;
        }

        if(constexpr auto value = to_string(axis_t::tx); _alias == value)
        {
            return axis_t::tx;
        }

        if(constexpr auto value = to_string(axis_t::ty); _alias == value)
        {
            return axis_t::ty;
        }

        if(constexpr auto value = to_string(axis_t::tz); _alias == value)
        {
            return axis_t::tz;
        }

        return axis_t::unknown;
    }

    //------------------------------------------------------------------------------

    static constexpr std::string_view to_string(axis_direction_event::direction_t _direction)
    {
        switch(_direction)
        {
            case axis_direction_event::direction_t::up:
                return "up";

            case axis_direction_event::direction_t::down:
                return "down";

            case axis_direction_event::direction_t::centered:
                return "centered";

            default:
                return "unknown";
        }
    }

    /// @}
};

} //namespace sight::io::joystick
