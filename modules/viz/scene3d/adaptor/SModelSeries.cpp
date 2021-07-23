/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SModelSeries.hpp"

#include "modules/viz/scene3d/adaptor/SMesh.hpp"
#include "modules/viz/scene3d/adaptor/SReconstruction.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/Boolean.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/Reconstruction.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

static const core::com::Slots::SlotKeyType s_CHANGE_FIELD_SLOT = "changeField";

const std::string SModelSeries::s_MODEL_INPUT = "model";

static const std::string s_AUTORESET_CAMERA_CONFIG = "autoresetcamera";
static const std::string s_MATERIAL_CONFIG         = "material";
static const std::string s_DYNAMIC_CONFIG          = "dynamic";
static const std::string s_DYNAMIC_VERTICES_CONFIG = "dynamicVertices";
static const std::string s_QUERY_CONFIG            = "queryFlags";

//------------------------------------------------------------------------------

SModelSeries::SModelSeries() noexcept
{
    newSlot(s_CHANGE_FIELD_SLOT, &SModelSeries::showReconstructionsOnFieldChanged, this);
}

//------------------------------------------------------------------------------

SModelSeries::~SModelSeries() noexcept
{
}

//------------------------------------------------------------------------------

void SModelSeries::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    this->setTransformId(
        config.get<std::string>(
            sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG,
            this->getID() + "_transform"
        )
    );

    m_autoResetCamera = config.get<bool>(s_AUTORESET_CAMERA_CONFIG, true);

    m_materialTemplateName = config.get<std::string>(s_MATERIAL_CONFIG, m_materialTemplateName);
    m_isDynamic            = config.get<bool>(s_DYNAMIC_CONFIG, m_isDynamic);
    m_isDynamicVertices    = config.get<bool>(s_DYNAMIC_VERTICES_CONFIG, m_isDynamicVertices);

    if(config.count(s_QUERY_CONFIG))
    {
        const std::string hexaMask = config.get<std::string>(s_QUERY_CONFIG);
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

void SModelSeries::starting()
{
    this->initialize();

    this->updating();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SModelSeries::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_MODEL_INPUT, data::ModelSeries::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_INPUT, data::ModelSeries::s_RECONSTRUCTIONS_ADDED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_INPUT, data::ModelSeries::s_RECONSTRUCTIONS_REMOVED_SIG, s_UPDATE_SLOT);
    connections.push(s_MODEL_INPUT, data::ModelSeries::s_ADDED_FIELDS_SIG, s_CHANGE_FIELD_SLOT);
    connections.push(s_MODEL_INPUT, data::ModelSeries::s_REMOVED_FIELDS_SIG, s_CHANGE_FIELD_SLOT);
    connections.push(s_MODEL_INPUT, data::ModelSeries::s_CHANGED_FIELDS_SIG, s_CHANGE_FIELD_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SModelSeries::updating()
{
    // Retrieves the associated Sight ModelSeries object
    const auto modelSeries = m_model.lock();

    this->stopping();

    // showRec indicates if we have to show the associated reconstructions or not
    const bool showRec = modelSeries->getField("ShowReconstructions", data::Boolean::New(true))->value();

    for(auto reconstruction : modelSeries->getReconstructionDB())
    {
        auto adaptor = this->registerService<module::viz::scene3d::adaptor::SReconstruction>(
            "::sight::module::viz::scene3d::adaptor::SReconstruction"
        );
        adaptor->registerInput(reconstruction, "reconstruction", true);

        // We use the default service ID to get a unique number because a ModelSeries contains several Reconstructions
        adaptor->setID(this->getID() + "_" + adaptor->getID());

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

        module::viz::scene3d::adaptor::SMesh::sptr meshAdaptor = adaptor->getMeshAdaptor();
        meshAdaptor->setDynamic(m_isDynamic);
        meshAdaptor->setDynamicVertices(m_isDynamicVertices);
    }
}

//------------------------------------------------------------------------------

void SModelSeries::stopping()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SModelSeries::setVisible(bool _visible)
{
    auto adaptors = this->getRegisteredServices();
    for(auto adaptor : adaptors)
    {
        auto recAdaptor = module::viz::scene3d::adaptor::SReconstruction::dynamicCast(adaptor.lock());
        recAdaptor->updateVisibility(!_visible);
    }
}

//------------------------------------------------------------------------------

void SModelSeries::showReconstructionsOnFieldChanged()
{
    const auto modelSeries = m_model.lock();

    const bool showRec = modelSeries->getField("ShowReconstructions", data::Boolean::New(true))->value();

    auto adaptors = this->getRegisteredServices();
    for(auto adaptor : adaptors)
    {
        auto recAdaptor = module::viz::scene3d::adaptor::SReconstruction::dynamicCast(adaptor.lock());
        recAdaptor->updateVisibility(!showRec);
    }
}

} // namespace sight::module::viz::scene3d::adaptor.
