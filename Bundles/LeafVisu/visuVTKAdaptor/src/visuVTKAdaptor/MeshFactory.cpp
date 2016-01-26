/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/MeshFactory.hpp"

#include <fwData/Material.hpp>

#include <fwVtkIO/vtk.hpp>

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkDepthSortPolyData.h>
#include <vtkPlaneCollection.h>

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

MeshFactory::MeshFactory() :
    m_normals(vtkPolyDataNormals::New()),
    m_clippingPlanes(nullptr),
    m_actor(vtkActor::New()),
    m_normalsFeatureAngle(180.)
{
}

//------------------------------------------------------------------------------

MeshFactory::~MeshFactory()
{
    m_clippingPlanes = nullptr;

    m_normals->Delete();
    m_normals = nullptr;

    m_actor->Delete();
    m_actor = nullptr;
}

//------------------------------------------------------------------------------

vtkActor* MeshFactory::getActor() const
{
    return m_actor;
}

//------------------------------------------------------------------------------

void MeshFactory::updateMaterial( ::fwData::Material::sptr material )
{
    if (m_actor)
    {
        vtkProperty *property = m_actor->GetProperty();

        ::fwData::Color::sptr diffuse = material->diffuse();
        property->SetDiffuseColor(diffuse->red(), diffuse->green(), diffuse->blue());

        // Use alpha from the diffuse color
        property->SetOpacity( diffuse->alpha() );

        ::fwData::Color::sptr ambient = material->ambient();
        property->SetAmbientColor(ambient->red(), ambient->green(), ambient->blue());

        property->SetSpecularColor(1.,1.,1.);
        property->SetSpecularPower(100.); //Shininess

        /// Sets RepresentationMode
        if(material->getRepresentationMode() == ::fwData::Material::SURFACE)
        {
            m_actor->GetProperty()->SetRepresentationToSurface();
            m_actor->GetProperty()->EdgeVisibilityOff();
        }
        else if(material->getRepresentationMode() == ::fwData::Material::EDGE)
        {
            m_actor->GetProperty()->SetRepresentationToSurface();
            m_actor->GetProperty()->EdgeVisibilityOn();
        }
        else if(material->getRepresentationMode() == ::fwData::Material::WIREFRAME)
        {
            m_actor->GetProperty()->SetRepresentationToWireframe();
        }
        else if(material->getRepresentationMode() == ::fwData::Material::POINT)
        {
            m_actor->GetProperty()->SetRepresentationToPoints();
        }

        /// Sets ShadingMode
        if(material->getShadingMode() == ::fwData::Material::PHONG)
        {
            m_actor->GetProperty()->SetInterpolationToPhong();
        }
        else if(material->getShadingMode() == ::fwData::Material::GOURAUD)
        {
            m_actor->GetProperty()->SetInterpolationToGouraud();
        }
        else if(material->getShadingMode() == ::fwData::Material::FLAT)
        {
            m_actor->GetProperty()->SetInterpolationToFlat();
        }
    }
}

//------------------------------------------------------------------------------

void MeshFactory::updateVisibility( bool isVisible)
{
    if (m_actor)
    {
        m_actor->SetVisibility( isVisible );
    }
}

//------------------------------------------------------------------------------

void MeshFactory::setVtkClippingPlanes(vtkPlaneCollection *planes)
{
    m_clippingPlanes = planes;
}


} //namespace visuVTKAdaptor

