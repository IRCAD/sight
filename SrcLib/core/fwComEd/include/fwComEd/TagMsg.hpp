/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TAGMSG_HPP_
#define _FWCOMED_TAGMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for Tag : store modification information
 * @class   TagMsg
 * 
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API TagMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((TagMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< TagMsg >);

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string TAG_IS_MODIFIED;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API TagMsg(::fwServices::ObjectMsg::Key key);

    /**
      * @brief  Destuctor : do nothing.
      */
    FWCOMED_API virtual ~TagMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_TAGMSG_HPP_

