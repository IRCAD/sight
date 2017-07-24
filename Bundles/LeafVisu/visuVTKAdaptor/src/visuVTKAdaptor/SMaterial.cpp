/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SMaterial.hpp"

#include <fwData/Material.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkImageData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SMaterial );

namespace visuVTKAdaptor
{

const ::fwServices::IService::KeyType SMaterial::s_MATERIAL_INPUT = "material";

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
    m_property = NULL;
}

//------------------------------------------------------------------------------

void SMaterial::setVtkProperty(vtkProperty* property)
{
    if (m_manageProperty)
    {
        m_property->Delete();
        m_property = NULL;
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
    m_property->SetDiffuseColor(diffuse->red(), diffuse->green(), diffuse->blue());

    // Use alpha from the diffuse color
    m_property->SetOpacity( diffuse->alpha() );

    ::fwData::Color::sptr ambient = material->ambient();
    m_property->SetAmbientColor(ambient->red(), ambient->green(), ambient->blue());

    m_property->SetSpecularColor(1., 1., 1.);
    m_property->SetSpecularPower(100.); //Shininess

    // set texture
    ::fwData::Image::sptr diffTex = material->getDiffuseTexture();

    if(diffTex != NULL)
    {
        ::fwData::mt::ObjectReadLock lock(diffTex);

        if (diffTex->getSizeInBytes() != 0)
        {
            vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
            ::fwVtkIO::toVTKImage( diffTex, vtkImage );

            vtkSmartPointer<vtkTexture> vtkTex = vtkSmartPointer< vtkTexture >::New();
            vtkTex->SetInputData(vtkImage);

            ::fwData::Material::WrappingType wrapping = material->getDiffuseTextureWrapping();
            vtkTex->SetRepeat( wrapping == ::fwData::Material::REPEAT );
            vtkTex->SetEdgeClamp( wrapping == ::fwData::Material::CLAMP );
            m_property->RemoveTexture("diffuse");
            m_property->SetTexture("diffuse", vtkTex);
        }
    }

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

        default:
            OSLM_ASSERT("Unknown material representation mode : " << material->getRepresentationMode(), false );
    }

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

        default:
            OSLM_ASSERT("Unknown shading mode : " << material->getShadingMode(), false );
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
