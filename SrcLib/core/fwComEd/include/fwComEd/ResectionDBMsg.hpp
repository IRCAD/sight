/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_RESECTIONDBMSG_HPP_
#define _FWCOMED_RESECTIONDBMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include <fwData/Resection.hpp>
#include <fwData/ResectionDB.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for resectionDB : store modification information
 * @class   ResectionDBMsg
 * 
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ResectionDBMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((ResectionDBMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< ResectionDBMsg >);

    /**
     * @name Event identifier
     */
    /// @{
    /// @brief Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_RESECTIONDB_SELECTED;
    FWCOMED_API static std::string RESECTIONDB_SELECTED;
    FWCOMED_API static std::string NEW_RESECTION_SELECTED;
    FWCOMED_API static std::string NEW_SAFE_PART_SELECTED;
    FWCOMED_API static std::string ADD_RESECTION;
    FWCOMED_API static std::string ADD_SAFE_PART;
    FWCOMED_API static std::string MODIFIED;
    FWCOMED_API static std::string RESECTIONDB_INVALIDATED;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API ResectionDBMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~ResectionDBMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_RESECTIONDBMSG_HPP_

