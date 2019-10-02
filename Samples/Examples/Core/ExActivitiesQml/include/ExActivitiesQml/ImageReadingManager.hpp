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

#include "ExActivitiesQml/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>

#include <fwQml/IQmlAppManager.hpp>

#include <fwServices/AppManager.hpp>
#include <fwServices/IService.hpp>

#include <fwVTKQml/FrameBufferItem.hpp>

#include <QObject>

/**
 * @brief AppManager for a Qml application that allows to read an image and display it a basic 3D scene
 *
 * It is associated to `ExImageReading.qml` file.
 */
class EXACTIVITIESQML_CLASS_API ImageReadingManager : public ::fwQml::IQmlAppManager,
                                                      public ::fwCom::HasSignals
{
Q_OBJECT
typedef ::fwVTKQml::FrameBufferItem FrameBufferItem;
Q_PROPERTY(FrameBufferItem* frameBuffer MEMBER m_frameBuffer)

public:
    /// Constructor.
    EXACTIVITIESQML_API ImageReadingManager() noexcept;

    /// Destructor. Do nothing.
    EXACTIVITIESQML_API ~ImageReadingManager() noexcept override;

public Q_SLOTS:

    /// Initialize the manager
    void initialize() override;

    /// Uninitialize the manager
    void uninitialize() override;

    /// Create the VTK scene and its adaptors
    void createVtkScene();

    /// Open the image
    void openImage();

private:

    typedef ::fwCom::Signal<void ()> VoidSignalType;

    bool m_vtkSceneCreated {false};
    ::fwVTKQml::FrameBufferItem* m_frameBuffer{nullptr};

    ::fwServices::IService::sptr m_imageAdaptor;
};
