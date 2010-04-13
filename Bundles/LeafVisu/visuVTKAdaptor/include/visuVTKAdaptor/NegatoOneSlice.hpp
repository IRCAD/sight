/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOONESLICE_HPP_
#define _VISUVTKADAPTOR_NEGATOONESLICE_HPP_

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

class VISUVTKADAPTOR_CLASS_API NegatoOneSlice: public ::fwComEd::helper::MedicalImageAdaptor, public ::fwRenderVTK::IVtkAdaptorService 
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoOneSlice)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API NegatoOneSlice() throw();

    VISUVTKADAPTOR_API virtual ~NegatoOneSlice() throw();

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);


    void buildPipeline();
    void buildOutline();

    void updateOutline();


    // update DisplayExtent according to orientation
    void setSlice( int slice );

    void updateImage( ::fwData::Image::sptr image  );
    void updateSliceIndex( ::fwData::Image::sptr image );
    void updateWindowing( ::fwData::Image::sptr image );
    void updateTransfertFunction( ::fwData::Image::sptr image );

    vtkLookupTable *m_lut;
    vtkImageActor *m_imageActor;
    vtkImageMapToColors *m_map2colors;
    vtkImageData *m_imageData;

    vtkPolyData *m_planeOutlinePolyData;
    vtkPolyDataMapper *m_planeOutlineMapper;
    vtkActor *m_planeOutlineActor;


};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOONESLICE_HPP_
