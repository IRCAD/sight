/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_VIDEOMSG_HPP__
#define __FWCOMED_VIDEOMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for Video : store modification information
 * @class   VideoMsg
 *
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 * @todo specialize it when Video class is clearly defined
 */
class FWCOMED_CLASS_API VideoMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((VideoMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< VideoMsg >);

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string VIDEO_IS_REFRESHED;
    FWCOMED_API static std::string VIDEO_IS_INITIALIZED;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API VideoMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief  Destuctor : do nothing.
     */
    FWCOMED_API virtual ~VideoMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_VIDEOMSG_HPP__

