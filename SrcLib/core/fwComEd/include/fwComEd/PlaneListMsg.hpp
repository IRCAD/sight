/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_PLANELISTMSG_HPP_
#define _FWCOMED_PLANELISTMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for PlaneList : store modification information
 * @class   PlaneListMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API PlaneListMsg : public ::fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((PlaneListMsg)(::fwServices::ObjectMsg), ( () ), new PlaneListMsg );

    /**
     * @name Event identifier
     * @{
     */
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string ADD_PLANE;
    FWCOMED_API static std::string REMOVE_PLANE;
    FWCOMED_API static std::string PLANELIST_VISIBILITY;
    FWCOMED_API static std::string PLANELIST_MODIFIED;
    FWCOMED_API static std::string DESELECT_ALL_PLANES;
    /// @}

    /**
      * @brief  Constructor : does nothing.
      */
    FWCOMED_API PlaneListMsg() throw();

    /**
      * @brief  Destuctor : does nothing.
      */
    FWCOMED_API virtual ~PlaneListMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_PLANELISTMSG_HPP_

