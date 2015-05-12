/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_SPLINEMSG_HPP__
#define __FWCOMED_SPLINEMSG_HPP__

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object message specialized for Spline : store modification information
 * @class   SplineMsg
 *
 * @date    2007-2009.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API SplineMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((SplineMsg)(::fwServices::ObjectMsg), ( ( ) ),
                                           ::fwServices::factory::message::New< SplineMsg > );

    /// Event identifier used to inform for modification
    FWCOMED_API static std::string NEW_SPLINE;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API SplineMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : do nothing.
     */
    FWCOMED_API virtual ~SplineMsg() throw();

};

} // namespace op

#endif //__FWCOMED_SPLINEMSG_HPP__

