/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassFactoryRegistry.hpp>

#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"

namespace fwRenderVTK
{

//-----------------------------------------------------------------------------

const IVtkRenderWindowInteractorManager::FactoryRegistryKeyType IVtkRenderWindowInteractorManager::REGISTRY_KEY = "::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY";

//-----------------------------------------------------------------------------

IVtkRenderWindowInteractorManager::sptr IVtkRenderWindowInteractorManager::createManager()
{
    IVtkRenderWindowInteractorManager::sptr manager = ::fwTools::ClassFactoryRegistry::create< ::fwRenderVTK::IVtkRenderWindowInteractorManager >( ::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY );
    SLM_ASSERT("Sorry, The factory process to create an IVtkRenderWindowInteractorManager, failed. Missing Qt or Wx component ?", manager );
    return manager;
}

//-----------------------------------------------------------------------------

IVtkRenderWindowInteractorManager::IVtkRenderWindowInteractorManager()
{}

//-----------------------------------------------------------------------------

IVtkRenderWindowInteractorManager::~IVtkRenderWindowInteractorManager()
{}

//-----------------------------------------------------------------------------

} // namespace fwRenderVTK



