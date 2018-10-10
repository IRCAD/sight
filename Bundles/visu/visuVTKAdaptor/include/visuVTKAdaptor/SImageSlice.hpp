/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkImageActor;
class vtkLookupTable;
class vtkImageMapToColors;
class vtkImageData;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display only one slice of an image
 *
 *
 * @section Slots Slots
 * - \b updateSliceIndex() : update image slice index
 * - \b updateSliceType() : update image slice type
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SImageSlice" autoConnect="yes">
       <inout key="image" uid="..." />
       <config renderer="default" picker="negatodefault" sliceIndex="axial"
               transform="trf" ctrlimage="imageKey" interpolation="off" vtkimagesource="imgSource"
               actorOpacity="1.0" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 *
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image
 *    - \b picker (mandatory): identifier of the picker
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *    - \b ctrlimage (mandatory): image to show
 *    - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
 *    - \b vtkimagesource (optional): source image, used for blend
 *    - \b actorOpacity (optional, default=1.0): actor opacity (float)
 */

class VISUVTKADAPTOR_CLASS_API SImageSlice : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageSlice)(::fwRenderVTK::IAdaptor) )

    VISUVTKADAPTOR_API SImageSlice() noexcept;

    VISUVTKADAPTOR_API virtual ~SImageSlice() noexcept;

    //------------------------------------------------------------------------------

    void setVtkImageSourceId(std::string id)
    {
        m_imageSourceId = id;
    }
    //------------------------------------------------------------------------------

    void setVtkImageSource(vtkObject* obj)
    {
        m_imageSource = obj;
    }
    //------------------------------------------------------------------------------

    void setInterpolation(bool interpolation)
    {
        m_interpolation = interpolation;
    }

    //------------------------------------------------------------------------------

    void setActorOpacity(double actorOpacity)
    {
        m_actorOpacity = actorOpacity;
    }

    //------------------------------------------------------------------------------

    void setOrientation(::fwDataTools::helper::MedicalImage::Orientation _orientation)
    {
        m_helper.setOrientation(_orientation);
    }

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ::fwData::Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect ::fwData::Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    virtual void buildPipeline();
    virtual void buildOutline();

    void setSlice( int slice, ::fwData::Image::sptr image  );

    ::fwData::Image::sptr getCtrlImage();

    void updateOutline();
    void updateImage( ::fwData::Image::sptr SImageSlice  );
    void updateSImageSliceIndex( ::fwData::Image::sptr SImageSlice );

    bool m_interpolation;
    double m_actorOpacity;

    std::string m_imageSourceId;
    vtkObject* m_imageSource;

    vtkImageActor* m_imageActor;

    vtkPolyData* m_planeOutlinePolyData;
    vtkPolyDataMapper* m_planeOutlineMapper;
    vtkActor* m_planeOutlineActor;

private:

    /**
     * @name Slots
     * @{
     */

    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */

    ::fwDataTools::helper::MedicalImage m_helper;

};

} //namespace visuVTKAdaptor
