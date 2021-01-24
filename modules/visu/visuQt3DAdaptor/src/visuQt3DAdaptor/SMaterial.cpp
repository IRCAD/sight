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

#include "visuQt3DAdaptor/SMaterial.hpp"

#include <data/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <QQmlEngine>

fwServicesRegisterMacro( ::fwRenderQt3D::IAdaptor, ::visuQt3DAdaptor::SMaterial, data::Material )

//-----------------------------------------------------------------------------

namespace visuQt3DAdaptor
{

//-----------------------------------------------------------------------------

static const std::string s_MATERIAL_INOUT = "material";

static const std::string s_MATERIAL_NAME_CONFIG = "materialName";

//-----------------------------------------------------------------------------

SMaterial::SMaterial() noexcept
{
    // Allows using Material as QML type when using SMaterial service in QML applications.
    qmlRegisterType< ::fwRenderQt3Ddata::Material >("fwRenderQt3D", 1, 0, "Material");
    qRegisterMetaType< ::fwRenderQt3Ddata::Material* >("::fwRenderQt3D::Material*");
}

//-----------------------------------------------------------------------------

SMaterial::~SMaterial() noexcept
{
}

//-----------------------------------------------------------------------------

void SMaterial::configuring()
{
    const ConfigType configTree = this->getConfigTree();
    const auto config           = configTree.get_child_optional("config.<xmlattr>");

    if(config)
    {
        m_materialName = config->get<std::string>(s_MATERIAL_NAME_CONFIG, m_materialName);
    }
}

//-----------------------------------------------------------------------------

void SMaterial::starting()
{
    this->initialize();

    // Creates a Qt3D mesh from sight data.
    m_material = new ::fwRenderQt3Ddata::Material();

    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMaterial::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_MATERIAL_INOUT, data::Material::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SMaterial::updating()
{
    // Reads the material from the input as sight data.
    data::Material::sptr material = this->getInOut< data::Material >(s_MATERIAL_INOUT);

    m_material->updatePolygonMode(material->getRepresentationMode());
    m_material->updateOptionsMode(material->getOptionsMode());
    m_material->updateShadingMode(material->getShadingMode());
    m_material->updateRGBAMode(material);
}

//-----------------------------------------------------------------------------

void SMaterial::stopping()
{
}

//-----------------------------------------------------------------------------

void SMaterial::setMaterial(::fwRenderQt3Ddata::Material* _material)
{
    m_material = _material;
}

//------------------------------------------------------------------------------

::fwRenderQt3Ddata::Material* SMaterial::getMaterial()
{
    return m_material;
}

//------------------------------------------------------------------------------

std::string SMaterial::getMaterialName()
{
    return m_materialName;
}

} // namespace visuQt3DAdaptor.
