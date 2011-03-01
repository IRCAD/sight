/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Factory.hpp>

#include "fwGui/Application.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

Application::Application()
{
    //m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::IApplication>( ::fwGui::IApplication::REGISTRY_KEY );
}

//-----------------------------------------------------------------------------

Application::~Application()
{}


Application::sptr Application::factory()
{
    static Application::sptr app = ::fwTools::ClassFactoryRegistry::create< ::fwGui::Application >( ::fwGui::IApplication::REGISTRY_KEY );
    SLM_ASSERT("no implementation of " << ::fwGui::IApplication::REGISTRY_KEY, app );
    return app;
}

} // namespace fwGui



