/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGESLICEORIENTATIONTEXT_HPP__
#define __VISUVTKADAPTOR_IMAGESLICEORIENTATIONTEXT_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <memory> //unique_ptr
#include <string>

namespace visuVTKAdaptor
{

class ImageSliceOrientationTextPImpl;

/**
 * @brief Shows image orientation information (right, left, ...)
 * This adaptor show locations labels in the four borders of the scene
 */
class VISUVTKADAPTOR_CLASS_API ImageSliceOrientationText : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                           public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (ImageSliceOrientationText)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API ImageSliceOrientationText() noexcept;
    VISUVTKADAPTOR_API virtual ~ImageSliceOrientationText() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    /**
     * @brief Configure the adaptor
     *
     * Example :
       @code{.xml}
        <adaptor id="adaptorID" class="::visuVTKAdaptor::ImageSliceOrientationText" objectId="imageKey">
            <config renderer="default" >
                <locations>R,L,A,P,S,I</locations>
                <initialOrientation>${orientation}</initialOrientation>
            </config>
        </adaptor>
       @endcode
     *
     * - locations : comma-separated location terms to use, in this order :
     *   Right,Left,Anterior,Posterior,Superior,Inferior
     *   if 'default' is given, 'R,L,A,P,S,I' is used. If empty, nothing is displayed
     * - initialOrientation : initial orientation of the associated slice
     */
    VISUVTKADAPTOR_API void doConfigure();

    /// @brief IService API implementation
    /// @{
    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doSwap();
    /// @}

    /// Overides MedicalImageAdaptor's setOrientation
    VISUVTKADAPTOR_API void setOrientation( Orientation orientation );

    /// Locations string
    std::string m_locations;

    /// adaptor's initial orientation
    std::string m_initialOrientation;

    /// adaptor's private implementation
    std::unique_ptr< ImageSliceOrientationTextPImpl > m_pimpl;

private:
    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGESLICEORIENTATIONTEXT_HPP__
