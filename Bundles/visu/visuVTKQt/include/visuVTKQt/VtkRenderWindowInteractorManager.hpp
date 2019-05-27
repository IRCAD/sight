/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "visuVTKQt/config.hpp"

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <QPointer>

class vtkRenderWindowInteractor;

QT_BEGIN_NAMESPACE
class QVTKOpenGLNativeWidget;
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

    QPointer<QVTKOpenGLNativeWidget> m_QVTKOpenGLWidget;

    vtkRenderWindowInteractor* m_interactor;

    SPTR(::fwGuiQt::container::QtContainer) m_parentContainer;

};

} // namespace visuVTKQt
