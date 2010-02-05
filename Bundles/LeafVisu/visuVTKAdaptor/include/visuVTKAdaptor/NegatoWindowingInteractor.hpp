/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NEGATOWINDOWINGINTERACTOR_HPP_
#define _VISUVTKADAPTOR_NEGATOWINDOWINGINTERACTOR_HPP_

#include <fwData/TransfertFunction.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkCommand;

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API NegatoWindowingInteractor: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (NegatoWindowingInteractor)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API NegatoWindowingInteractor() throw();

    VISUVTKADAPTOR_API virtual ~NegatoWindowingInteractor() throw();

    VISUVTKADAPTOR_API void startWindowing();

    VISUVTKADAPTOR_API void stopWindowing();

    VISUVTKADAPTOR_API void updateWindowing( double , double);

    VISUVTKADAPTOR_API void resetWindowing();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    vtkCommand *m_vtkObserver;
    double m_initialWindow;
    double m_initialLevel;
    double m_priority;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NEGATOWINDOWINGINTERACTOR_HPP_
