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

#include "joystick_test.hpp"

#include <io/joystick/detail/event_loop.hpp>

#include <utest/wait.hpp>

#include <SDL2/SDL.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::joystick::ut::io_joystick_test);

namespace sight::io::joystick::ut
{

/// This allow us to use protected members of the event_loop class
struct friendly_event_loop : public io::joystick::detail::event_loop
{
    using io::joystick::detail::event_loop::make;
    using io::joystick::detail::event_loop::instance;
};

//------------------------------------------------------------------------------

void io_joystick_test::setUp()
{
}

//------------------------------------------------------------------------------

void io_joystick_test::tearDown()
{
}

//------------------------------------------------------------------------------

void io_joystick_test::basic_test()
{
    // If the event loop is not created, the instance should be null
    CPPUNIT_ASSERT(!friendly_event_loop::instance());

    {
        // Name of the virtual joystick
        static constexpr auto s_VIRTUAL_NAME = "Virtual Flight Stick";

        // Create the event loop
        auto event_loop = friendly_event_loop::make();

        // The instance should be a singleton
        CPPUNIT_ASSERT(event_loop == friendly_event_loop::instance());

        struct interactor final : public io::joystick::interactor
        {
            using io::joystick::interactor::interactor::devices;
            using io::joystick::interactor::interactor::start_listening_joystick;
            using io::joystick::interactor::interactor::stop_listening_joystick;

            interactor()
            {
                start_listening_joystick();
            }

            ~interactor() override
            {
                stop_listening_joystick();
            }

            //------------------------------------------------------------------------------

            void joystick_axis_motion_event(const axis_motion_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index << "]: axis_motion_event["
                << int(_event.axis) << ", " << _event.value << "]" << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_axis_motion_event++;
                }
            }

            //------------------------------------------------------------------------------

            void joystick_axis_direction_event(const axis_direction_event& _event) final
            {
                const auto* const direction =
                    _event.value == axis_direction_event::direction_t::up
                    ? "->"
                    : _event.value == axis_direction_event::direction_t::down
                    ? "<-"
                    : "-";

                std::cerr << "Device " << _event.device->name << " [" << _event.device->index
                << "]: axis_direction_event["
                << int(_event.axis) << ", " << (direction) << "]" << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_axis_direction_event = true;
                }
            }

            //------------------------------------------------------------------------------

            void joystick_hat_motion_event(const hat_motion_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index << "]: hat_motion_event["
                << int(_event.hat) << ", " << int(_event.value) << "]" << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_hat_motion_event = true;
                }
            }

            //------------------------------------------------------------------------------

            void joystick_ball_motion_event(const ball_motion_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index << "]: ball_motion_event["
                << int(_event.ball) << ", " << _event.x << ", " << _event.y << "]" << std::endl;
            }

            //------------------------------------------------------------------------------

            void joystick_button_pressed_event(const button_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index
                << "]: button_pressed_event[" << int(_event.button) << ", " << int(_event.state) << "]" << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_button_pressed_event = true;
                }
            }

            //------------------------------------------------------------------------------

            void joystick_button_released_event(const button_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index
                << "]: button_released_event[" << int(_event.button) << ", " << int(_event.state) << "]" << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_button_released_event = true;
                }
            }

            //------------------------------------------------------------------------------

            void joystick_added_event(const joystick_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index << "]: added_event"
                << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_added_event = true;
                }
            }

            //------------------------------------------------------------------------------

            void joystick_removed_event(const joystick_event& _event) final
            {
                std::cerr << "Device " << _event.device->name << " [" << _event.device->index << "]: removed_event"
                << std::endl;

                if(_event.device->name == s_VIRTUAL_NAME)
                {
                    received_removed_event = true;
                }
            }

            std::atomic_bool received_added_event {false};
            std::atomic_bool received_removed_event {false};
            std::atomic_int received_axis_motion_event {0};
            std::atomic_bool received_axis_direction_event {false};
            std::atomic_bool received_hat_motion_event {false};
            std::atomic_bool received_button_pressed_event {false};
            std::atomic_bool received_button_released_event {false};
        } interactor;

        for(const auto& device : interactor.devices())
        {
            std::cerr << "Device [" << device->index << "]" << std::endl;
            std::cerr << "\tID: " << device->id << std::endl;
            std::cerr << "\tName: " << device->name << std::endl;
            std::cerr << "\tPath: " << device->path << std::endl;
            std::cerr << "\tGUID: " << device->guid << std::endl;
            std::cerr << "\tVendor: " << device->vendor << std::endl;
            std::cerr << "\tProduct: " << device->product << std::endl;
            std::cerr << "\tVersion: " << device->version << std::endl;
            std::cerr << "\tSerial: " << device->serial << std::endl;
            std::cerr << "\tAxes: " << device->axes << std::endl;
            std::cerr << "\tButtons: " << device->buttons << std::endl;
            std::cerr << "\tHats: " << device->hats << std::endl;
            std::cerr << "\tBalls: " << device->balls << std::endl;
            std::cerr << std::endl;
        }

        // Test with a virtual joystick
        {
            struct virtual_device
            {
                virtual_device() :
                    index(SDL_JoystickAttachVirtual(SDL_JoystickType::SDL_JOYSTICK_TYPE_FLIGHT_STICK, 1, 1, 1))
                {
                    CPPUNIT_ASSERT(index >= 0);
                }

                ~virtual_device()
                {
                    CPPUNIT_ASSERT(SDL_JoystickDetachVirtual(index) >= 0);
                }

                int index {-1};
            } virtual_device_instance;

            SIGHT_TEST_WAIT(interactor.received_added_event);
            CPPUNIT_ASSERT(interactor.received_added_event);

            // Retrieve the real SDL objects
            const auto virtual_id = SDL_JoystickGetDeviceInstanceID(virtual_device_instance.index);
            CPPUNIT_ASSERT(virtual_id >= 0);

            auto* const virtual_joystick = SDL_JoystickFromInstanceID(virtual_id);
            CPPUNIT_ASSERT(virtual_joystick != nullptr);

            // Test the virtual joystick
            // 1000 is less than the dead zone. The first value should be ignored, not the second one
            SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 1000);
            SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 10000);
            SIGHT_TEST_WAIT(interactor.received_axis_motion_event > 0);
            CPPUNIT_ASSERT(interactor.received_axis_motion_event == 1);

            // Normally axis event will also trigger the synthetic direction event, but since the second value is less
            // than two time the dead zone, it should not trigger the event.
            // Try with a value that is greater than 16000
            CPPUNIT_ASSERT(not interactor.received_axis_direction_event);
            SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 30000);
            SIGHT_TEST_WAIT(interactor.received_axis_direction_event);
            CPPUNIT_ASSERT(interactor.received_axis_direction_event);

            // Test other events
            SDL_JoystickSetVirtualHat(virtual_joystick, 0, SDL_HAT_UP);
            SIGHT_TEST_WAIT(interactor.received_hat_motion_event);
            CPPUNIT_ASSERT(interactor.received_hat_motion_event);

            SDL_JoystickSetVirtualButton(virtual_joystick, 0, SDL_PRESSED);
            SIGHT_TEST_WAIT(interactor.received_button_pressed_event);
            CPPUNIT_ASSERT(interactor.received_button_pressed_event);

            SDL_JoystickSetVirtualButton(virtual_joystick, 0, SDL_RELEASED);
            SIGHT_TEST_WAIT(interactor.received_button_released_event);
            CPPUNIT_ASSERT(interactor.received_button_released_event);

            // Test blocing joystick events. Only the last event should be received
            interactor.stop_listening_joystick();
            interactor.received_axis_motion_event = 0;
            SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 10000);
            interactor.start_listening_joystick();
            SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 10000);

            SIGHT_TEST_WAIT(interactor.received_axis_motion_event > 0);
            CPPUNIT_ASSERT(interactor.received_axis_motion_event == 1);
        }

        // Now the virtual joystick should be removed
        SIGHT_TEST_WAIT(interactor.received_removed_event);
        CPPUNIT_ASSERT(interactor.received_removed_event);

        /// @note uncomment the following line to test a real joystick
        //SIGHT_TEST_WAIT(false, 100000);
    }

    // The instance should not be recreated, as the original one has expired
    CPPUNIT_ASSERT(!friendly_event_loop::instance());
}

