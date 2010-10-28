/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_IMAGEPICKERINTERACTOR_HPP_
#define _VISUVTKADAPTOR_IMAGEPICKERINTERACTOR_HPP_

#include <vector>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkCommand;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API ImagePickerInteractor: public ::fwComEd::helper::MedicalImageAdaptor, public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ImagePickerInteractor)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API ImagePickerInteractor() throw();

    VISUVTKADAPTOR_API virtual ~ImagePickerInteractor() throw();

protected:
    friend class ImagePickerInteractorCallback;

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    VISUVTKADAPTOR_API void notifyEvent(::fwComEd::InteractionMsg::sptr msg);



    vtkCommand *m_interactionCommand;
    float m_priority;

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_IMAGEPICKERINTERACTOR_HPP_
