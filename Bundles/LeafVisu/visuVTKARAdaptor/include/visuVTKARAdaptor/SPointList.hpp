/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SPOINTLIST_HPP__
#define __VISUVTKARADAPTOR_SPOINTLIST_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwCore/base.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

namespace fwData
{
class Point;
}

namespace visuVTKARAdaptor
{

/**
 * @brief   Display a 2D point list.
 *
 *  This adaptor works on a ::fwData::PointList.
 *
 *  The coordinate system of the point list is [ (0,0);(width,-height) ] whereas the coordinate system of the scene
 *  is [ (-width/2;-height/2,width/2:height/2], so we need to transform the points.
 */
class VISUVTKARADAPTOR_CLASS_API SPointList : public ::fwRenderVTK::IVtkAdaptorService
{

public:
    typedef std::vector< SPTR(::fwData::Point) > PointListType;

    fwCoreServiceClassDefinitionsMacro ( (SPointList)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKARADAPTOR_API SPointList() throw();

    VISUVTKARADAPTOR_API virtual ~SPointList() throw();

protected:

    /// Copy point list and create adaptors
    VISUVTKARADAPTOR_API void doStart() throw(fwTools::Failed);

    /// Unregister adaptors and clears local point list
    VISUVTKARADAPTOR_API void doStop() throw(fwTools::Failed);

    /**
     * @code{.xml}
       <adaptor id="points" class="::visuVTKARAdaptor::SPointList" objectId="pointListKey">
           <config renderer="default" imageId="${imageId1}"cameraUID="cameraUid" color="#cb1f72" radius="3.0" />
       </adaptor>
       @endcode
     * - \b renderer : defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b imageId(optional if camera is set) : Id of the image used to extract the resolution of the source image.
     * - \b camera(optional if image is set) : camera for taking account the optical center.
     * - \b color(optional) : color used to display the points.
     * - \b radius (optional) : size of the point beeing displayed.
     */
    VISUVTKARADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Restart the service (stop-start)
    VISUVTKARADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Create adaptors
    VISUVTKARADAPTOR_API void doUpdate() throw(fwTools::Failed);

private:

    /// Copy of the points so that we can modify them freely
    PointListType m_pointList;

    /// uid of the image where the points come from - used to get the resolution
    std::string m_imageId;

    /// uid of the camera
    std::string m_cameraUID;

    ///handle the color
    std::string m_hexaColor;

    ///handle the size
    std::string m_radius;

};




} //namespace visuVTKARAdaptor

#endif // __VISUVTKARADAPTOR_SPOINTLIST_HPP__
