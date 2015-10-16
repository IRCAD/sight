/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_DATA_VIEWPORTMSG_HPP__
#define __SCENE2D_DATA_VIEWPORTMSG_HPP__

#include "scene2D/config.hpp"

#include <fwServices/ObjectMsg.hpp>

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
    fwCoreClassDefinitionsWithFactoryMacro( (ViewportMsg) ( ::fwServices::ObjectMsg), ( () ),
                                            ::fwServices::factory::message::New<ViewportMsg> );

    /**
     * @brief Constructor : does nothing.
     * @param key Private construction key
     */
    SCENE2D_API ViewportMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destuctor : does nothing.
     */
    SCENE2D_API virtual ~ViewportMsg();

    /**
     * Event identifier used to inform for any modification among viewport parameters.
     */
    SCENE2D_API static std::string VALUE_IS_MODIFIED;
};


}   // namespace data
}   // namespace scene2D

#endif  // __SCENE2D_DATA_VIEWPORTMSG_HPP__

