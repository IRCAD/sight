/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_NEGATOMPR_HPP__
#define __VISUVTKADAPTOR_NEGATOMPR_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

namespace visuVTKAdaptor
{

class SliceCursor;

class VISUVTKADAPTOR_CLASS_API NegatoMPR : public ::fwDataTools::helper::MedicalImageAdaptor,
                                           public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoMPR)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API NegatoMPR() throw();

    VISUVTKADAPTOR_API virtual ~NegatoMPR() throw();

    typedef enum
    {
        NO_SLICE = 0,
        ONE_SLICE,
        THREE_SLICES
    } SliceMode;

    void setAllowAlphaInTF(bool allow)
    {
        m_allowAlphaInTF = allow;
    }
    void setInterpolation(bool interpolation)
    {
        m_interpolation = interpolation;
    }
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

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /**
     * @brief Configures the service
     *
     * @code{.xml}
       <adaptor id="negato" class="::visuVTKAdaptor::NegatoMPR" objectId="imageKey">
           <config renderer="default" picker="negatodefault" mode="2d" slices="1" sliceIndex="axial"
                   transform="trf" tfalpha="yes" interpolation="off" vtkimagesource="imgSource" actorOpacity="1.0"
                   selectedTFKey="tkKey" tfSelectionFwID="selectionID" />
       </adaptor>
       @endcode
     * - \b renderer (mandatory): defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b picker (mandatory): identifier of the picker
     * - \b mode (optional, 2d or 3d): defines the scene mode. In 2d mode, the camera follow the negato in
     * axial/frontal/sagital orientation. In 3d mode, the camera is automatically reset when the image is modified. If
     * mode is not defined, the camera is free.
     * - \b slices (optional, default=3): number of slices shown in the adaptor
     * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
     * - \b transform (optional): the vtkTransform to associate to the adaptor
     * - \b tfalpha (optional, yes/no, default=no): if true, the opacity of the transfer function is used in the negato.
     * - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
     * - \b vtkimagesource (optional): source image, used for blend
     * - \b actorOpacity (optional, default=1.0): actor opacity (float)
     * - \b tfSelectionFwID (optional): fwID of the composite containing transfer functions
     * - \b selectedTFKey (optional): key of the transfer function to use in negato
     */
    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    ::fwRenderVTK::IVtkAdaptorService::sptr addAdaptor(std::string adaptor, int axis = -1);


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

    ::fwRenderVTK::IVtkAdaptorService::wptr m_sliceCursor;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_NEGATOMPR_HPP__
