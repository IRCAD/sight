/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/Application.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

Application::sptr Application::factory()
{
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IApplication::REGISTRY_KEY);
    static Application::sptr app = ::fwGui::Application::dynamicCast(guiObj);
    SLM_ASSERT("no implementation of " << ::fwGui::IApplication::REGISTRY_KEY, app );
    return app;
}

} // namespace fwGui



