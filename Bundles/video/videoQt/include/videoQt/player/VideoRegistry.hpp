/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_PLAYER_VIDEOREGISTRY_HPP__
#define __VIDEOQT_PLAYER_VIDEOREGISTRY_HPP__

#include "videoQt/config.hpp"

#include <arData/Camera.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace videoQt
{
namespace player
{

class QVideoPlayer;

class VIDEOQT_CLASS_API VideoRegistry
{
public:

    VIDEOQT_API static VideoRegistry& getInstance();

    VIDEOQT_API QVideoPlayer* requestPlayer(const ::arData::Camera::csptr& camera);

    VIDEOQT_API void releasePlayer(QVideoPlayer* player);

protected:

    VIDEOQT_API VideoRegistry();

    VIDEOQT_API virtual ~VideoRegistry();

private:

    struct Key
    {
        ::arData::Camera::SourceType m_eType;
        std::string m_strID;

        static bool less( const Key& _rKeyA, const Key& _rKeyB )
        {
            return _rKeyA.m_eType < _rKeyB.m_eType
                   ||  (_rKeyA.m_eType == _rKeyB.m_eType && _rKeyA.m_strID < _rKeyB.m_strID);
        }
    };

    typedef bool (*KeyCompareFuncPtr) (const Key&, const Key&);
    typedef std::map< Key, QVideoPlayer*, KeyCompareFuncPtr> MapVideoPlayerType;
    typedef std::map< Key, std::uint8_t, KeyCompareFuncPtr> MapRefCountType;

    MapVideoPlayerType m_mapVideoPlayer;
    MapRefCountType m_mapRefCount;
};

} //namespace player
} //namespace videoQt

#endif // __VIDEOQT_PLAYER_VIDEOREGISTRY_HPP__
