/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef __VISUVTKVRADAPTOR_VOLUME_HPP__
#define __VISUVTKVRADAPTOR_VOLUME_HPP__

#include "visuVTKVRAdaptor/config.hpp"

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwData/Image.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwRenderVTK/SRender.hpp>


class vtkAbstractVolumeMapper;
class vtkBoxWidget2;
class vtkColorTransferFunction;
class vtkCommand;
class vtkPiecewiseFunction;
class vtkPlaneCollection;
class vtkVolume;
class vtkVolumeProperty;

namespace visuVTKVRAdaptor
{

class VISUVTKVRADAPTOR_CLASS_API Volume : public ::fwComEd::helper::MedicalImageAdaptor,
                                          public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Volume)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKVRADAPTOR_API Volume() throw();

    VISUVTKVRADAPTOR_API virtual ~Volume() throw();

    VISUVTKVRADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::SRender::VtkObjectIdType id );

    VISUVTKVRADAPTOR_API void setVtkClippingPlanes( vtkPlaneCollection *planes );

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKVRADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

    /// Apply the cropping on volume rendering
    VISUVTKVRADAPTOR_API void crop();

    /// Update associated transform adaptor with CropBox transform
    VISUVTKVRADAPTOR_API void updateTransform();

    /// Update CropBox transform with transform adaptor
    VISUVTKVRADAPTOR_API void updateCropBoxTransform();

protected:

    VISUVTKVRADAPTOR_API void doStart() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /**
     * @brief Configures the service
     *
     * Configuration example :
       @code{.xml}
       <adaptor id="VolumeScene3D" class="::visuVTKVRAdaptor::Volume" objectId="imageKey">
         <config renderer="default" clippingplanes="clippingPlanesId" autoresetcamera="yes|no" croppingBox="yes|no" reductionFactor="0.5"/>
       </adaptor>
       @endcode
     *
     * \b renderer : ID of renderer the adaptor must use
     * \b clippingplanes : id of VTK object for clipping planes
     * \b autoresetcamera : (not mandatory, default is yes) set if the renderer must reset camera when updating image
     * \b croppingBox : (not mandatory, default is no) set if the cropping box must be shown
     * \b reductionFactor [0-1]: factor to resample the original image.
     *
     * @throw fwTools::Failed
     */
    VISUVTKVRADAPTOR_API void doConfigure() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doSwap() throw(fwTools::Failed);


    /// Called when transfer function points are modified.
    VISUVTKVRADAPTOR_API virtual void updatingTFPoints();

    /// Called when transfer function windowing is modified.
    VISUVTKVRADAPTOR_API virtual void updatingTFWindowing(double window, double level);

    /// Slot: reset the clipping box widget around the volume
    void resetBoxWidget();

    /// Slot: show/hide clipping box
    void activateBoxClipping( bool activate );

    void buildPipeline();

    void updateImage( ::fwData::Image::sptr image  );

    void updateVolumeTransferFunction( ::fwData::Image::sptr image );

    vtkPlaneCollection *m_clippingPlanes;

    ::fwRenderVTK::SRender::VtkObjectIdType m_clippingPlanesId;

    vtkAbstractVolumeMapper  * m_volumeMapper;
    vtkVolumeProperty        * m_volumeProperty;
    vtkVolume                * m_volume;

    vtkPiecewiseFunction     * m_opacityTransferFunction;
    vtkColorTransferFunction * m_colorTransferFunction;

    vtkCommand               * m_abortCommand;

    vtkBoxWidget2 * m_boxWidget;
    vtkCommand    * m_croppingCommand;
    vtkCommand    * m_transformCommand;

    /// Croping box default state
    bool m_croppingBoxDefaultState;

private:

    bool m_autoResetCamera;

    double m_reductionFactor;
};




} //namespace visuVTKVRAdaptor

#endif // __VISUVTKVRADAPTOR_VOLUME_HPP__
