/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_IMAGESLICE_HPP__
#define __VISUVTKADAPTOR_IMAGESLICE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Connection.hpp>
#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>


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
 * - \b checkCtrlImage() : Check if ctrl image changed and update scene
 * - \b updateSliceIndex() : update image slice index
 * - \b updateSliceType() : update image slice type
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <adaptor id="imageSlice" class="::visuVTKAdaptor::ImageSlice" objectId="self">
       <config renderer="default" picker="negatodefault" sliceIndex="axial"
               transform="trf" ctrlimage="imageKey" interpolation="off" vtkimagesource="imgSource"
               actorOpacity="1.0" />
   </adaptor>
   @endcode
 *
 * @subsection Configuration Configuration
 *
 * - \b renderer (mandatory): defines the renderer to show the arrow. It must be different from the 3D objects renderer.
 * - \b picker (mandatory): identifier of the picker
 * - \b sliceIndex (optional, axial/frontal/sagittal, default=axial): orientation of the negato
 * - \b transform (optional): the vtkTransform to associate to the adaptor
 * - \b ctrlimage (mandatory): image to show
 * - \b interpolation (optional, yes/no, default=yes): if true, the image pixels are interpolated
 * - \b vtkimagesource (optional): source image, used for blend
 * - \b actorOpacity (optional, default=1.0): actor opacity (float)
 *
 */

class VISUVTKADAPTOR_CLASS_API ImageSlice : public ::fwDataTools::helper::MedicalImageAdaptor,
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
    void setVtkImageSource(vtkObject* obj)
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

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Composite::s_ADDED_OBJECTS_SIG to this::s_CHECK_CTRL_IMAGE_SLOT
     * Connect Composite::s_CHANGED_OBJECTS_SIG to this::s_CHECK_CTRL_IMAGE_SLOT
     * Connect Composite::s_REMOVED_OBJECTS_SIG to this::s_CHECK_CTRL_IMAGE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doConfigure() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    virtual void buildPipeline();
    virtual void buildOutline();

    void setSlice( int slice, ::fwData::Image::sptr image  );

    ::fwData::Image::sptr getCtrlImage();

    void updateOutline();
    void updateImage( ::fwData::Image::sptr ImageSlice  );
    void updateImageSliceIndex( ::fwData::Image::sptr ImageSlice );


    std::string m_ctrlImageId;
    ::fwData::Image::wptr m_ctrlImage;

    bool m_interpolation;
    bool m_useImageTF;
    double m_actorOpacity;

    std::string m_imageSourceId;
    vtkObject* m_imageSource;

    vtkImageActor* m_imageActor;

    vtkPolyData* m_planeOutlinePolyData;
    vtkPolyDataMapper* m_planeOutlineMapper;
    vtkActor* m_planeOutlineActor;

    ::fwCom::helper::SigSlotConnection m_connections;

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: Check if ctrl image changed and update scene
    void checkCtrlImage();

    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);
    /**
     * @}
     */

};


} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_IMAGESLICE_HPP__
