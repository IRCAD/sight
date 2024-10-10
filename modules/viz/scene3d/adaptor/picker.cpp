/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/picker.hpp"

#include <core/com/signals.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::signals::key_t PICKED_SIG = "picked";

//-----------------------------------------------------------------------------

picker::picker() noexcept
{
    m_picked_sig = new_signal<sight::viz::scene3d::interactor::mesh_picker_interactor::point_clicked_sig_t>(PICKED_SIG);
}

//-----------------------------------------------------------------------------

void picker::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_PRIORITY_CONFIG              = CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG            = CONFIG + "queryMask";
    static const std::string s_LAYER_ORDER_DEPENDANT_CONFIG = CONFIG + "layerOrderDependant";

    m_priority              = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_layer_order_dependant = config.get<bool>(s_LAYER_ORDER_DEPENDANT_CONFIG, m_layer_order_dependant);

    const std::string hexa_mask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexa_mask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexa_mask,
            hexa_mask.length() > 2
            && hexa_mask.substr(0, 2) == "0x"
        );
        m_query_mask = static_cast<std::uint32_t>(std::stoul(hexa_mask, nullptr, 16));
    }
}

//-----------------------------------------------------------------------------

void picker::starting()
{
    adaptor::init();

    const auto layer = this->layer();
    m_interactor = std::make_shared<sight::viz::scene3d::interactor::mesh_picker_interactor>(
        layer,
        m_layer_order_dependant
    );
    m_interactor->set_query_mask(m_query_mask);
    m_interactor->set_point_clicked_sig(m_picked_sig);

    layer->add_interactor(m_interactor, m_priority);
}

//-----------------------------------------------------------------------------

void picker::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void picker::stopping()
{
    const auto layer = this->layer();
    layer->remove_interactor(m_interactor);

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
