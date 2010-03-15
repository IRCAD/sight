/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_INTEGERMSG_HPP_
#define _FWCOMED_INTEGERMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for integer field : store modification information
 * @class   IntegerMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API IntegerMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((IntegerMsg)(::fwServices::ObjectMsg::Baseclass), ( ( ) ), new IntegerMsg );

    /// Event identifier used to inform that the value is modified.
    FWCOMED_API static std::string VALUE_IS_MODIFIED;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API IntegerMsg() throw();

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~IntegerMsg() throw();
};


} // namespace fwComEd

#endif //_FWCOMED_INTEGERMSG_HPP_

