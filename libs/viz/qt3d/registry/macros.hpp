/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#pragma once

#include "viz/qt3d/registry/detail.hpp"
#include "viz/qt3d/window_interactor.hpp"

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

namespace sight::viz::qt3d::registry
{

#define SIGHT_REGISTER_RENDER_QT3D(Qt3DRenderInteractorMngClassname, FunctorKey) \
        static sight::viz::qt3d::window_interactor::registry<Qt3DRenderInteractorMngClassname> \
        BOOST_PP_CAT(s__factory__record__, __LINE__)(FunctorKey);

} // namespace sight::viz::qt3d::registry
