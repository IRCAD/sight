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

class vtkObject;


namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API NegatoOneSlice: public ::fwComEd::helper::MedicalImageAdaptor, public ::fwRenderVTK::IVtkAdaptorService 
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoOneSlice)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API NegatoOneSlice() throw();

    VISUVTKADAPTOR_API virtual ~NegatoOneSlice() throw();

    VISUVTKADAPTOR_API void setAllowAlphaInTF(bool allow) {m_allowAlphaInTF = allow;};
    VISUVTKADAPTOR_API void setInterpolation(bool interpolation){m_interpolation = interpolation;};
    VISUVTKADAPTOR_API void setVtkImageSourceId(std::string id) {m_imageSourceId = id;};
    VISUVTKADAPTOR_API void setVtkImageSource(vtkObject *obj)   {m_imageSource = obj;};

protected :

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);

    bool m_manageImageSource;
    std::string m_imageSourceId;
    vtkObject  *m_imageSource;

    bool m_allowAlphaInTF;
    bool m_interpolation;

    vtkObject* getImageSource();
    ::fwRenderVTK::IVtkAdaptorService::sptr getImageSliceAdaptor();
    ::fwRenderVTK::IVtkAdaptorService::sptr getImageAdaptor();

    ::fwRenderVTK::IVtkAdaptorService::wptr m_imageAdaptor;
    ::fwRenderVTK::IVtkAdaptorService::wptr m_imageSliceAdaptor;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOONESLICE_HPP_
