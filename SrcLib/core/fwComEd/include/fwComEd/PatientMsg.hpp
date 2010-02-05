/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_PATIENTMSG_HPP_
#define _FWCOMED_PATIENTMSG_HPP_


#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief       Object event message specialized for Patient : store modification information.
 * @class       PatientMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see         ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API PatientMsg : public ::fwServices::ObjectMsg
{
public:
        fwCoreClassDefinitionsWithFactoryMacro((PatientMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new PatientMsg );

        /// Event identifier used to inform new patient creation
        FWCOMED_API static std::string NEW_PATIENT;

        /// Event identifier used to inform that a new material is created for reconstruction
        FWCOMED_API static std::string NEW_MATERIAL_FOR_RECONSTRUCTION;

        /**
         * @brief       Constructor : does nothing.
         */
        FWCOMED_API PatientMsg() throw();

        /**
         * @brief       Destructor : does nothing.
         */
        FWCOMED_API virtual ~PatientMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_PATIENTMSG_HPP_

