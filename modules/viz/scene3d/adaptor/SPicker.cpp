/************************************************************************
 *
 * Copyright (C) 2019-2022 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SPicker.hpp"

#include <core/com/Signals.hpp>

namespace sight::module::viz::scene3d::adaptor
{

static const core::com::Signals::SignalKeyType s_PICKED_SIG = "picked";

//-----------------------------------------------------------------------------

SPicker::SPicker() noexcept
{
    m_pickedSig = newSignal<sight::viz::scene3d::interactor::MeshPickerInteractor::PointClickedSigType>(s_PICKED_SIG);
}

//-----------------------------------------------------------------------------

void SPicker::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    static const std::string s_PRIORITY_CONFIG              = s_CONFIG + "priority";
    static const std::string s_QUERY_MASK_CONFIG            = s_CONFIG + "queryMask";
    static const std::string s_LAYER_ORDER_DEPENDANT_CONFIG = s_CONFIG + "layerOrderDependant";

    m_priority            = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_layerOrderDependant = config.get<bool>(s_LAYER_ORDER_DEPENDANT_CONFIG, m_layerOrderDependant);

    const std::string hexaMask = config.get<std::string>(s_QUERY_MASK_CONFIG, "");
    if(!hexaMask.empty())
    {
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryMask = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }
}

//-----------------------------------------------------------------------------

void SPicker::starting()
{
    this->initialize();

    const auto layer = this->getLayer();
    m_interactor = std::make_shared<sight::viz::scene3d::interactor::MeshPickerInteractor>(
        layer,
        m_layerOrderDependant
    );
    m_interactor->setQueryMask(m_queryMask);
    m_interactor->setPointClickedSig(m_pickedSig);

    layer->addInteractor(m_interactor, m_priority);
}

//-----------------------------------------------------------------------------

void SPicker::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void SPicker::stopping()
{
    const auto layer = this->getLayer();
    layer->removeInteractor(m_interactor);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
