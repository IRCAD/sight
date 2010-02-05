/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_SPLINEMSG_HPP_
#define _FWCOMED_SPLINEMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for Spline : store modification information
 * @class   SplineMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API SplineMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((SplineMsg)(::fwServices::ObjectMsg::Baseclass), ( ( ) ), new SplineMsg );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_SPLINE;

    /**
     * @brief   Constructor : do nothing.
     */
    FWCOMED_API SplineMsg() throw();

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~SplineMsg() throw();

};

} // namespace op

#endif //_FWCOMED_SPLINEMSG_HPP_

