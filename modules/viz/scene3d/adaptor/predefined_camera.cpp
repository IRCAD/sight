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

#include "modules/viz/scene3d/adaptor/predefined_camera.hpp"

#include <core/com/slots.hxx>

#include <geometry/data/matrix4.hpp>

#include <viz/scene3d/Utils.hpp>

namespace sight::module::viz::scene3d::adaptor
{

predefined_camera::predefined_camera() noexcept
{
    new_slot(slots::SET_PARAMETER, &predefined_camera::setParameter, this);
    new_slot(
        slots::NEXT_POSITION,
        [this]()
        {
            if(m_interactor)
            {
                m_interactor->nextPosition();
            }
        });

    new_slot(
        slots::PREVIOUS_POSITION,
        [this]()
        {
            if(m_interactor)
            {
                m_interactor->previousPosition();
            }
        });
    new_slot(slots::UPDATE_TRANSFORM, &predefined_camera::updateTransform, this);
}

//-----------------------------------------------------------------------------

void predefined_camera::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    m_priority            = config.get<int>(s_CONFIG + "priority", m_priority);
    m_layerOrderDependant = config.get<bool>(s_CONFIG + "layerOrderDependant", m_layerOrderDependant);

    m_manualRotation = config.get<bool>(s_CONFIG + "mouseRotation", m_manualRotation);

    const auto& defaultPosition = config.get_optional<std::string>(s_CONFIG + "defaultPosition");
    m_defaultPosition = defaultPosition ? std::make_optional(*defaultPosition) : std::nullopt;

    m_animate = config.get<bool>(s_CONFIG + "animate", m_animate);

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

void predefined_camera::starting()
{
    this->initialize();

    const auto layer = this->getLayer();

    m_interactor =
        std::make_shared<sight::viz::scene3d::interactor::predefined_position_interactor>(
            layer,
            m_layerOrderDependant,
            m_cameraPositions,
            m_defaultPosition,
            m_animate
        );

    m_interactor->setMouseRotation(m_manualRotation);

    layer->addInteractor(m_interactor, m_priority);

    if(const auto& transform = m_transform.const_lock(); transform)
    {
        const auto ogre_mat = ::sight::viz::scene3d::Utils::convertTM3DToOgreMx(transform.get_shared());

        m_interactor->setTransform(ogre_mat);
    }

    this->requestRender();
}

//-----------------------------------------------------------------------------

void predefined_camera::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void predefined_camera::stopping()
{
    const auto layer = this->getLayer();
    layer->removeInteractor(m_interactor);
    m_interactor.reset();
}

//------------------------------------------------------------------------------

void predefined_camera::setParameter(ui::parameter_t value, std::string key)
{
    if(m_interactor)
    {
        m_interactor->setParameter(value, key);
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::updateTransform()
{
    if(const auto& transform = m_transform.const_lock(); transform)
    {
        const auto ogre_mat = ::sight::viz::scene3d::Utils::convertTM3DToOgreMx(transform.get_shared());

        m_interactor->setTransform(ogre_mat);
    }
}

//-----------------------------------------------------------------------------

predefined_camera::connections_t predefined_camera::auto_connections() const
{
    return {{s_REGISTRATION_TRANSFORM_IN, sight::data::matrix4::MODIFIED_SIG, slots::UPDATE_TRANSFORM}};
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
