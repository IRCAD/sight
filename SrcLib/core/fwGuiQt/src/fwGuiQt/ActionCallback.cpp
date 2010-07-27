/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiQt/ActionCallback.hpp"


REGISTER_BINDING( ::fwGui::ActionCallbackBase,
        ::fwGuiQt::ActionCallback,
         ::fwGui::ActionCallbackBase::RegistryKeyType,
          ::fwGui::ActionCallbackBase::REGISTRY_KEY );

namespace fwGuiQt
{

//-----------------------------------------------------------------------------

ActionCallback::ActionCallback()
{}

//-----------------------------------------------------------------------------

ActionCallback::~ActionCallback()
{}

//-----------------------------------------------------------------------------

void ActionCallback::executeQt(bool checked)
{
    this->check(checked);
    this->execute();
}

} // namespace fwGuiQt


