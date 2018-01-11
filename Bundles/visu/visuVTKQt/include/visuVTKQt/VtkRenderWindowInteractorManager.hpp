/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKQT_VTKRENDERWINDOWINTERACTORMANAGER_HPP__
#define __VISUVTKQT_VTKRENDERWINDOWINTERACTORMANAGER_HPP__

#include "visuVTKQt/config.hpp"

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <QPointer>

class vtkRenderWindowInteractor;

QT_BEGIN_NAMESPACE
class QVTKOpenGLWidget;
QT_END_NAMESPACE

namespace fwGuiQt
{
namespace container
{
class QtContainer;
}
}

namespace visuVTKQt
{

/**
 * @brief   Defines a class to manage vtkRenderWindowInteractor in a window.
 */
class VISUVTKQT_CLASS_API VtkRenderWindowInteractorManager : public ::fwRenderVTK::IVtkRenderWindowInteractorManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (VtkRenderWindowInteractorManager)
                                                (::fwRenderVTK::IVtkRenderWindowInteractorManager) )

    VISUVTKQT_API VtkRenderWindowInteractorManager(::fwRenderVTK::IVtkRenderWindowInteractorManager::Key key);

    VISUVTKQT_API virtual ~VtkRenderWindowInteractorManager();

    /// Creates an interactor and installs it in window.
    VISUVTKQT_API virtual void installInteractor( ::fwGui::container::fwContainer::sptr _parent ) override;

    /// Deletes interactor and manage correctly the window (removing layout).
    VISUVTKQT_API virtual void uninstallInteractor() override;

    /// Return a pointer on interactor
    VISUVTKQT_API virtual ::vtkRenderWindowInteractor* getInteractor() override;

private:

    QPointer<QVTKOpenGLWidget> m_QVTKOpenGLWidget;

    vtkRenderWindowInteractor* m_interactor;

    SPTR(::fwGuiQt::container::QtContainer) m_parentContainer;

};

} // namespace visuVTKQt

#endif // __VISUVTKQT_VTKRENDERWINDOWINTERACTORMANAGER_HPP__

