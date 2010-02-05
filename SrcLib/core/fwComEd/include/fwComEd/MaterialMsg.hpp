/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_MATERIALMSG_HPP_
#define _FWCOMED_MATERIALMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief       Object event message specialized for material : store modification information
 * @class       MaterialMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 * @see         ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API MaterialMsg : public ::fwServices::ObjectMsg
{
public:
        fwCoreClassDefinitionsWithFactoryMacro((MaterialMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new MaterialMsg );

        /**
         * @name Event identifier
         */
        /// @{
        /// Event identifier used to inform for modification
        FWCOMED_API static std::string MATERIAL_IS_MODIFIED;
        /// @}

        /**
         * @brief       Constructor : do nothing.
         */
        FWCOMED_API MaterialMsg() throw();

        /**
         * @brief       Destuctor : do nothing.
         */
        FWCOMED_API virtual ~MaterialMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_MATERIALMSG_HPP_

