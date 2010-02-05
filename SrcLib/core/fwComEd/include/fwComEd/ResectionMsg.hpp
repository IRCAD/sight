/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_RESECTIONMSG_HPP_
#define _FWCOMED_RESECTIONMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief       Object event message specialized for resection : store modification information
 * @class       ResectionMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see         ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ResectionMsg : public ::fwServices::ObjectMsg
{
public:
        fwCoreClassDefinitionsWithFactoryMacro((ResectionMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new ResectionMsg );

        /**
         * @name Event identifier
         */
        /// @{
        /// Event identifier used to inform for modification
        FWCOMED_API static std::string ADD_RECONSTRUCTION;
        FWCOMED_API static std::string VISIBILITY;
        FWCOMED_API static std::string NEW_RECONSTRUCTION_SELECTED;
        FWCOMED_API static std::string MODIFIED;
        /// @}

        /**
         * @brief       Constructor : do nothing.
         */
        FWCOMED_API ResectionMsg() throw();

        /**
         * @brief       Destuctor : do nothing.
         */
        FWCOMED_API virtual ~ResectionMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_RESECTIONMSG_HPP_

