/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_INTERACTORSTYLE_HPP__
#define __VISUVTKADAPTOR_INTERACTORSTYLE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>


class vtkInteractorStyle;

namespace visuVTKAdaptor
{

/**
 * @brief Managing vtk interactor style
 */
class VISUVTKADAPTOR_CLASS_API InteractorStyle : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (InteractorStyle)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API InteractorStyle() throw();

    VISUVTKADAPTOR_API virtual ~InteractorStyle() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    void setInteractorStyle(vtkInteractorStyle *interactor);

    std::string m_configuredStyle;
    vtkInteractorStyle *m_interactorStyle;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_INTERACTORSTYLE_HPP__
