/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiReconstructionQml/Plugin.hpp"

#include "uiReconstructionQml/SOrganMaterialEditor.hpp"
#include "uiReconstructionQml/SRepresentationEditor.hpp"

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>
#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace uiReconstructionQml
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::uiReconstructionQml::Plugin");

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
    qmlRegisterType<SOrganMaterialEditor>("uiReconstructionQml", 1, 0, "SOrganMaterialEditor");
    qmlRegisterType<SRepresentationEditor>("uiReconstructionQml", 1, 0, "SRepresentationEditor");
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();

    auto path = ::fwRuntime::getBundleResourcePath("uiReconstructionQml");

    engine->importModulePath(path);
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

} // namespace uiReconstructionQml
