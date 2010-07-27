/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/GuiRegistry.hpp"
#include "fwGui/registrar/ActionRegistrar.hpp"

namespace fwGui
{
namespace registrar
{

//-----------------------------------------------------------------------------

ActionRegistrar::ActionRegistrar(const std::string sid) : m_sid(sid)
{}

//-----------------------------------------------------------------------------

ActionRegistrar::~ActionRegistrar()
{}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStopping()
{
    ::fwGui::GuiRegistry::actionServiceStopping(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStarting()
{
    ::fwGui::GuiRegistry::actionServiceStarting(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceChecked(bool checked)
{
    ::fwGui::GuiRegistry::actionServiceChecked(m_sid, checked);
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
