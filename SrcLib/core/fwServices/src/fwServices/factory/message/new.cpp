/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/ObjectMsg.hpp"

#include "fwServices/factory/message/new.hpp"


namespace fwServices
{

namespace factory
{

namespace message
{

::fwServices::ObjectMsg::sptr New( const ::fwServices::registry::message::KeyType & classname )
{
    ::fwServices::ObjectMsg::sptr obj = ::fwServices::registry::message::get()->create(classname);
    return obj;
}

} // namespace message

} // namespace factory

} // namespace fwServices


