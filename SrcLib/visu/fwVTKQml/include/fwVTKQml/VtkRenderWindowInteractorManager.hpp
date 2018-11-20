/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVTKQml/config.hpp"
#include "fwVTKQml/FrameBufferItem.hpp"

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <QPointer>

class vtkRenderWindowInteractor;

namespace fwGui
{
namespace container
{
class fwContainer;
}
}

namespace fwVTKQml
{

/**
 * @brief  Manage vtkRenderWindowInteractor in a qml interface.
 */
class FWVTKQML_CLASS_API VtkRenderWindowInteractorManager : public ::fwRenderVTK::IVtkRenderWindowInteractorManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (VtkRenderWindowInteractorManager)
                                                (::fwRenderVTK::IVtkRenderWindowInteractorManager) )

    /// Constructor. Do nothing
    FWVTKQML_API VtkRenderWindowInteractorManager(::fwRenderVTK::IVtkRenderWindowInteractorManager::Key key);

    /// Destructor
    FWVTKQML_API virtual ~VtkRenderWindowInteractorManager();

    /// Does nothing.
    FWVTKQML_API virtual void installInteractor( ::fwGui::container::fwContainer::sptr _parent ) override;

    /// Does nothing
    FWVTKQML_API virtual void uninstallInteractor() override;

    /// Return a pointer on interactor
    FWVTKQML_API virtual vtkRenderWindowInteractor* getInteractor() override;

    /// Set the qml frame buffer item used to render the scene
    FWVTKQML_API void setFrameBuffer(QPointer< ::fwVTKQml::FrameBufferItem > frameBuffer );

private:

    /// frame buffer item
    QPointer< ::fwVTKQml::FrameBufferItem > m_frameBufferItem;

};

} // namespace fwVTKQml
