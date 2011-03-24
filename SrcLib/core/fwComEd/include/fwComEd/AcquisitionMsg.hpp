/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_ACQUISITIONMSG_HPP_
#define _FWCOMED_ACQUISITIONMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for acquisition : store modification information
 * @class   AcquisitionMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API AcquisitionMsg : public ::fwServices::ObjectMsg
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((AcquisitionMsg)(::fwServices::ObjectMsg), ( () ), new AcquisitionMsg );

    /**
     * @name Event identifier
     */
    /// @{
    /// Event identifier used to inform for modification
    FWCOMED_API static std::string ADD_RECONSTRUCTION;
    FWCOMED_API static std::string REMOVED_RECONSTRUCTIONS;
    FWCOMED_API static std::string SHOW_RECONSTRUCTIONS;
    FWCOMED_API static std::string VISIBILITY;
    FWCOMED_API static std::string NEW_RECONSTRUCTION_SELECTED;
    /// @}

    /**
     * @brief   Constructor : do nothing.
     */
    FWCOMED_API AcquisitionMsg() throw();

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~AcquisitionMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_ACQUISITIONMSG_HPP_

