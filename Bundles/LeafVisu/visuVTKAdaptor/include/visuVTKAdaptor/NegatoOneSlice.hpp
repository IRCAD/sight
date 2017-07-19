/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_NEGATOONESLICE_HPP__
#define __VISUVTKADAPTOR_NEGATOONESLICE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>


class vtkObject;


namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API NegatoOneSlice : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoOneSlice)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API NegatoOneSlice() noexcept;

    VISUVTKADAPTOR_API virtual ~NegatoOneSlice() noexcept;

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
    void setVtkImageSource(vtkObject* obj)
    {
        m_imageSource = obj;
    }
    void setActorOpacity(double actorOpacity)
    {
        m_actorOpacity = actorOpacity;
    }

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_IMAGE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doUpdate();

    /**
     * @brief Configures the service
     *
     * @code{.xml}
       <adaptor id="negatoOneSlice" class="::visuVTKAdaptor::NegatoOneSlice" objectId="imageKey">
           <config renderer="default" picker="negatodefault" sliceIndex="axial"
                   transform="trf" tfalpha="yes" interpolation="off" vtkimagesource="imgSource" actorOpacity="1.0"
                   selectedTFKey="tkKey" tfSelectionFwID="selectionID" />
       </adaptor>
       @endcode
     * - \b renderer (mandatory): defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b picker (mandatory): identifier of the picker
     * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
     * - \b transform (optional): the vtkTransform to associate to the adaptor
     * - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
     * - \b vtkimagesource (optional): source image, used for blend
     * - \b actorOpacity (optional, default=1.0): actor opacity (float)
     * - \b tfSelectionFwID (optional): fwID of the composite containing transfer functions
     * - \b selectedTFKey (optional): key of the transfer function to use in negato
     */
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();

    bool m_manageImageSource;
    std::string m_imageSourceId;
    vtkObject* m_imageSource;

    bool m_allowAlphaInTF;
    bool m_interpolation;
    double m_actorOpacity;

    vtkObject* getImageSource();
    void cleanImageSource();
    ::fwRenderVTK::IVtkAdaptorService::sptr getImageSliceAdaptor();
    ::fwRenderVTK::IVtkAdaptorService::sptr getImageAdaptor();

    ::fwRenderVTK::IVtkAdaptorService::wptr m_imageAdaptor;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_imageSliceAdaptor;

private:
    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice type
    void updateSliceType(int from, int to);

    /// Slot: update image
    void updateImage();
    /**
     * @}
     */
};


} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_NEGATOONESLICE_HPP__
