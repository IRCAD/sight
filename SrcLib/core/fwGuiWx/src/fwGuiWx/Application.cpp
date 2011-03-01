/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>

#include <fwTools/ClassRegistrar.hpp>

#include "fwGuiWx/Application.hpp"


REGISTER_BINDING( ::fwGui::Application, ::fwGuiWx::Application, ::fwGui::IApplication::FactoryRegistryKeyType , ::fwGui::IApplication::REGISTRY_KEY );

namespace fwGuiWx
{
//-----------------------------------------------------------------------------

void Application::exit(int returncode)
{
    SLM_WARN_IF("returncode is actually ignored in fwGuiWx", returncode != 0);
    wxExit();
}

//-----------------------------------------------------------------------------

} // namespace fwGuiWx



