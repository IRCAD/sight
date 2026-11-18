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

#include "module/ui/qt/sound_player.hpp"

#include <core/spy_log.hpp>

#include <QAudioDevice>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QUrl>

#include <utility>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

static bool has_audio_output()
{
    return !QMediaDevices::audioOutputs().isEmpty();
}

//------------------------------------------------------------------------------

sound_player::sound_player(audio_probe_t _has_audio_output) :
    m_has_audio_output(_has_audio_output ? std::move(_has_audio_output) : audio_probe_t(&has_audio_output))
{
}

//------------------------------------------------------------------------------

sound_player::~sound_player() = default;

//------------------------------------------------------------------------------

void sound_player::play(const std::filesystem::path& _file)
{
    if(m_silent)
    {
        return;
    }

    if(!m_player)
    {
        if(!m_has_audio_output())
        {
            SIGHT_WARN("No audio outputs available: the notification sounds are not played");
            m_silent = true;
            return;
        }

        m_player = std::make_unique<QMediaPlayer>();
        m_player->setAudioOutput(new QAudioOutput(m_player.get()));
    }

    m_player->setSource(QUrl::fromLocalFile(QString::fromStdString(_file.string())));
    m_player->play();
}

//------------------------------------------------------------------------------

void sound_player::stop()
{
    if(m_player)
    {
        m_player->stop();
        m_player.reset();
    }

    m_silent = false;
}

//------------------------------------------------------------------------------

bool sound_player::silent() const
{
    return m_silent;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
