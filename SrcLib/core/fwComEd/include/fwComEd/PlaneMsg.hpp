/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_PLANEMSG_HPP__
#define __FWCOMED_PLANEMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for PlaneList : store modification information
 * @class   PlaneMsg
 *
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API PlaneMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((PlaneMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< PlaneMsg > );

    /**
     * @name Event identifier
     * @{
     */
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string PLANE_MODIFIED;
    FWCOMED_API static std::string START_PLANE_INTERACTION;
    FWCOMED_API static std::string DESELECT_PLANE;
    FWCOMED_API static std::string WAS_SELECTED;
    FWCOMED_API static std::string WAS_DESELECTED;
    /// @}

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API PlaneMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief  Destuctor : does nothing.
     */
    FWCOMED_API virtual ~PlaneMsg() throw();

};

} // namespace fwComEd

#endif //__FWCOMED_PLANEMSG_HPP__


