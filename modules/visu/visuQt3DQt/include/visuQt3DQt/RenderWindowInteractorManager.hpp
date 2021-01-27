/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "visuQt3DQt/config.hpp"

#include <core/base.hpp>

#include <fwRenderQt3D/IRenderWindowInteractorManager.hpp>

#include <gui/container/fwContainer.hpp>

#include <QObject>
#include <QPointer>

namespace sight::guiQt
{
namespace container
{
class QtContainer;
}
}

namespace Qt3DExtras
{
class Qt3DWindow;
}

namespace visuQt3DQt
{

/// Defines a class to manage a render window.
class VISUQT3DQT_CLASS_API RenderWindowInteractorManager : public QObject,
                                                           public ::fwRenderQt3D::IRenderWindowInteractorManager
{

Q_OBJECT

public:

    fwCoreClassMacro(RenderWindowInteractorManager, ::fwRenderQt3D::IRenderWindowInteractorManager)

    /// Initialize members.
    VISUQT3DQT_API RenderWindowInteractorManager(::fwRenderQt3D::IRenderWindowInteractorManager::Key);

    /// Destroys the window container.
    VISUQT3DQT_API ~RenderWindowInteractorManager() final;
    /**
     * @brief Creates the container that holds the QtWidget.
     * @param _parent the parent container of the widget.
     * @param _renderOnDemand if true, the rendering will be done only when it's requested.
     * @param _fullscreen enable the fullscreen.
     */
    VISUQT3DQT_API void createContainer(gui::container::fwContainer::sptr _parent) final;

    VISUQT3DQT_API void set3DView(Qt3DExtras::Qt3DWindow* _3dView) final;

private:

    /// Contains the parent of the widget.
    SPTR(guiQt::container::QtContainer) m_parentContainer;

    /// Contains the window container.
    QPointer< QWidget > m_windowContainer;

};

} // namespace visuQt3DQt.
