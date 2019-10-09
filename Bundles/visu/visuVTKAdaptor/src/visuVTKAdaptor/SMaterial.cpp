/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/SMaterial.hpp"

#include <fwData/Material.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkWindowLevelLookupTable.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SMaterial )

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SMaterial::s_MATERIAL_INPUT = "material";

// Name of the texture, don't use "diffuse" as name to avoid shadowing variables,
// "diffuse" name is already used as vec3 in VTK shaders.
static const std::string s_TEXTURENAME = " u_diffuseTexture";

//------------------------------------------------------------------------------

SMaterial::SMaterial() noexcept :
    m_property(vtkProperty::New()),
    m_manageProperty(true),
    m_lighting(true)
{
}

//------------------------------------------------------------------------------

SMaterial::~SMaterial() noexcept
{
    if (m_manageProperty)
    {
        m_property->Delete();
    }
    m_property = nullptr;
}

//------------------------------------------------------------------------------

void SMaterial::setVtkProperty(vtkProperty* property)
{
    if (m_manageProperty)
    {
        m_property->Delete();
        m_property = nullptr;
    }

    if (property)
    {
        m_manageProperty = false;
        m_property       = property;
    }
    else
    {
        m_manageProperty = true;
        m_property       = vtkProperty::New();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

vtkProperty* SMaterial::getVtkProperty() const
{
    return m_property;
}

//------------------------------------------------------------------------------

void SMaterial::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SMaterial::starting()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SMaterial::updating()
{
    ::fwData::Material::csptr material = this->getInput < ::fwData::Material >(s_MATERIAL_INPUT);
    SLM_ASSERT("Missing material", material);

    this->updateMaterial( material );
    this->requestRender();
}

//------------------------------------------------------------------------------

void SMaterial::stopping()
{

}

//------------------------------------------------------------------------------

void SMaterial::updateMaterial( CSPTR(::fwData::Material)material )
{
    //3DVSP-like rendering
    m_property->SetLighting(material->getShadingMode() > 0);

    ::fwData::Color::sptr diffuse = material->diffuse();
    m_property->SetDiffuseColor(static_cast<double>(diffuse->red()),
                                static_cast<double>(diffuse->green()),
                                static_cast<double>(diffuse->blue()));

    // Use alpha from the diffuse color
    m_property->SetOpacity(static_cast<double>(diffuse->alpha()) );

    ::fwData::Color::sptr ambient = material->ambient();
    m_property->SetAmbientColor(static_cast<double>(ambient->red()),
                                static_cast<double>(ambient->green()),
                                static_cast<double>(ambient->blue()));

    m_property->SetSpecularColor(1., 1., 1.);
    m_property->SetSpecularPower(100.); //Shininess

    // set texture
    ::fwData::Image::sptr diffTex = material->getDiffuseTexture();

    if(diffTex != nullptr)
    {
        ::fwData::mt::ObjectReadLock lock(diffTex);

        if (diffTex->getSizeInBytes() != 0)
        {
            vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
            ::fwVtkIO::toVTKImage( diffTex, vtkImage );

            vtkSmartPointer<vtkTexture> vtkTex;
            if( vtkImage->GetScalarType() != VTK_UNSIGNED_CHAR )
            {
                auto lut           = vtkSmartPointer< vtkWindowLevelLookupTable>::New();
                const double range = vtkImage->GetScalarRange()[1] - vtkImage->GetScalarRange()[0];
                const double level = (vtkImage->GetScalarRange()[1] + vtkImage->GetScalarRange()[0]) * 0.5;
                lut->SetWindow(range);
                lut->SetLevel(level);
                lut->Build();

                vtkTex = vtkSmartPointer< vtkTexture >::New();
                vtkTex->SetInputData(vtkImage);
                vtkTex->SetLookupTable(lut);
#ifdef VTK_MAJOR_VERSION > 7
                vtkTex->SetColorMode(VTK_COLOR_MODE_MAP_SCALARS);
#else
                vtkTex->SetMapColorScalarsThroughLookupTable(1);
#endif
            }
            else
            {
                vtkTex = vtkSmartPointer< vtkTexture >::New();
                vtkTex->SetInputData(vtkImage);
            }

            ::fwData::Material::FilteringType filtering = material->getDiffuseTextureFiltering();
            ::fwData::Material::WrappingType wrapping   = material->getDiffuseTextureWrapping();
            vtkTex->SetRepeat( wrapping == ::fwData::Material::REPEAT );
            vtkTex->SetEdgeClamp( wrapping == ::fwData::Material::CLAMP );
            vtkTex->SetInterpolate( filtering == ::fwData::Material::LINEAR );
            //Warning: don't use "diffuse" as texture name, "diffuse" name is already used as vec3 in VTK shaders.
            m_property->RemoveTexture(s_TEXTURENAME.c_str());
            m_property->SetTexture(s_TEXTURENAME.c_str(), vtkTex);
        }
    }

    // Test first if representation mode is known to avoid unhandled conditions.
    const bool knownRep = material->getRepresentationMode() == ::fwData::Material::SURFACE ||
                          material->getRepresentationMode() == ::fwData::Material::EDGE ||
                          material->getRepresentationMode() == ::fwData::Material::WIREFRAME ||
                          material->getRepresentationMode() == ::fwData::Material::POINT;

    OSLM_ASSERT("Unknown material representation mode : " << material->getRepresentationMode(), knownRep);

    switch(material->getRepresentationMode())
    {
        case ::fwData::Material::SURFACE:
            m_property->SetRepresentationToSurface();
            m_property->EdgeVisibilityOff();
            break;

        case ::fwData::Material::EDGE:
            m_property->SetRepresentationToSurface();
            m_property->EdgeVisibilityOn();
            break;

        case ::fwData::Material::WIREFRAME:
            m_property->SetRepresentationToWireframe();
            break;

        case ::fwData::Material::POINT:
            m_property->SetRepresentationToPoints();
            break;
    }

    // Test first if shading mode is known to avoid unhandled conditions.
    const bool knownShading = material->getShadingMode() == ::fwData::Material::AMBIENT ||
                              material->getShadingMode() == ::fwData::Material::PHONG ||
                              material->getShadingMode() == ::fwData::Material::GOURAUD ||
                              material->getShadingMode() == ::fwData::Material::FLAT;

    OSLM_ASSERT("Unknown shading mode : " << material->getShadingMode(), knownShading);

    switch(material->getShadingMode())
    {
        /// Sets ShadingMode
        case ::fwData::Material::AMBIENT:
            break;
        case ::fwData::Material::PHONG:
            m_property->SetInterpolationToPhong();
            break;

        case ::fwData::Material::GOURAUD:
            m_property->SetInterpolationToGouraud();
            break;

        case ::fwData::Material::FLAT:
            m_property->SetInterpolationToFlat();
            break;
    }

    m_property->Modified();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMaterial::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MATERIAL_INPUT, ::fwData::Material::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
