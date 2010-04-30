/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_IMAGE_HPP_
#define _VISUVTKADAPTOR_IMAGE_HPP_

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

class VISUVTKADAPTOR_CLASS_API Image: public ::fwComEd::helper::MedicalImageAdaptor, public ::fwRenderVTK::IVtkAdaptorService 
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Image)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Image() throw();

    VISUVTKADAPTOR_API virtual ~Image() throw();

    void setVtkImageRegisterId(std::string id) {m_imageRegisterId = id;};
    void setVtkImageRegister(vtkObject *obj)   {m_imageRegister = obj;};
    void setImageOpacity(double opacity)       {m_imageOpacity = opacity;};

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);


    virtual void buildPipeline();

    void updateImage( ::fwData::Image::sptr image  );
    void updateImageOpacity();
    void updateWindowing( ::fwData::Image::sptr image );
    void updateTransfertFunction( ::fwData::Image::sptr image );

    std::string m_imageRegisterId;
    vtkObject  *m_imageRegister;

    int    m_imagePortId;
    double m_imageOpacity;

    vtkLookupTable *m_lut;
    vtkImageMapToColors *m_map2colors;
    vtkImageData *m_imageData;


};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_IMAGE_HPP_
