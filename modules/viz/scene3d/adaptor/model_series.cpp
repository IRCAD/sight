/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/model_series.hpp"

#include "modules/viz/scene3d/adaptor/mesh.hpp"
#include "modules/viz/scene3d/adaptor/reconstruction.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/boolean.hpp>
#include <data/material.hpp>
#include <data/matrix4.hpp>
#include <data/reconstruction.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

static const core::com::slots::key_t CHANGE_FIELD_SLOT = "changeField";

//------------------------------------------------------------------------------

model_series::model_series() noexcept
{
    new_slot(CHANGE_FIELD_SLOT, &model_series::showReconstructionsOnFieldChanged, this);
}

//------------------------------------------------------------------------------

void model_series::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::transformable::s_TRANSFORM_CONFIG,
            this->get_id() + "_transform"
        )
    );

    static const std::string s_AUTORESET_CAMERA_CONFIG = s_CONFIG + "autoresetcamera";
    static const std::string s_MATERIAL_CONFIG         = s_CONFIG + "material";
    static const std::string s_DYNAMIC_CONFIG          = s_CONFIG + "dynamic";
    static const std::string s_DYNAMIC_VERTICES_CONFIG = s_CONFIG + "dynamicVertices";
    static const std::string s_QUERY_CONFIG            = s_CONFIG + "queryFlags";

    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    m_materialTemplateName = config.get<std::string>(s_MATERIAL_CONFIG, m_materialTemplateName);
    m_isDynamic            = config.get<bool>(s_DYNAMIC_CONFIG, m_isDynamic);
    m_isDynamicVertices    = config.get<bool>(s_DYNAMIC_VERTICES_CONFIG, m_isDynamicVertices);

    if(config.count(s_QUERY_CONFIG) != 0U)
    {
        const auto hexaMask = config.get<std::string>(s_QUERY_CONFIG);
        SIGHT_ASSERT(
            "Hexadecimal values should start with '0x'"
            "Given value : " + hexaMask,
            hexaMask.length() > 2
            && hexaMask.substr(0, 2) == "0x"
        );
        m_queryFlags = static_cast<std::uint32_t>(std::stoul(hexaMask, nullptr, 16));
    }

    if(config.get_optional<bool>("visible"))
    {
        m_isVisibleTag = true;
    }
}

//------------------------------------------------------------------------------

void model_series::starting()
{
    this->initialize();

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t model_series::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_MODEL_INPUT, data::model_series::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_MODEL_INPUT, data::model_series::RECONSTRUCTIONS_ADDED_SIG, service::slots::UPDATE);
    connections.push(s_MODEL_INPUT, data::model_series::RECONSTRUCTIONS_REMOVED_SIG, service::slots::UPDATE);
    connections.push(s_MODEL_INPUT, data::model_series::ADDED_FIELDS_SIG, CHANGE_FIELD_SLOT);
    connections.push(s_MODEL_INPUT, data::model_series::REMOVED_FIELDS_SIG, CHANGE_FIELD_SLOT);
    connections.push(s_MODEL_INPUT, data::model_series::CHANGED_FIELDS_SIG, CHANGE_FIELD_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void model_series::updating()
{
    // Retrieves the associated Sight ModelSeries object
    const auto modelSeries = m_model.lock();

    this->stopping();

    // showRec indicates if we have to show the associated reconstructions or not
    const bool showRec = modelSeries->getField("ShowReconstructions", std::make_shared<data::boolean>(true))->value();

    for(const auto& reconstruction : modelSeries->getReconstructionDB())
    {
        auto adaptor = this->registerService<module::viz::scene3d::adaptor::reconstruction>(
            "sight::module::viz::scene3d::adaptor::reconstruction"
        );
        adaptor->set_input(reconstruction, "reconstruction", true);

        // We use the default service ID to get a unique number because a ModelSeries contains several Reconstructions
        adaptor->set_id(this->get_id() + "_" + adaptor->get_id());

        adaptor->setRenderService(this->getRenderService());
        adaptor->setLayerID(m_layerID);
        adaptor->setTransformId(this->getTransformId());
        adaptor->setMaterialTemplateName(m_materialTemplateName);
        adaptor->setAutoResetCamera(m_autoResetCamera);
        adaptor->setQueryFlags(m_queryFlags);

        adaptor->start();

        if(m_isVisibleTag)
        {
            adaptor->updateVisibility(!m_isVisible);
            SIGHT_WARN("The value of the modelSeries field will not be taken into account");
        }
        else
        {
            adaptor->updateVisibility(!showRec);
        }

        module::viz::scene3d::adaptor::mesh::sptr meshAdaptor = adaptor->getMeshAdaptor();
        meshAdaptor->setDynamic(m_isDynamic);
        meshAdaptor->setDynamicVertices(m_isDynamicVertices);
    }
}

//------------------------------------------------------------------------------

void model_series::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void model_series::setVisible(bool _visible)
{
    auto adaptors = this->getRegisteredServices();
    for(const auto& adaptor : adaptors)
    {
        auto recAdaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::reconstruction>(adaptor.lock());
        recAdaptor->updateVisibility(!_visible);
    }
}

//------------------------------------------------------------------------------

void model_series::showReconstructionsOnFieldChanged()
{
    const auto modelSeries = m_model.lock();

    const bool showRec = modelSeries->getField("ShowReconstructions", std::make_shared<data::boolean>(true))->value();

    auto adaptors = this->getRegisteredServices();
    for(const auto& adaptor : adaptors)
    {
        auto recAdaptor = std::dynamic_pointer_cast<module::viz::scene3d::adaptor::reconstruction>(adaptor.lock());
        recAdaptor->updateVisibility(!showRec);
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
