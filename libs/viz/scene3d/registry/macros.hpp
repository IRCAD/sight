/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/light_adaptor.hpp"
#include "viz/scene3d/registry/detail.hpp"
#include "viz/scene3d/text.hpp"
#include "viz/scene3d/window_interactor.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace sight::viz::scene3d::registry
{

#define SIGHT_REGISTER_SCENE3D(OgreRenderInteractorMngClassname, FunctorKey) \
    static sight::viz::scene3d::window_interactor::Registry<OgreRenderInteractorMngClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

#define SIGHT_REGISTER_SCENE3D_OFFSCREEN(OgreRenderInteractorMngClassname, FunctorKey) \
    static sight::viz::scene3d::window_interactor::OffscreenMgrRegistry<OgreRenderInteractorMngClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

#define SIGHT_REGISTER_SCENE3D_INTERACTOR(OgreInteractorClassname) \
    static sight::viz::scene3d::interactor::base::Registry<OgreInteractorClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(BOOST_PP_STRINGIZE(OgreInteractorClassname));

#define SIGHT_REGISTER_SCENE3D_LIGHT(OgreLightClassname, FunctorKey) \
    static sight::viz::scene3d::light_adaptor::Registry<OgreLightClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

#define SIGHT_REGISTER_SCENE3D_TEXT(OgreTextClassname, FunctorKey) \
    static sight::viz::scene3d::text::Registry<OgreTextClassname> \
    BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

} // namespace sight::viz::scene3d::registry
