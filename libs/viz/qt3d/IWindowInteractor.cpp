/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "IWindowInteractor.hpp"

namespace sight::viz::qt3d
{

//-----------------------------------------------------------------------------

const IWindowInteractor::FactoryRegistryKeyType IWindowInteractor::REGISTRY_KEY =
    "::viz::qt3d::IWindowInteractor::REGISTRY_KEY";

//-----------------------------------------------------------------------------

IWindowInteractor::sptr IWindowInteractor::createManager()
{
    IWindowInteractor::sptr manager = viz::qt3d::factory::New(
        viz::qt3d::IWindowInteractor::REGISTRY_KEY );
    SLM_ASSERT(
        "The factory process to create an IWindowInteractor, failed. Missing Qt or Wx component ?",
        manager );
    return manager;
}

//-----------------------------------------------------------------------------

IWindowInteractor::IWindowInteractor()
{
}

//-----------------------------------------------------------------------------

IWindowInteractor::~IWindowInteractor()
{
}

} // namespace sight::viz::qt3d.
