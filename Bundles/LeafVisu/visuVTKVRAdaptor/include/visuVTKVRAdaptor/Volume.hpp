/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _VISUVTKVRADAPTOR_VOLUME_HPP_
#define _VISUVTKVRADAPTOR_VOLUME_HPP_

#include <fwData/Image.hpp>

#include <fwRenderVTK/VtkRenderService.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "visuVTKVRAdaptor/config.hpp"

class vtkLookupTable;
class vtkImageMapToColors;
class vtkImageData;
class vtkAbstractVolumeMapper;
class vtkVolumeProperty;
class vtkVolume;
class vtkCommand;

class vtkBoxWidget2;

namespace visuVTKVRAdaptor
{

class VISUVTKVRADAPTOR_CLASS_API Volume : public ::fwComEd::helper::MedicalImageAdaptor, public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Volume)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKVRADAPTOR_API Volume() throw();

    VISUVTKVRADAPTOR_API virtual ~Volume() throw();

    VISUVTKVRADAPTOR_API void setClippingPlanesId( ::fwRenderVTK::VtkRenderService::VtkObjectIdType id );

    VISUVTKVRADAPTOR_API void setVtkClippingPlanes( vtkPlaneCollection *planes );

protected :

    VISUVTKVRADAPTOR_API void doStart() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw (fwTools::Failed);

    VISUVTKVRADAPTOR_API void configuring() throw(fwTools::Failed);

    VISUVTKVRADAPTOR_API void doSwap() throw(fwTools::Failed);


    void resetBoxWidget();

    void activateBoxClipping( bool activate );

    void buildPipeline();

    void updateImage( ::fwData::Image::sptr image  );

    void updateWindowing( ::fwData::Image::sptr image );

    void updateVolumeTransferFunction( ::fwData::Image::sptr image );

    vtkPlaneCollection *m_clippingPlanes;

    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanesId;

    vtkAbstractVolumeMapper  * m_volumeMapper;
    vtkVolumeProperty        * m_volumeProperty;
    vtkVolume                * m_volume;

    vtkPiecewiseFunction     * m_opacityTransferFunction;
    vtkColorTransferFunction * m_colorTransferFunction;

    vtkCommand               * m_abortCommand;

    vtkBoxWidget2 * m_boxWidget;
    vtkCommand    * m_croppingCommand;
    bool m_bClippingBoxIsActivate;

private:

    bool   m_autoResetCamera;
};




} //namespace visuVTKVRAdaptor

#endif // _VISUVTKVRADAPTOR_NEGATOONESLICE_HPP_
