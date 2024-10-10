/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/trackball_camera.hpp"

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

void trackball_camera::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    m_priority              = config.get<int>(CONFIG + "priority", m_priority);
    m_layer_order_dependant = config.get<bool>(CONFIG + "layerOrderDependant", m_layer_order_dependant);
}

//-----------------------------------------------------------------------------

void trackball_camera::starting()
{
    adaptor::init();

    const auto layer = this->layer();
    m_trackball =
        std::make_shared<sight::viz::scene3d::interactor::trackball_interactor>(layer, m_layer_order_dependant);

    layer->add_interactor(m_trackball, m_priority);
}

//-----------------------------------------------------------------------------

void trackball_camera::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void trackball_camera::stopping()
{
    const auto layer = this->layer();
    layer->remove_interactor(m_trackball);

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
