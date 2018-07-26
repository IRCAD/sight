/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
