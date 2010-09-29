/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKQT_VTKRENDERWINDOWINTERACTORMANAGER_HPP_
#define _VISUVTKQT_VTKRENDERWINDOWINTERACTORMANAGER_HPP_

#include <string>

#include <fwCore/base.hpp>

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <fwGui/container/fwContainer.hpp>

#include "visuVTKQt/config.hpp"

class vtkRenderWindowInteractor;

QT_BEGIN_NAMESPACE
class QVTKWidget;
QT_END_NAMESPACE


namespace visuVTKQt
{

/**
 * @brief   Defines a class to manage vtkRenderWindowInteractor in a window.
 * @class   VtkRenderWindowInteractorManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class VISUVTKQT_CLASS_API VtkRenderWindowInteractorManager : public ::fwRenderVTK::IVtkRenderWindowInteractorManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (VtkRenderWindowInteractorManager)(::fwRenderVTK::IVtkRenderWindowInteractorManager) )

    /// Constructor. Do nothing.
    VISUVTKQT_API VtkRenderWindowInteractorManager();

    /// Destructor. Do nothing.
    VISUVTKQT_API virtual ~VtkRenderWindowInteractorManager();

    /// Creates an interactor and installs it in window.
    VISUVTKQT_API virtual void installInteractor( ::fwGui::container::fwContainer::sptr _parent );

    /// Deletes interactor and manage correctly the window (removing layout).
    VISUVTKQT_API virtual void uninstallInteractor();

    /// Return a pointer on interactor
    VISUVTKQT_API virtual ::vtkRenderWindowInteractor * getInteractor();

private:

    QPointer<QVTKWidget> m_qVTKWidget;

    vtkRenderWindowInteractor *m_interactor;

};

} // namespace visuVTKQt

#endif // _VISUVTKQT_VTKRENDERWINDOWINTERACTORMANAGER_HPP_


