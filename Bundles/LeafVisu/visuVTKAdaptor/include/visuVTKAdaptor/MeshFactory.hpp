/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_VTKADAPTOR_MESHFACTORY_HPP_
#define _VISUVTKADAPTOR_VTKADAPTOR_MESHFACTORY_HPP_

#include <fwData/Material.hpp>

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

    VISUVTKADAPTOR_API vtkActor* getActor();

    VISUVTKADAPTOR_API void updateMaterial( ::fwData::Material::sptr material );

    VISUVTKADAPTOR_API void updateVisibility( bool isVisible);

    VISUVTKADAPTOR_API void setVtkClippingPlanes(vtkPlaneCollection *planes);

    void setNormalsFeatureAngle(double angle){ m_normalsFeatureAngle = angle; }

protected:
    vtkPolyDataNormals * m_normals;
    vtkPlaneCollection * m_clippingPlanes;
    vtkActor * m_actor ;

    double m_normalsFeatureAngle;
};

} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_VTKADAPTOR_MESHFACTORY_HPP_

