/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __POC07TIMELINE_MESSAGETL_HPP__
#define __POC07TIMELINE_MESSAGETL_HPP__

#include "extData/GenericTL.hpp"
#include "extData/GenericTL.hxx"
#include "extData/timeline/GenericObject.hpp"
#include "extData/timeline/GenericObject.hxx"

#include "PoC07TimeLine/config.hpp"

namespace PoC07TimeLine
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
 * @class   MessageTL
 * @brief   This class defines a timeline that stores string messages.
 */
class POC07TIMELINE_CLASS_API MessageTL : public ::extData::GenericTL< MsgData >
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (MessageTL)(::extData::TimeLine),(()), ::fwData::factory::New< MessageTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    POC07TIMELINE_API MessageTL( ::fwData::Object::Key key ) : GenericTL< MsgData >(key) {}
};

} // namespace PoC07TimeLine


#endif // __POC07TIMELINE_MESSAGETL_HPP__
