/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGESLICEORIENTATIONTEXT_HPP__
#define __VISUVTKADAPTOR_SIMAGESLICEORIENTATIONTEXT_HPP__

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
 *
 * @section Slots Slots
 * - \b updateSliceType(int from, int to): update image slice type
 *
 * @section XML XML Configuration
 *
 *  @code{.xml}
        <service type="::visuVTKAdaptor::SImageSliceOrientationText" autoConnect="yes">
            <in key="image" uid="..." />
            <config renderer="default" >
                <locations>R,L,A,P,S,I</locations>
                <initialOrientation>${orientation}</initialOrientation>
            </config>
        </adaptor>
     @endcode
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image
 *    - b locations : comma-separated location terms to use, in this order :
 *        Right,Left,Anterior,Posterior,Superior,Inferior
 *        if 'default' is given, 'R,L,A,P,S,I' is used. If empty, nothing is displayed
 *    - b initialOrientation : initial orientation of the associated slice
 */
class VISUVTKADAPTOR_CLASS_API SImageSliceOrientationText : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                            public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageSliceOrientationText)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImageSliceOrientationText() noexcept;
    VISUVTKADAPTOR_API virtual ~SImageSliceOrientationText() noexcept;

protected:

    /// @brief IService API implementation
    /// @{
    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();
    /// @}

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

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

#endif // __VISUVTKADAPTOR_SIMAGESLICEORIENTATIONTEXT_HPP__
