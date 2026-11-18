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

#pragma once

#include <QMediaPlayer>

#include <filesystem>
#include <functional>
#include <memory>

namespace sight::module::ui::qt
{

/**
 * @brief The player of the notification sounds of a service.
 *
 * Asking for the audio outputs starts a probe of the audio devices, whose end the application waits for when it stops:
 * the player is created by the first sound. Without audio outputs, the first sound warns and the others are silent.
 */
class sound_player final
{
public:

    /// Tells whether the system has an audio output.
    using audio_probe_t = std::function<bool()>;

    /// @param _has_audio_output asked by the first sound; by default, asks Qt for the audio outputs.
    explicit sound_player(audio_probe_t _has_audio_output = {});
    ~sound_player();

    sound_player(const sound_player&)            = delete;
    sound_player(sound_player&&)                 = delete;
    sound_player& operator=(const sound_player&) = delete;
    sound_player& operator=(sound_player&&)      = delete;

    /// Plays a sound file, unless the system has no audio output.
    void play(const std::filesystem::path& _file);

    /// Stops the sound being played and releases the player: the next sound probes the audio outputs again.
    void stop();

    /// True once a sound found no audio output: the next sounds are not played.
    [[nodiscard]] bool silent() const;

private:

    audio_probe_t m_has_audio_output;

    std::unique_ptr<QMediaPlayer> m_player;

    /// No audio output: the sounds are not played.
    bool m_silent {false};
};

} // namespace sight::module::ui::qt
