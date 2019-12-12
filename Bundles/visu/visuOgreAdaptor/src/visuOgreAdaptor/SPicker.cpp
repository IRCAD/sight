/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SPicker.hpp"

#include <fwCom/Signals.hpp>

static const ::fwCom::Signals::SignalKeyType s_PICKED_SIG = "picked";

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

SPicker::SPicker() noexcept
{
    m_pickedSig = newSignal< ::fwRenderOgre::interactor::MeshPickerInteractor::PointClickedSigType >(s_PICKED_SIG);
}

//-----------------------------------------------------------------------------

SPicker::~SPicker() noexcept
{

}

//-----------------------------------------------------------------------------

void SPicker::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_pickingPriority = config.get<int>("priority", m_pickingPriority);
    const std::string hexaMask = config.get<std::string>("queryMask");

    if(!hexaMask.empty())
    {
        SLM_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2 &&
            hexaMask.substr(0, 2) == "0x");
        m_queryMask = static_cast< std::uint32_t >(std::stoul(hexaMask, nullptr, 16));
    }

}

//-----------------------------------------------------------------------------

void SPicker::starting()
{
    this->initialize();

    const auto layer = this->getLayer();
    m_interactor = std::make_shared< ::fwRenderOgre::interactor::MeshPickerInteractor >(layer);
    m_interactor->setQueryMask(m_queryMask);
    m_interactor->setPointClickedSig(m_pickedSig);

    layer->addInteractor(m_interactor, m_pickingPriority);

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

} //namespace visuOgreAdaptor
