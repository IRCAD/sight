/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_MODELMSG_HPP_
#define _FWCOMED_MODELMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for model : store modification information
 * @class   ModelMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ModelMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ModelMsg)(::fwServices::ObjectMsg), ( ( ) ), new ModelMsg );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_MODEL;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API ModelMsg() throw();

    /**
     * @brief   Destuctor : does nothing.
     */
    FWCOMED_API virtual ~ModelMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_MODELMSG_HPP_

