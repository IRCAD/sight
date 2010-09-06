/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkDepthSortPolyData.h>
#include <vtkPlaneCollection.h>

#include <fwData/Material.hpp>
#include <fwData/TriangularMesh.hpp>

#include <vtkIO/vtk.hpp>

#include "visuVTKAdaptor/MeshFactory.hpp"

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

MeshFactory::MeshFactory()
{
    m_clippingPlanes = 0;
    m_actor = vtkActor::New();
    m_normals = vtkPolyDataNormals::New();
#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
    m_depthSort = vtkDepthSortPolyData::New();
    m_hasAlpha = false;
#endif
#endif
    m_normalsFeatureAngle = 180;
}

//------------------------------------------------------------------------------

MeshFactory::~MeshFactory()
{
    m_clippingPlanes = 0;
#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
    m_depthSort->Delete();
    m_depthSort = 0;
#endif
#endif

    m_normals->Delete();
    m_normals = 0;

    m_actor->Delete();
    m_actor = 0;
}

//------------------------------------------------------------------------------

vtkActor* MeshFactory::getActor()
{
    return m_actor;
}

//------------------------------------------------------------------------------

void MeshFactory::updateTriangulaMesh( ::fwData::TriangularMesh::sptr mesh)
{
    vtkPolyData * polyData       = ::vtkIO::toVTKMesh(mesh);
    vtkPolyDataMapper  * mapper  = vtkPolyDataMapper::New();

    m_normals->SetInput(polyData);
    m_normals->ComputePointNormalsOn ();
    m_normals->ComputeCellNormalsOff ();
    m_normals->ConsistencyOn ();
    m_normals->SplittingOn ();
    m_normals->SetFeatureAngle(m_normalsFeatureAngle);


    mapper->SetInputConnection(m_normals->GetOutputPort());

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
    m_depthSort->SetInputConnection(m_normals->GetOutputPort());
    m_depthSort->SetDirectionToBackToFront();
    mapper->SetInputConnection(m_depthSort->GetOutputPort());
#endif
#endif

    m_actor->SetMapper(mapper);

    if (m_clippingPlanes)
    {
        mapper->RemoveAllClippingPlanes();
        mapper->SetClippingPlanes(m_clippingPlanes);
    }

    mapper->Delete();
    polyData->Delete();
}

//------------------------------------------------------------------------------

void MeshFactory::updateMaterial( ::fwData::Material::sptr material )
{
    if (m_actor)
    {
        ::fwData::Color::sptr color = material->ambient();

        vtkProperty *property = m_actor->GetProperty();

        property->SetColor( color->red(),
                color->green(),
                color->blue());

        //3DVSP like rendering
        property->SetSpecularColor(1.,1.,1.);
        property->SetSpecularPower(100.); //Shininess
        property->SetAmbient(.05);
        property->SetDiffuse(1.);
        property->SetSpecular(1.);
//        property->SetInterpolationToPhong();


        property->SetOpacity( color->alpha() );

        /// Sets RepresentationMode
        if(material->getRepresentationMode() == ::fwData::Material::MODE_SURFACE)
        {
            m_actor->GetProperty()->SetRepresentationToSurface();
            m_actor->GetProperty()->EdgeVisibilityOff();
        }
        else if(material->getRepresentationMode() == ::fwData::Material::MODE_EDGE)
        {
            m_actor->GetProperty()->SetRepresentationToSurface();
            m_actor->GetProperty()->EdgeVisibilityOn();
        }
        else if(material->getRepresentationMode() == ::fwData::Material::MODE_WIREFRAME)
        {
            m_actor->GetProperty()->SetRepresentationToWireframe();
        }
        else if(material->getRepresentationMode() == ::fwData::Material::MODE_POINT)
        {
            m_actor->GetProperty()->SetRepresentationToPoints();
        }

        /// Sets ShadingMode
        if(material->getShadingMode() == ::fwData::Material::MODE_PHONG)
        {
            m_actor->GetProperty()->SetInterpolationToPhong();
        }
        else if(material->getShadingMode() == ::fwData::Material::MODE_GOURAUD)
        {
            m_actor->GetProperty()->SetInterpolationToGouraud();
        }
        else if(material->getShadingMode() == ::fwData::Material::MODE_FLAT)
        {
            m_actor->GetProperty()->SetInterpolationToFlat();
        }

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
        if (color->alpha() < 1.)
        {
            m_actor->GetMapper()->SetInputConnection(m_depthSort->GetOutputPort());
            m_hasAlpha = true;
        }
        else
        {
            m_actor->GetMapper()->SetInputConnection(m_normals->GetOutputPort());
            m_hasAlpha = false;
        }
#endif
#endif

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

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
#ifdef USE_DEPTH_SORT
bool MeshFactory::hasAlpha()
{
    return m_hasAlpha;
};

#endif
#endif


void MeshFactory::setVtkClippingPlanes(vtkPlaneCollection *planes)
{
    m_clippingPlanes = planes;
}




} //namespace visuVTKAdaptor

