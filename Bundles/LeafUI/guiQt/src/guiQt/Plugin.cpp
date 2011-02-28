/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>

#include <fwCore/base.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>

#include <fwGuiQt/App.hpp>

#include "guiQt/Plugin.hpp"

namespace guiQt
{
//-----------------------------------------------------------------------------


static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::guiQt::Plugin");

//-----------------------------------------------------------------------------

Plugin::~Plugin() throw()
{}

//-----------------------------------------------------------------------------

void Plugin::start() throw(::fwRuntime::RuntimeException)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
    SLM_ASSERT("Profile is not initialized", profile);
    ::fwRuntime::profile::Profile::ParamsContainer params = profile->getParams();
    int    argc = params.size();
    char** argv = profile->getRawParams();

    ::fwGuiQt::App *app;
    app = new ::fwGuiQt::App( argc, argv );
    m_app = app;


    ::fwRuntime::profile::getCurrentProfile()->setRunCallback(::boost::bind(&Plugin::run, this));
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{
    delete m_app;
}

//-----------------------------------------------------------------------------

int Plugin::run() throw()
{
    ::fwRuntime::profile::getCurrentProfile()->setup();
    int res = m_app->exec();
    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    return res;
}

} // namespace guiQt
