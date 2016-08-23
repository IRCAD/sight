/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EX02TIMELINE_MESSAGETL_HPP__
#define __EX02TIMELINE_MESSAGETL_HPP__

#include <arData/GenericTL.hpp>
#include <arData/GenericTL.hxx>
#include <arData/timeline/GenericObject.hpp>
#include <arData/timeline/GenericObject.hxx>

#include "Ex02TimeLine/config.hpp"

namespace Ex02TimeLine
{

struct MsgData
{
    /// Maximum size of a message
    static const size_t MAX_MSG_SIZE = 255;

    /// Identifier of the sender
    unsigned int uidSender;
    /// Message as a C string
    char szMsg[MAX_MSG_SIZE];
};


/**
 * @brief   This class defines a timeline that stores string messages.
 */
class EX02TIMELINE_CLASS_API MessageTL : public ::arData::GenericTL< MsgData >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MessageTL)(::arData::TimeLine),(()), ::fwData::factory::New< MessageTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MessageTL( ::fwData::Object::Key key ) : GenericTL< MsgData >(key)
    {
    }
};

} // namespace Ex02TimeLine


#endif // __EX02TIMELINE_MESSAGETL_HPP__
