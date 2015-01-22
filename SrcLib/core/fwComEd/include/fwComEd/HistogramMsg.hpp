/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_FLOAT_MSG_HPP_
#define _FWCOMED_FLOAT_MSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for histogram field.
 * @class   HistogramMsg
 * 
 * @date    2011.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API HistogramMsg : public fwServices::ObjectMsg
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((HistogramMsg)(::fwServices::ObjectMsg), ( ( ) ),
                                           ::fwServices::factory::message::New< HistogramMsg >);

    /// Event identifier used to inform that the value is modified.
    FWCOMED_API static std::string VALUE_IS_MODIFIED;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API HistogramMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~HistogramMsg() throw();
};


} // namespace fwComEd

#endif //_FWCOMED_HISTOGRAM_MSG_HPP_

