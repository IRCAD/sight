/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "fwVideoQt/config.hpp"

#include <arData/Camera.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace fwVideoQt
{

class Player;

class FWVIDEOQT_CLASS_API Registry
{
public:

    FWVIDEOQT_API static Registry& getInstance();

    FWVIDEOQT_API Player* requestPlayer(const ::arData::Camera::csptr& camera);

    FWVIDEOQT_API void releasePlayer(Player* player);

protected:

    FWVIDEOQT_API Registry();

    FWVIDEOQT_API virtual ~Registry();

private:

    struct Key
    {
        ::arData::Camera::SourceType m_eType;
        std::string m_strID;

        //------------------------------------------------------------------------------

        static bool less( const Key& _rKeyA, const Key& _rKeyB )
        {
            return _rKeyA.m_eType < _rKeyB.m_eType
                   ||  (_rKeyA.m_eType == _rKeyB.m_eType && _rKeyA.m_strID < _rKeyB.m_strID);
        }
    };

    typedef bool (* KeyCompareFuncPtr) (const Key&, const Key&);
    typedef std::map< Key, Player*, KeyCompareFuncPtr> MapVideoPlayerType;
    typedef std::map< Key, std::uint8_t, KeyCompareFuncPtr> MapRefCountType;

    MapVideoPlayerType m_mapVideoPlayer;
    MapRefCountType m_mapRefCount;
};

} //namespace fwVideoQt
