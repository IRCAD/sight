/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_COLORMSG_HPP_
#define _FWCOMED_COLORMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for Color : store modification information
 * @class   ColorMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API ColorMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ColorMsg)(::fwServices::ObjectMsg), ( ( ) ), new ColorMsg );

    /// Event identifier used to inform that the value is modified.
    FWCOMED_API static std::string VALUE_IS_MODIFIED;

    /**
     * @brief   Constructor : does nothing.
     */
    FWCOMED_API ColorMsg() throw();

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~ColorMsg() throw();
};


} // namespace fwComEd

#endif //_FWCOMED_COLORMSG_HPP_

