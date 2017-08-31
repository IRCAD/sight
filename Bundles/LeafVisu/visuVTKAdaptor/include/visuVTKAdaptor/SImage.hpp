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
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::Image" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tfSelection" uid="..." />
       <config renderer="default" picker="negatodefault" transform="trf" tfalpha="yes" vtkimageregister="imgSource"
               opacity="1.0" selectedTFKey="tkKey" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tfSelection [::fwData::Composite] (optional): composite containing the TransferFunction.
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
 *    - \b selectedTFKey (optional): key of the transfer function to use in negato
 */
class VISUVTKADAPTOR_CLASS_API SImage : public ::fwDataTools::helper::MedicalImageAdaptor,
                                        public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImage)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SImage() noexcept;

    VISUVTKADAPTOR_API virtual ~SImage() noexcept;

    static const ::fwServices::IService::KeyType s_IMAGE_INOUT;
    static const ::fwServices::IService::KeyType s_TF_SELECTION_INOUT;

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

    VISUVTKADAPTOR_API void configuring();
    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;

    virtual void buildPipeline();
    virtual void destroyPipeline();

    void updateImage( ::fwData::Image::sptr image  );

    /// Slot: Update image opacity and visibility
    void updateImageOpacity();
    void updateWindowing();
    void updateImageTransferFunction( ::fwData::Image::sptr image );

    /// Called when transfer function points are modified.
    VISUVTKADAPTOR_API virtual void updatingTFPoints();

    /// Called when transfer function windowing is modified.
    VISUVTKADAPTOR_API virtual void updatingTFWindowing(double window, double level);

private:

    std::string m_imageRegisterId;
    vtkObject* m_imageRegister;

    int m_imagePortId;
    double m_imageOpacity;
    bool m_allowAlphaInTF;

    vtkSmartPointer< fwVtkWindowLevelLookupTable > m_lut;
    vtkImageMapToColors* m_map2colors;
    vtkImageData* m_imageData;

};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SIMAGE_HPP__
