/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
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

#include <fwData/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>

#include <QQmlEngine>

fwServicesRegisterMacro( ::fwRenderQt3D::IAdaptor, ::visuQt3DAdaptor::SMaterial, ::fwData::Material )

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
    qmlRegisterType< ::fwRenderQt3D::data::Material >("fwRenderQt3D", 1, 0, "Material");
    qRegisterMetaType< ::fwRenderQt3D::data::Material* >("::fwRenderQt3D::Material*");
}

//-----------------------------------------------------------------------------

SMaterial::~SMaterial() noexcept
{
}

//-----------------------------------------------------------------------------

void SMaterial::configuring()
{
    if(this->getConfigTree().count("config"))
    {
        // Gets scene configuration.
        const ConfigType config = this->getConfigTree().get_child("config");

        if(!config.empty())
        {
            const ConfigType attributes = config.get_child("<xmlattr>");

            // If materialName attribute is found updates m_materialName accordingly.
            if (attributes.count(s_MATERIAL_NAME_CONFIG))
            {
                m_materialName = attributes.get<std::string>(s_MATERIAL_NAME_CONFIG);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SMaterial::starting()
{
    this->initialize();

    // Creates a Qt3D mesh from sight data.
    m_material = new ::fwRenderQt3D::data::Material();

    this->updating();
}

//-----------------------------------------------------------------------------

void SMaterial::stopping()
{
}

//-----------------------------------------------------------------------------

void SMaterial::updating()
{
    // Reads the material from the input as sight data.
    ::fwData::Material::sptr material = this->getInOut< ::fwData::Material >(s_MATERIAL_INOUT);

    m_material->updatePolygonMode(material->getRepresentationMode());
    m_material->updateOptionsMode(material->getOptionsMode());
    m_material->updateShadingMode(material->getShadingMode());
    m_material->updateRGBAMode(material);
}

//-----------------------------------------------------------------------------

void SMaterial::setMaterial(::fwRenderQt3D::data::Material* _material)
{
    m_material = _material;
}

//------------------------------------------------------------------------------

void SMaterial::setMaterialName(std::string _materialName)
{
    m_materialName = _materialName;
}

//------------------------------------------------------------------------------

::fwRenderQt3D::data::Material* SMaterial::getMaterial()
{
    return m_material;
}

//------------------------------------------------------------------------------

std::string SMaterial::getMaterialName()
{
    return m_materialName;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMaterial::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( s_MATERIAL_INOUT, ::fwData::Material::s_MODIFIED_SIG, s_UPDATE_SLOT );
    return connections;
}

} // namespace visuQt3DAdaptor.
