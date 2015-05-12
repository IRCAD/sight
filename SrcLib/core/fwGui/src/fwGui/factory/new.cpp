/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/factory/new.hpp"


namespace fwGui
{

namespace factory
{

::fwGui::GuiBaseObject::sptr New( const ::fwGui::registry::KeyType & classname )
{
    return ::fwGui::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwGui


