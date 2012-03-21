/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOSLICINGINTERACTOR_HPP_
#define _VISUVTKADAPTOR_NEGATOSLICINGINTERACTOR_HPP_

#include <fwData/TransferFunction.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class VISUVTKADAPTOR_CLASS_API vtkCommand;

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API NegatoSlicingInteractor: public ::fwComEd::helper::MedicalImageAdaptor,public ::fwRenderVTK::IVtkAdaptorService 
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoSlicingInteractor)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API NegatoSlicingInteractor() throw();

    VISUVTKADAPTOR_API virtual ~NegatoSlicingInteractor() throw();

    VISUVTKADAPTOR_API void startSlicing(double pickedPoint[3]);

    VISUVTKADAPTOR_API void stopSlicing();

    VISUVTKADAPTOR_API void updateSlicing( double pickedPoint[3]);

    VISUVTKADAPTOR_API void pushSlice( int factor, Orientation axis);

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    vtkCommand *m_vtkObserver;
    double m_priority;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOSLICINGINTERACTOR_HPP_
