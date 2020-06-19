/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

    fwCoreClassMacro(VtkRenderWindowInteractorManager, ::fwRenderVTK::IVtkRenderWindowInteractorManager);

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
