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

#include "plugin_test.hpp"

#include <core/runtime/runtime.hpp>

#include <io/joystick/interactor.hpp>

#include <utest/wait.hpp>

#include <SDL2/SDL.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::joystick::ut::plugin_test);

namespace sight::module::io::joystick::ut
{

inline static const std::string MODULE_NAME = "sight::module::io::joystick";

//------------------------------------------------------------------------------

void plugin_test::setUp()
{
}

//------------------------------------------------------------------------------

void plugin_test::tearDown()
{
}

//------------------------------------------------------------------------------

void plugin_test::basic_test()
{
    namespace joystick = sight::io::joystick;

    // Name of the virtual joystick
    static constexpr auto s_VIRTUAL_NAME = "Virtual Flight Stick";

    struct interactor final : public joystick::interactor
    {
        using joystick::interactor::interactor::devices;

        //------------------------------------------------------------------------------

        void joystick_axis_motion_event(const joystick::axis_motion_event& _event) final
        {
            if(_event.device->name == s_VIRTUAL_NAME)
            {
                received_axis_motion_event++;
            }
        }

        //------------------------------------------------------------------------------

        void joystick_added_event(const joystick::joystick_event& _event) final
        {
            if(_event.device->name == s_VIRTUAL_NAME)
            {
                received_added_event++;
            }
        }

        //------------------------------------------------------------------------------

        void joystick_removed_event(const joystick::joystick_event& _event) final
        {
            if(_event.device->name == s_VIRTUAL_NAME)
            {
                received_removed_event++;
            }
        }

        std::atomic_int received_added_event {0};
        std::atomic_int received_removed_event {0};
        std::atomic_int received_axis_motion_event {0};
    };

    struct virtual_device
    {
        virtual_device() :
            index(SDL_JoystickAttachVirtual(SDL_JoystickType::SDL_JOYSTICK_TYPE_FLIGHT_STICK, 1, 1, 1))
        {
            CPPUNIT_ASSERT(index >= 0);
        }

        ~virtual_device()
        {
            if(const auto module = sight::core::runtime::find_module(MODULE_NAME); module && module->is_started())
            {
                CPPUNIT_ASSERT(SDL_JoystickDetachVirtual(index) >= 0);
            }
        }

        int index {-1};
    };

    {
        interactor interactor_instance;
        virtual_device virtual_device_instance;

        SIGHT_TEST_WAIT(interactor_instance.received_added_event > 0);
        CPPUNIT_ASSERT(interactor_instance.received_added_event == 1);

        // Retrieve the real SDL objects
        const auto virtual_id = SDL_JoystickGetDeviceInstanceID(virtual_device_instance.index);
        CPPUNIT_ASSERT(virtual_id >= 0);

        auto* const virtual_joystick = SDL_JoystickFromInstanceID(virtual_id);
        CPPUNIT_ASSERT(virtual_joystick != nullptr);

        // Send a fake axis motion event
        SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 30000);
        SIGHT_TEST_WAIT(interactor_instance.received_axis_motion_event > 0);
        CPPUNIT_ASSERT(interactor_instance.received_axis_motion_event == 1);

        sight::core::runtime::unload_module(MODULE_NAME);

        // No joystick event should be received after the module is unloaded
        interactor_instance.received_axis_motion_event = 0;
        SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 30000);
        SIGHT_TEST_WAIT(interactor_instance.received_axis_motion_event > 0, 1000);
        CPPUNIT_ASSERT(interactor_instance.received_axis_motion_event == 0);
    }

    {
        // joystick event should be received again once the module is re-loaded, but for NEW interactors
        sight::core::runtime::load_module(MODULE_NAME);

        interactor interactor_instance;

        virtual_device virtual_device_instance;

        SIGHT_TEST_WAIT(interactor_instance.received_added_event > 0);
        CPPUNIT_ASSERT(interactor_instance.received_added_event == 1);

        // Retrieve the real SDL objects
        const auto virtual_id = SDL_JoystickGetDeviceInstanceID(virtual_device_instance.index);
        CPPUNIT_ASSERT(virtual_id >= 0);

        auto* const virtual_joystick = SDL_JoystickFromInstanceID(virtual_id);
        CPPUNIT_ASSERT(virtual_joystick != nullptr);

        SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 30000);
        SIGHT_TEST_WAIT(interactor_instance.received_axis_motion_event > 0);
        CPPUNIT_ASSERT(interactor_instance.received_axis_motion_event == 1);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::joystick::ut
