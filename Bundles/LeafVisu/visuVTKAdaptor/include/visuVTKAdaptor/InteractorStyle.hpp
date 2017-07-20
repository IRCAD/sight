/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_INTERACTORSTYLE_HPP__
#define __VISUVTKADAPTOR_INTERACTORSTYLE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

class vtkInteractorStyle;

namespace visuVTKAdaptor
{

/**
 * @brief Managing vtk interactor style
 */
class VISUVTKADAPTOR_CLASS_API InteractorStyle : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (InteractorStyle)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API InteractorStyle() noexcept;

    VISUVTKADAPTOR_API virtual ~InteractorStyle() noexcept;

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

    void setInteractorStyle(vtkInteractorStyle* interactor);

    std::string m_configuredStyle;
    vtkInteractorStyle* m_interactorStyle;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_INTERACTORSTYLE_HPP__
