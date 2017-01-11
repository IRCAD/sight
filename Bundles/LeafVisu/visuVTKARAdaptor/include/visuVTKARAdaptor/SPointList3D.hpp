/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SPOINTLIST3D_HPP__
#define __VISUVTKARADAPTOR_SPOINTLIST3D_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Color.hpp>
#include <fwData/PointList.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>

namespace fwData
{
class Point;
}

namespace visuVTKARAdaptor
{

/**
 * @brief   Display a 3D point list.
 *
 *  This adaptor works on a ::fwData::PointList.
 */
class VISUVTKARADAPTOR_CLASS_API SPointList3D : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    typedef std::vector< SPTR(::fwData::Point) > PointListType;

    fwCoreServiceClassDefinitionsMacro ( (SPointList3D)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKARADAPTOR_API SPointList3D() throw();

    VISUVTKARADAPTOR_API virtual ~SPointList3D() throw();

protected:

    /// Copy point list and create adaptors
    VISUVTKARADAPTOR_API void doStart() throw(fwTools::Failed);

    /// Unregister adaptors and clears local point list
    VISUVTKARADAPTOR_API void doStop() throw(fwTools::Failed);

    /**
     * @code{.xml}
       <adaptor id="points" class="::visuVTKRDAdaptor::SPointList3D" objectId="pointListKey">
        <config renderer="default" color="#cb1f72" radius="3.0" />
       </adaptor>
       @endcode
     * - \b renderer : defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b color(optional) : color used to display the points.
     * - \b radius (optional) : point sphere radius.
     * - \b transform (optional) : transform used to display the points.
     */
    VISUVTKARADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Restart the service (stop-start)
    VISUVTKARADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Create adaptors
    VISUVTKARADAPTOR_API void doUpdate() throw(fwTools::Failed);

private:

    vtkSmartPointer<vtkPoints> m_points; ///< vtk points

    double m_radius; ///< Sphere radius

    /// color of the points
    ::fwData::Color::sptr m_ptColor;

};




} //namespace visuVTKRDAdaptor

#endif // __VISUVTKARADAPTOR_SPOINTLIST3D_HPP__
