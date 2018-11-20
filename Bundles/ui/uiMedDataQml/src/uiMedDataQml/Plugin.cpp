/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQml/Plugin.hpp"

#include "uiMedDataQml/OrganListModel.hpp"
#include "uiMedDataQml/SModelSeriesList.hpp"

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace uiMedDataQml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiMedDataQml::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<SModelSeriesList>("uiMedDataQml", 1, 0, "SModelSeriesList");
    qmlRegisterType<OrganListModel>("uiMedDataQml", 1, 0, "OrganListModel");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    auto path = ::fwRuntime::getBundleResourcePath("uiMedDataQml");

    engine->importModulePath(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace uiMedDataQml
