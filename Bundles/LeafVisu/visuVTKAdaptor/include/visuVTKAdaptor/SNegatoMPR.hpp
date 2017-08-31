/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SNEGATOMPR_HPP__
#define __VISUVTKADAPTOR_SNEGATOMPR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <boost/logic/tribool.hpp>

namespace visuVTKAdaptor
{

class SliceCursor;

/**
 * @brief Display a negato image
 *
 * @section Slots Slots
 * - \b updateSliceType(int from, int to): update image slice type
 * - \b updateSliceMode(int mode): update Slice mode (0: NO_SLICE, 1: ONE_SLICE, 3: THREE_SLICES)
 * - \b showSlice(bool isShown): show/hide slice
 * - \b setCrossScale(double scale): set the slice cross scale. Forward the information to SliceCursor sub-adaptor.
 * - \b changeImageSource(std::string _value, std::string _key): set the VTK source image. The key must be "ImageSource"
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::visuVTKAdaptor::SNegatoMPR" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." optional="yes" />
       <config renderer="default" picker="negatodefault" mode="2d" slices="1" sliceIndex="axial"
               transform="trf" tfalpha="yes" interpolation="off" vtkimagesource="imgSource" actorOpacity="1.0" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image.
 *    - \b picker (mandatory): identifier of the picker
 *    - \b mode (optional, 2d or 3d): defines the scene mode. In 2d mode, the camera follow the negato in
 *    axial/frontal/sagital orientation. In 3d mode, the camera is automatically reset when the image is modified. If
 *    mode is not defined, the camera is free.
 *    - \b slices (optional, default=3): number of slices shown in the adaptor
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *    - \b tfalpha (optional, yes/no, default=no): if true, the opacity of the transfer function is used in the negato.
 *    - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
 *    - \b vtkimagesource (optional): source image, used for blend
 *    - \b actorOpacity (optional, default=1.0): actor opacity (float)
 */
class VISUVTKADAPTOR_CLASS_API SNegatoMPR : public ::fwDataTools::helper::MedicalImageAdaptor,
                                            public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SNegatoMPR)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SNegatoMPR() noexcept;

    VISUVTKADAPTOR_API virtual ~SNegatoMPR() noexcept;

    static const ::fwServices::IService::KeyType s_IMAGE_INOUT;
    static const ::fwServices::IService::KeyType s_TF_INOUT;

    typedef enum
    {
        NO_SLICE = 0,
        ONE_SLICE,
        THREE_SLICES
    } SliceMode;

    //------------------------------------------------------------------------------

    void setAllowAlphaInTF(bool allow)
    {
        m_allowAlphaInTF = allow;
    }
    //------------------------------------------------------------------------------

    void setInterpolation(bool interpolation)
    {
        m_interpolation = interpolation;
    }
    //------------------------------------------------------------------------------

    void setVtkImageSourceId(std::string id)
    {
        m_imageSourceId = id;
    }

    VISUVTKADAPTOR_API void setSliceMode(SliceMode sliceMode);
    VISUVTKADAPTOR_API SliceMode getSliceMode() const;
    VISUVTKADAPTOR_API ::boost::logic::tribool is3dModeEnabled() const;
    VISUVTKADAPTOR_API void set3dMode( bool enabled );

    /// Set actor opacity
    void setActorOpacity(double actorOpacity)
    {
        m_actorOpacity = actorOpacity;
    }

protected:

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();
    VISUVTKADAPTOR_API void swapping(const KeyType& key);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

    ::fwRenderVTK::IAdaptor::sptr addAdaptor(const std::string& adaptor, int axis = -1);

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice type
    void updateSliceType(int from, int to);

    /// Slot: update Slice mode (0: NO_SLICE, 1: ONE_SLICE, 3: THREE_SLICES)
    void updateSliceMode(int mode);

    /// Slot: show/hide slice
    void showSlice(bool isShown);

    /// Slot: set the slice cross scale. Forward the information to SliceCursor sub-adaptor.
    void setCrossScale(double scale);

    /// Slot: set the VTK source image. The key must be "ImageSource"
    void changeImageSource(std::string _value, std::string _key);

    /**
     * @}
     */

    bool m_allowAlphaInTF;
    bool m_interpolation;
    double m_actorOpacity;

    std::string m_imageSourceId;

    std::string m_slicingStartingProxy; ///< channel of the proxy used to start slicing
    std::string m_slicingStoppingProxy; ///< channel of the proxy used to stop slicing

    ::boost::logic::tribool m_3dModeEnabled;
    SliceMode m_sliceMode;
    SliceMode m_backupedSliceMode;
    ::fwCom::helper::SigSlotConnection m_connections; /// store subservices connections

    ::fwRenderVTK::IAdaptor::wptr m_sliceCursor;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SNEGATOMPR_HPP__
