/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwVideoQt/Registry.hpp"

#include "fwVideoQt/helper/formats.hpp"
#include "fwVideoQt/Player.hpp"
#include "fwVideoQt/Surface.hpp"

#include <arPreferences/preferences.hpp>

#include <fwCore/exceptionmacros.hpp>
#include <fwCore/spyLog.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

namespace fwVideoQt
{

//-----------------------------------------------------------------------------

Registry& Registry::getInstance()
{
    static Registry instance;
    return instance;
}

//-----------------------------------------------------------------------------

Registry::Registry() :
    m_mapVideoPlayer(Key::less),
    m_mapRefCount(Key::less)
{
}

//-----------------------------------------------------------------------------

Registry::~Registry()
{
    SLM_ASSERT("VideoPlayer map must be empty.", m_mapVideoPlayer.size() == 0 );
}

//-----------------------------------------------------------------------------

Player* Registry::requestPlayer(const ::arData::Camera::csptr& camera)
{
    const ::arData::Camera::SourceType type = camera->getCameraSource();

    Key videoKey;
    videoKey.m_eType = type;
    switch(type)
    {
        case ::arData::Camera::FILE:
            videoKey.m_strID = camera->getVideoFile().string();
            break;
        case ::arData::Camera::STREAM:
            videoKey.m_strID = camera->getStreamUrl();
            break;
        case ::arData::Camera::DEVICE:
            videoKey.m_strID = camera->getCameraID();
            break;
        default:
            SLM_ASSERT("Unknown camera source type", false);
    }

    MapVideoPlayerType::const_iterator playerIter = m_mapVideoPlayer.find(videoKey);
    MapRefCountType::const_iterator refCountIter  = m_mapRefCount.find(videoKey);
    std::uint8_t nbRef                            = 1;

    Player* player;
    if(playerIter != m_mapVideoPlayer.end())
    {
        nbRef  = refCountIter->second + 1;
        player = playerIter->second;
    }
    else
    {
        player = new Player();
        switch(type)
        {
            case ::arData::Camera::FILE:
            {
                /// Path of the video file stored in the camera description
                std::filesystem::path videoPath(camera->getVideoFile());
                std::filesystem::path videoDir(::arPreferences::getVideoDir());

                // For compatibility with old calibration with absolute path
                if (!videoPath.is_absolute())
                {
                    videoPath = videoDir / videoPath;
                }

                try
                {
                    player->initCameraFile(videoPath);
                }
                catch(std::exception& e)
                {
                    delete player;
                    ::fwGui::dialog::MessageDialog::showMessageDialog(
                        "Camera error",
                        e.what(),
                        ::fwGui::dialog::IMessageDialog::WARNING);
                    return nullptr;
                }

                break;
            }
            case ::arData::Camera::STREAM:
            {
                player->initCameraStream(camera->getStreamUrl());
                break;
            }
            case ::arData::Camera::DEVICE:
            {
                ::QVideoFrame::PixelFormat qtPixelFormat  = QVideoFrame::PixelFormat::Format_Invalid;
                ::arData::Camera::PixelFormat sightFormat = camera->getPixelFormat();

                ::fwVideoQt::helper::PixelFormatTranslatorType::right_const_iterator iter;
                iter = ::fwVideoQt::helper::pixelFormatTranslator.right.find(sightFormat);

                if(iter != ::fwVideoQt::helper::pixelFormatTranslator.right.end())
                {
                    qtPixelFormat = iter->second;
                }
                else
                {
                    OSLM_ERROR("No compatible pixel format found");
                }

                player->initCameraDevice(camera->getCameraID(),
                                         camera->getWidth(), camera->getHeight(),
                                         camera->getMaximumFrameRate(),
                                         qtPixelFormat);
                break;
            }
            default:
                SLM_FATAL("Unknown camera source type");
        }
        m_mapVideoPlayer.insert(MapVideoPlayerType::value_type(videoKey, player));
    }
    m_mapRefCount[videoKey] = nbRef;

    return player;
}

//-----------------------------------------------------------------------------

void Registry::releasePlayer(Player* player)
{
    for(const MapVideoPlayerType::value_type elt : m_mapVideoPlayer)
    {
        if(elt.second == player)
        {
            MapRefCountType::const_iterator refCountIter = m_mapRefCount.find(elt.first);
            std::uint8_t nbRef                           = refCountIter->second;
            nbRef--;
            if(nbRef > 0)
            {
                m_mapRefCount[elt.first] = nbRef;
            }
            else
            {
                m_mapVideoPlayer.erase(elt.first);
                m_mapRefCount.erase(elt.first);
                player->deleteLater();
            }
            return;
        }
    }
    SLM_FATAL("Couldn't not release VideoPlayer.");
}

//-----------------------------------------------------------------------------

} //namespace fwVideoQt
