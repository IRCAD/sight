/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SIMAGE_HPP__
#define __VISUVTKADAPTOR_SIMAGE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

#include <vtkSmartPointer.h>

class fwVtkWindowLevelLookupTable;
class vtkImageMapToColors;
class vtkImageData;

namespace visuVTKAdaptor
{

/**
 * @brief Render an image on the generic scene
 *
 * @section Slots Slots
 * - \b updateImageOpacity() : update the image opacity and visibility according to image fields
 * - \b updateTFPoints() : update the displayed transfer function according to the new points
 * - \b updateTFWindowing(double window, double level) : update the displayed transfer function according to the new
 *      window and level
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::Image">
       <inout key="image" uid="..." autoConnect="yes" />
       <inout key="tf" uid="..." optional="yes" />
       <config renderer="default" picker="negatodefault" transform="trf" tfalpha="yes" vtkimageregister="imgSource"
               opacity="1.0" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel). The transferFunction's signals are automatically connected to
 *      the slots 'updateTFPoints' and 'updateTFWindowing'.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the image
 *    - \b picker (mandatory): identifier of the picker
 *    - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 *    - \b transform (optional): the vtkTransform to associate to the adaptor
 *    - \b tfalpha (optional, yes/no, default=no): if true, the opacity of the transfer function is used in the negato.
 *    - \b vtkimageregister (optional): source image, used for blend
 *    - \b opacity (optional, default=1.0): actor opacity (float)
 */
class VISUVTKADAPTOR_CLASS_API SImage : public ::fwDataTools::helper::MedicalImageAdaptor,
                                        public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImage)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImage() noexcept;

    VISUVTKADAPTOR_API virtual ~SImage() noexcept;

    static const ::fwServices::IService::KeyType s_IMAGE_INOUT;
    static const ::fwServices::IService::KeyType s_TF_INOUT;

    //------------------------------------------------------------------------------

    void setVtkImageRegisterId(std::string id)
    {
        m_imageRegisterId = id;
    }
    //------------------------------------------------------------------------------

    void setVtkImageRegister(vtkObject* obj)
    {
        m_imageRegister = obj;
    }
    //------------------------------------------------------------------------------

    void setImageOpacity(double opacity)
    {
        m_imageOpacity = opacity;
    }
    //------------------------------------------------------------------------------

    void setAllowAlphaInTF(bool allow)
    {
        m_allowAlphaInTF = allow;
    }

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;
    VISUVTKADAPTOR_API void swapping(const KeyType& key) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    virtual void buildPipeline();
    virtual void destroyPipeline();

    void updateImage( ::fwData::Image::sptr image  );

    /// Slot: Update image opacity and visibility
    void updateImageOpacity();
    void updateImageTransferFunction();

    /// Slot: update the displayed lookup table
    VISUVTKADAPTOR_API virtual void updateTFPoints() override;

    /// Slot: update the windowing of the displayed lookup table
    VISUVTKADAPTOR_API virtual void updateTFWindowing(double window, double level) override;

private:

    std::string m_imageRegisterId;
    vtkObject* m_imageRegister;

    int m_imagePortId;
    double m_imageOpacity;
    bool m_allowAlphaInTF;

    vtkSmartPointer< fwVtkWindowLevelLookupTable > m_lut;
    vtkSmartPointer< vtkImageMapToColors > m_map2colors;
    vtkSmartPointer< vtkImageData > m_imageData;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SIMAGE_HPP__
