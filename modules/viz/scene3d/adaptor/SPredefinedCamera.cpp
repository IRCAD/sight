/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SPredefinedCamera.hpp"

#include <core/com/Slots.hxx>

#include <geometry/data/Matrix4.hpp>

#include <viz/scene3d/Utils.hpp>

namespace sight::module::viz::scene3d::adaptor
{

SPredefinedCamera::SPredefinedCamera() noexcept
{
    newSlot(slots::s_SET_PARAMETER, &SPredefinedCamera::setParameter, this);
    newSlot(
        slots::s_NEXT_POSITION,
        [this]()
        {
            if(m_interactor)
            {
                m_interactor->nextPosition();
            }
        });

    newSlot(
        slots::s_PREVIOUS_POSITION,
        [this]()
        {
            if(m_interactor)
            {
                m_interactor->previousPosition();
            }
        });
    newSlot(slots::s_UPDATE_TRANSFORM, &SPredefinedCamera::updateTransform, this);
}

//-----------------------------------------------------------------------------

void SPredefinedCamera::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    m_priority            = config.get<int>(s_CONFIG + "priority", m_priority);
    m_layerOrderDependant = config.get<bool>(s_CONFIG + "layerOrderDependant", m_layerOrderDependant);

    m_manualRotation = config.get<bool>(s_CONFIG + "mouseRotation", m_manualRotation);

    const auto& defaultPosition = config.get_optional<std::string>(s_CONFIG + "defaultPosition");
    m_defaultPosition = defaultPosition ? std::make_optional(*defaultPosition) : std::nullopt;

    const auto positions = config.get_child("positions");

    const auto posCfg = positions.equal_range("position");

    // Configure tracked tool
    for(auto t = posCfg.first ; t != posCfg.second ; ++t)
    {
        predefined_position_t position;

        const auto posattr = t->second.get_child("<xmlattr>");

        // 1. Name (should be unique).
        position.name = posattr.get<std::string>("name");

        position.rx = posattr.get<float>("rx", 0.F);
        position.ry = posattr.get<float>("ry", 0.F);
        position.rz = posattr.get<float>("rz", 0.F);

        m_cameraPositions.push_back(position);
    }
}

//-----------------------------------------------------------------------------

void SPredefinedCamera::starting()
{
    this->initialize();

    const auto layer = this->getLayer();

    m_interactor = std::make_shared<sight::viz::scene3d::interactor::PredefinedPositionInteractor>(
        layer,
        m_layerOrderDependant,
        m_cameraPositions,
        m_defaultPosition
    );

    m_interactor->setMouseRotation(m_manualRotation);

    layer->addInteractor(m_interactor, m_priority);

    if(const auto& transform = m_transform.const_lock(); transform)
    {
        const auto ogre_mat = ::sight::viz::scene3d::Utils::convertTM3DToOgreMx(transform.get_shared());

        m_interactor->setTransform(ogre_mat);
    }
}

//-----------------------------------------------------------------------------

void SPredefinedCamera::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void SPredefinedCamera::stopping()
{
    const auto layer = this->getLayer();
    layer->removeInteractor(m_interactor);
}

//------------------------------------------------------------------------------

void SPredefinedCamera::setParameter(ui::base::parameter_t value, std::string key)
{
    if(m_interactor)
    {
        m_interactor->setParameter(value, key);
    }
}

//-----------------------------------------------------------------------------

void SPredefinedCamera::updateTransform()
{
    if(const auto& transform = m_transform.const_lock(); transform)
    {
        const auto ogre_mat = ::sight::viz::scene3d::Utils::convertTM3DToOgreMx(transform.get_shared());

        m_interactor->setTransform(ogre_mat);
    }
}

//-----------------------------------------------------------------------------

SPredefinedCamera::KeyConnectionsMap SPredefinedCamera::getAutoConnections() const
{
    return {{s_REGISTRATION_TRANSFORM_IN, sight::data::Matrix4::s_MODIFIED_SIG, slots::s_UPDATE_TRANSFORM}};
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
