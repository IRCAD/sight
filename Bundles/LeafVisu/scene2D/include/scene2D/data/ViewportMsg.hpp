/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_DATA_VIEWPORTMSG_HPP_
#define _SCENE2D_DATA_VIEWPORTMSG_HPP_

#include <fwServices/ObjectMsg.hpp>

#include "scene2D/config.hpp"


namespace scene2D
{
namespace data
{


/**
 * @brief   Object event message specialized for viewport.
 */
class SCENE2D_CLASS_API ViewportMsg : public ::fwServices::ObjectMsg
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ViewportMsg) ( ::fwServices::ObjectMsg), ( () ), new ViewportMsg );

    /**
     * @brief Constructor : does nothing.
     */
    SCENE2D_API ViewportMsg() throw();

    /**
     * @brief   Destuctor : does nothing.
     */
    SCENE2D_API virtual ~ViewportMsg() throw();

    /**
     * Event identifier used to inform for any modification among viewport parameters.
     */
    SCENE2D_API static std::string VALUE_IS_MODIFIED;
};


}   // namespace data
}   // namespace scene2D

#endif  // _SCENE2D_DATA_VIEWPORTMSG_HPP_

