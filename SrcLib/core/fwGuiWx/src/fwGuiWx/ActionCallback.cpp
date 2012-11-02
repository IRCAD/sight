/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwGui/registry/macros.hpp>

#include "fwGuiWx/ActionCallback.hpp"


fwGuiRegisterMacro( ::fwGuiWx::ActionCallback, ::fwGui::ActionCallbackBase::REGISTRY_KEY );

namespace fwGuiWx
{

//-----------------------------------------------------------------------------

ActionCallback::ActionCallback(::fwGui::GuiBaseObject::Key key)
{}

//-----------------------------------------------------------------------------

ActionCallback::~ActionCallback()
{}

//-----------------------------------------------------------------------------

void ActionCallback::executeWx(wxCommandEvent& event)
{
    this->check(event.IsChecked());
    this->execute();
}

} // namespace fwGuiWx


