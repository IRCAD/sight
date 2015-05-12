/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_RESECTIONMSG_HPP__
#define __FWCOMED_RESECTIONMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for resection : store modification information
 * @class   ResectionMsg
 *
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ResectionMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((ResectionMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< ResectionMsg >);

    /**
     * @name Event identifier
     */
    /// @{
    /// @brief Event identifier used to inform for modification
    FWCOMED_API static std::string ADD_RECONSTRUCTION;
    FWCOMED_API static std::string VISIBILITY;
    FWCOMED_API static std::string NEW_RECONSTRUCTION_SELECTED;
    FWCOMED_API static std::string MODIFIED;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API ResectionMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~ResectionMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_RESECTIONMSG_HPP__

