/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGE_HPP__
#define __VISUVTKADAPTOR_IMAGE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vtkSmartPointer.h>

class fwVtkWindowLevelLookupTable;
class vtkImageMapToColors;
class vtkImageData;

namespace visuVTKAdaptor
{

/**
 * @brief Render an image on the generic scene
 */
class VISUVTKADAPTOR_CLASS_API Image : public ::fwDataTools::helper::MedicalImageAdaptor,
                                       public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Image)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Image() noexcept;

    VISUVTKADAPTOR_API virtual ~Image() noexcept;

    void setVtkImageRegisterId(std::string id)
    {
        m_imageRegisterId = id;
    }
    void setVtkImageRegister(vtkObject* obj)
    {
        m_imageRegister = obj;
    }
    void setImageOpacity(double opacity)
    {
        m_imageOpacity = opacity;
    }
    void setAllowAlphaInTF(bool allow)
    {
        m_allowAlphaInTF = allow;
    }

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_IMAGE_OPACITY_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();


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

#endif // __VISUVTKADAPTOR_IMAGE_HPP__
