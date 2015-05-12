/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGESLICE_HPP__
#define __VISUVTKADAPTOR_IMAGESLICE_HPP__

#include <fwCom/Connection.hpp>

#include <fwData/Image.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "visuVTKAdaptor/config.hpp"

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
 */
class VISUVTKADAPTOR_CLASS_API ImageSlice : public ::fwComEd::helper::MedicalImageAdaptor,
                                            public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageSlice)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API ImageSlice() throw();

    VISUVTKADAPTOR_API virtual ~ImageSlice() throw();

    void setCtrlImageId(std::string id)
    {
        m_ctrlImageId = id;
    }
    void setCtrlImage(::fwData::Image::sptr image)
    {
        m_ctrlImage = image;
    }
    void setVtkImageSourceId(std::string id)
    {
        m_imageSourceId = id;
    }
    void setVtkImageSource(vtkObject *obj)
    {
        m_imageSource = obj;
    }
    void setInterpolation(bool interpolation)
    {
        m_interpolation = interpolation;
    }

    void setActorOpacity(double actorOpacity)
    {
        m_actorOpacity = actorOpacity;
    }

    void setUseImageTF(bool use)
    {
        m_useImageTF = use;
    }

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    /**
     * @brief Configures the service
     *
     * @verbatim
       <adaptor id="imageSlice" class="::visuVTKAdaptor::ImageSlice" objectId="self">
           <config renderer="default" picker="negatodefault" sliceIndex="axial"
                   transform="trf" ctrlimage="imageKey" interpolation="off" vtkimagesource="imgSource"
                   actorOpacity="1.0" />
       </adaptor>
       @endverbatim
     * - \b renderer (mandatory): defines the renderer to show the arrow. It must be different from the 3D objects renderer.
     * - \b picker (mandatory): identifier of the picker
     * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
     * - \b transform (optional): the vtkTransform to associate to the adaptor
     * - \b ctrlimage (mandatory): image to show
     * - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
     * - \b vtkimagesource (optional): source image, used for blend
     * - \b actorOpacity (optional, default=1.0): actor opacity (float)
     */
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    virtual void buildPipeline();
    virtual void buildOutline();

    void setSlice( int slice, ::fwData::Image::sptr image  );

    ::fwData::Image::sptr getCtrlImage();

    void updateOutline();
    void updateImage( ::fwData::Image::sptr ImageSlice  );
    void updateSliceIndex( ::fwData::Image::sptr ImageSlice );


    std::string m_ctrlImageId;
    ::fwData::Image::wptr m_ctrlImage;

    bool m_interpolation;
    bool m_useImageTF;
    double m_actorOpacity;

    std::string m_imageSourceId;
    vtkObject  *m_imageSource;

    vtkImageActor *m_imageActor;

    vtkPolyData *m_planeOutlinePolyData;
    vtkPolyDataMapper *m_planeOutlineMapper;
    vtkActor *m_planeOutlineActor;

    ::fwCom::Connection m_connection;

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGESLICE_HPP__
