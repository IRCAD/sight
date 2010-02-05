/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TAGMSG_HPP_
#define _FWCOMED_TAGMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/export.hpp"


namespace fwComEd
{

/**
 * @brief       Object event message specialized for Tag : store modification information
 * @class       TagMsg
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 * @see         ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API TagMsg : public ::fwServices::ObjectMsg
{

public:

        fwCoreClassDefinitionsWithFactoryMacro((TagMsg)(::fwServices::ObjectMsg::Baseclass), ( () ), new TagMsg );

        /// Event identifier used to inform for modification
        FWCOMED_API static std::string TAG_IS_MODIFIED;

        /**
          * @brief      Constructor : do nothing.
          */
        FWCOMED_API TagMsg() throw();

        /**
          * @brief      Destuctor : do nothing.
          */
        FWCOMED_API virtual ~TagMsg() throw();

};

} // namespace fwComEd

#endif //_FWCOMED_TAGMSG_HPP_