//------------------------------------------------------------------------------

void io_joystick_test::auto_repeat_test()
{
    // Name of the virtual joystick
    static constexpr auto s_VIRTUAL_NAME = "Virtual Flight Stick";

    // Create the event loop
    auto event_loop = friendly_event_loop::make();

    struct interactor final : public io::joystick::interactor
    {
        interactor()
        {
            start_listening_joystick();
        }

        ~interactor() override
        {
            stop_listening_joystick();
        }

        //------------------------------------------------------------------------------

        void joystick_axis_direction_event(const axis_direction_event& _event) final
        {
            if(_event.device->name == s_VIRTUAL_NAME)
            {
                ++received_axis_direction_event;
            }
        }

        //------------------------------------------------------------------------------

        void joystick_added_event(const joystick_event& _event) final
        {
            if(_event.device->name == s_VIRTUAL_NAME)
            {
                received_added_event = true;
            }
        }

        std::atomic_int received_axis_direction_event {0};
        std::atomic_bool received_added_event {false};
    } interactor;

    struct virtual_device
    {
        virtual_device() :
            index(SDL_JoystickAttachVirtual(SDL_JoystickType::SDL_JOYSTICK_TYPE_FLIGHT_STICK, 1, 1, 1))
        {
            CPPUNIT_ASSERT(index >= 0);
        }

        ~virtual_device()
        {
            CPPUNIT_ASSERT(SDL_JoystickDetachVirtual(index) >= 0);
        }

        int index {-1};
    } virtual_device_instance;

    // Wait for the virtual joystick to be added
    SIGHT_TEST_WAIT(interactor.received_added_event);
    CPPUNIT_ASSERT(interactor.received_added_event);

    // Retrieve the real SDL objects
    const auto virtual_id = SDL_JoystickGetDeviceInstanceID(virtual_device_instance.index);
    CPPUNIT_ASSERT(virtual_id >= 0);

    auto* const virtual_joystick = SDL_JoystickFromInstanceID(virtual_id);
    CPPUNIT_ASSERT(virtual_joystick != nullptr);

    // Send 3 axis events
    CPPUNIT_ASSERT(not interactor.received_axis_direction_event);
    SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 30000);

    // The event should be repeated
    SIGHT_TEST_WAIT(interactor.received_axis_direction_event > 2);
    CPPUNIT_ASSERT(interactor.received_axis_direction_event > 2);

    // ..and be stopped once we go back to the dead zone
    SDL_JoystickSetVirtualAxis(virtual_joystick, 0, 0);

    // Wait for the event to be processed
    auto received = interactor.received_axis_direction_event.load();
    SIGHT_TEST_WAIT(interactor.received_axis_direction_event.load() > received);

    // Wait 500 ms again to see if nothing is received
    received = interactor.received_axis_direction_event.load();
    SIGHT_TEST_WAIT(interactor.received_axis_direction_event.load() > received, 500);
    CPPUNIT_ASSERT(interactor.received_axis_direction_event.load() == received);
}

} //namespace sight::io::joystick::ut
