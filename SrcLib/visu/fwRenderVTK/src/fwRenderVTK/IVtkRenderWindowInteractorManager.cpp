/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"

namespace fwRenderVTK
{

//-----------------------------------------------------------------------------

const IVtkRenderWindowInteractorManager::FactoryRegistryKeyType IVtkRenderWindowInteractorManager::REGISTRY_KEY =
    "::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY";

//-----------------------------------------------------------------------------

IVtkRenderWindowInteractorManager::sptr IVtkRenderWindowInteractorManager::createManager()
{
    IVtkRenderWindowInteractorManager::sptr manager = ::fwRenderVTK::factory::New(
        ::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY );
    SLM_ASSERT("The factory process to create an IVtkRenderWindowInteractorManager, failed."
               "Missing Qt or Wx component ?", manager );
    return manager;
}

//-----------------------------------------------------------------------------

IVtkRenderWindowInteractorManager::IVtkRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

IVtkRenderWindowInteractorManager::~IVtkRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

} // namespace fwRenderVTK



