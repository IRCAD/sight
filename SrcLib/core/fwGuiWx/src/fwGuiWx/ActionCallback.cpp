/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiWx/ActionCallback.hpp"


REGISTER_BINDING( ::fwGui::ActionCallbackBase,
        ::fwGuiWx::ActionCallback,
         ::fwGui::ActionCallbackBase::RegistryKeyType,
          ::fwGui::ActionCallbackBase::REGISTRY_KEY );

namespace fwGuiWx
{

//-----------------------------------------------------------------------------

ActionCallback::ActionCallback()
{}

//-----------------------------------------------------------------------------

ActionCallback::~ActionCallback()
{}

//-----------------------------------------------------------------------------

void ActionCallback::executeWx(wxCommandEvent& event)
{
    this->execute();
}

} // namespace fwGuiWx


