/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_BOOLEANMSG_HPP__
#define __FWCOMED_BOOLEANMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for Boolean : store modification information
 * @class   BooleanMsg
 *
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API BooleanMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((BooleanMsg)(::fwServices::ObjectMsg), ( ( ) ),
                                           ::fwServices::factory::message::New< BooleanMsg > );

    /// Event identifier used to inform that the value is modified.
    FWCOMED_API static std::string VALUE_IS_MODIFIED;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API BooleanMsg(::fwServices::ObjectMsg::Key key);


    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~BooleanMsg() throw();

};


} // namespace fwComEd

#endif //__FWCOMED_BOOLEANMSG_HPP__

