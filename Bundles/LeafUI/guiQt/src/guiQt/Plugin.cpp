/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwCore/base.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

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

    SLM_ASSERT("Sorry, the rootObject attribute is deprecated, you must remove this field in the profile.xml. You must also change in the configuration the field implements=\"rootObject\" by implements= \"::fwServices::ServiceObjectConfig\"", ! this->getBundle()->hasParameter("rootObject") );

    if ( this->getBundle()->hasParameter("config")
         && this->getBundle()->hasParameter("configFile") )
    {
        std::string objectConfigurationName( this->getBundle()->getParameterValue("config") ) ;
        std::string objectConfigurationFile( this->getBundle()->getParameterValue("configFile") ) ;

        ::fwServices::OSR::setRootObjectConfigurationName(objectConfigurationName) ;
        ::fwServices::OSR::setRootObjectConfigurationFile(objectConfigurationFile) ;
    }
    else
    {
        SLM_FATAL(" Bundle gui, missing param : rootObject, config, configFile in profile");
    }

    SLM_FATAL_IF("Depreciated parameter Aspect", this->getBundle()->hasParameter("Aspect"));

    if( this->getBundle()->hasParameter("startingMode")
            &&  this->getBundle()->getParameterValue("startingMode") == "windows")
    {
        ::fwRuntime::profile::Profile::sptr profile = ::fwRuntime::profile::getCurrentProfile();
        SLM_ASSERT("Profile is not initialized", profile);
        ::fwRuntime::profile::Profile::ParamsContainer params = profile->getParams();
        int    argc = params.size();
        char** argv = profile->getRawParams();

        ::fwGuiQt::App app( argc, argv );

        app.exec();
    }
    else
    {
        SLM_FATAL("Starting Mode not available");
    }
}

//-----------------------------------------------------------------------------

void Plugin::stop() throw()
{}

} // namespace guiQt
