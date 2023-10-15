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

#include "viz/scene3d/factory/new.hpp"

#include "viz/scene3d/ILight.hpp"
#include "viz/scene3d/IText.hpp"
#include "viz/scene3d/window_interactor.hpp"

namespace sight::viz::scene3d
{

namespace factory
{

//------------------------------------------------------------------------------

viz::scene3d::window_interactor::sptr make(const viz::scene3d::registry::key_t& _classname)
{
    return viz::scene3d::registry::get()->create(_classname);
}

} // namespace factory

namespace offscreenInteractorMgrFactory
{

//------------------------------------------------------------------------------

viz::scene3d::window_interactor::sptr make(
    const viz::scene3d::registry::key_t& _classname,
    std::pair<unsigned int, unsigned int> _dims
)
{
    return viz::scene3d::registry::getOffscreenMgr()->create(_classname, _dims);
}

} // namespace offscreenInteractorMgrFactory

namespace lightFactory
{

//------------------------------------------------------------------------------

viz::scene3d::ILight::sptr make(const viz::scene3d::registry::key_t& _classname)
{
    return viz::scene3d::registry::getLightRegistry()->create(_classname);
}

} // namespace lightFactory

namespace textFactory
{

//------------------------------------------------------------------------------

viz::scene3d::IText::sptr make(
    const viz::scene3d::registry::key_t& _classname,
    const sight::viz::scene3d::Layer::sptr& _layer
)
{
    return viz::scene3d::registry::getTextRegistry()->create(_classname, _layer);
}

} // namespace textFactory

} // namespace sight::viz::scene3d
