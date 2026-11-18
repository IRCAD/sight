/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <module/ui/qt/sound_player.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::ui::qt::sound_player")
{
    // Asking for the audio outputs starts a probe of the devices: once is enough when there is none.
    TEST_CASE("asks_once_when_the_system_has_no_audio_output")
    {
        int asked = 0;
        sight::module::ui::qt::sound_player player([&asked]{++asked; return false;});

        for(int i = 0 ; i < 3 ; ++i)
        {
            player.play("sound.wav");
        }

        CHECK(player.silent());
        CHECK(asked == 1);
    }

    //------------------------------------------------------------------------------

    // A stop releases the player: the next sound asks again, as the system may have one now.
    TEST_CASE("asks_again_after_a_stop")
    {
        int asked = 0;
        sight::module::ui::qt::sound_player player([&asked]{++asked; return false;});

        player.play("sound.wav");
        player.stop();
        CHECK_FALSE(player.silent());
        player.play("sound.wav");

        CHECK(asked == 2);
    }
}
