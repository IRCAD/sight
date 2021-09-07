/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#pragma once

#include "viz/scene3d/ILight.hpp"
#include "viz/scene3d/IWindowInteractor.hpp"
#include "viz/scene3d/registry/detail.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace sight::viz::scene3d
{

namespace registry
{

#define fwRenderOgreRegisterMacro(OgreRenderInteractorMngClassname, FunctorKey) \
    static sight::viz::scene3d::IWindowInteractor::Registry<OgreRenderInteractorMngClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

#define fwRenderOgreRegisterOffscreenMgrMacro(OgreRenderInteractorMngClassname, FunctorKey) \
    static sight::viz::scene3d::IWindowInteractor::OffscreenMgrRegistry<OgreRenderInteractorMngClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

#define fwRenderOgreRegisterInteractorMacro(OgreInteractorClassname) \
    static sight::viz::scene3d::interactor::IInteractor::Registry<OgreInteractorClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(BOOST_PP_STRINGIZE(OgreInteractorClassname));

#define fwRenderOgreRegisterLightMacro(OgreLightClassname, FunctorKey) \
    static sight::viz::scene3d::ILight::Registry<OgreLightClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

} // end namespace registry

} // end namespace sight::viz::scene3d
