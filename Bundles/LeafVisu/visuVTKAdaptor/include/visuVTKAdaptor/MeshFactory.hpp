/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_VTKADAPTOR_MESHFACTORY_HPP_
#define _VISUVTKADAPTOR_VTKADAPTOR_MESHFACTORY_HPP_


#include <fwData/Material.hpp>
#include <fwData/TriangularMesh.hpp>

#include "visuVTKAdaptor/config.hpp"

class  vtkActor;
class  vtkDepthSortPolyData;
class  vtkPolyDataNormals;
class  vtkPlaneCollection;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API MeshFactory
{

public:

    VISUVTKADAPTOR_API MeshFactory();

    VISUVTKADAPTOR_API virtual ~MeshFactory();

protected:

    vtkActor* getActor();

    void updateTriangulaMesh( ::fwData::TriangularMesh::sptr mesh);

    void updateMaterial( ::fwData::Material::sptr material );

    void updateVisibility( bool isVisible);

    void setVtkClippingPlanes(vtkPlaneCollection *planes);

#ifndef USE_DEPTH_PEELING // replacement for depth peeling
    bool hasAlpha();

    vtkDepthSortPolyData *m_depthSort;
    bool m_hasAlpha;
#endif

    void setNormalsFeatureAngle(double angle){ m_normalsFeatureAngle = angle; }

protected:
    vtkPolyDataNormals * m_normals;
    vtkPlaneCollection * m_clippingPlanes;
    vtkActor * m_actor ;

    double m_normalsFeatureAngle;


};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_VTKADAPTOR_MESHFACTORY_HPP_

