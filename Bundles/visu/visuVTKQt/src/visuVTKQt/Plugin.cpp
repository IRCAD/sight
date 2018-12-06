/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "visuVTKQt/Plugin.hpp"

#include <fwCore/spyLog.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <QApplication>
#include <QSurfaceFormat>
#include <QVTKOpenGLWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

namespace visuVTKQt
{

//-----------------------------------------------------------------------------

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("visuVTKQt::Plugin");

//-----------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
    // Since we share the opengl context with Qt, we must set the default QSurfaceFormat before QApplication
    // to allow QVTKOpenGLWidget to work properly
    SLM_ASSERT( "QApplication is already created! The default surface format cannot be changed afterward.\n"
                "This problem can happen when the profile.xml is handwritten and contains 'visuVTKQt' and 'guiQt'.\n"
                "If that is the case, please ensure that 'visuVTKQt' is started before 'guiQt'.\n"
                "It could also happen for generated 'profile.xml' files, check it to be sure.",
                qApp == nullptr );

    vtkOpenGLRenderWindow::SetGlobalMaximumNumberOfMultiSamples(0);
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
}

//-----------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//-----------------------------------------------------------------------------

void Plugin::start()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void Plugin::stop() noexcept
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

} // namespace visuVTKQt
