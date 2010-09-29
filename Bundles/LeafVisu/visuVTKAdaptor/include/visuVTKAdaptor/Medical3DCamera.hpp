/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_MEDICAL3DCAMERA_HPP_
#define _VISUVTKADAPTOR_MEDICAL3DCAMERA_HPP_

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Medical3DCamera: public ::fwComEd::helper::MedicalImageAdaptor,public ::fwRenderVTK::IVtkAdaptorService 
{
public:

    fwCoreServiceClassDefinitionsMacro ( (Medical3DCamera)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Medical3DCamera() throw();

    VISUVTKADAPTOR_API virtual ~Medical3DCamera() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);


    void initializeCamera();

private:
    void updateView();
    void resetSagittalView();
    void resetFrontalView();
    void resetAxialView();


    vtkCamera* m_camera;
    static std::map< std::string, ::fwComEd::helper::MedicalImageAdaptor::Orientation > m_orientationConversion;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_MEDICAL3DCAMERA_HPP_
