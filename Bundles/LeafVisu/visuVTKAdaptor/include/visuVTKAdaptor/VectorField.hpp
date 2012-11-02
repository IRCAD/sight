/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_IMAGE_HPP_
#define _VISUVTKADAPTOR_IMAGE_HPP_

#include <vtkSmartPointer.h>

#include <fwData/Image.hpp>



#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkPolyDataAlgorithm;
class vtkImageData;
class vtkGlyph3D;



namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API VectorField: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (VectorField)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API VectorField() throw();

    VISUVTKADAPTOR_API virtual ~VectorField() throw();

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);


    virtual void buildPipeline();
    virtual void destroyPipeline();

    vtkSmartPointer<vtkPolyDataAlgorithm> m_arrowSource;
    vtkSmartPointer<vtkImageData> m_imageData;
    vtkSmartPointer<vtkGlyph3D> glyphFilter;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_IMAGE_HPP_
