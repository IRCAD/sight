/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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
 * @brief AppManager for a Qml application that allows to display an image and a model.
 *
 * It displays the organ manager that allows to change the color and visibility of the organs.
 * It is associated to `ExImageDisplaying.qml` file.
 */
class EXACTIVITIESQML_CLASS_API ImageDisplayingManager :
    public ::fwQml::IQmlAppManager,
    public ::fwCom::HasSignals
{

Q_OBJECT

typedef ::fwVTKQml::FrameBufferItem FrameBufferItem;

Q_PROPERTY(FrameBufferItem* frameBuffer MEMBER m_frameBuffer)

public:

    /// Adds requires objects.
    EXACTIVITIESQML_API ImageDisplayingManager() noexcept;

    /// Calls initialize().
    EXACTIVITIESQML_API ~ImageDisplayingManager() noexcept override;

public Q_SLOTS:

    /// Initializes the manager.
    EXACTIVITIESQML_API void initialize() override;

    /// Uninitializes the manager.
    EXACTIVITIESQML_API void uninitialize() override;

    /// Creates the VTK scene and its adaptors.
    EXACTIVITIESQML_API void createVtkScene();

    /// Registers the services instanciated from Qml.
    EXACTIVITIESQML_API void onServiceCreated(const QVariant& obj) override;

    /// Tells the image adaptor to change the slice mode.
    EXACTIVITIESQML_API void onUpdateSliceMode(int mode);

    /// Tells the image adaptor to show/hide image slices.
    EXACTIVITIESQML_API void onShowScan(bool isShown);

private:

    /// Defines if the scene has been created.
    bool m_sceneCreated { false };

    /// Contains the frame buffer given to the interactor manager.
    ::fwVTKQml::FrameBufferItem* m_frameBuffer { nullptr };

    /// Contains the negato adaptor.
    ::fwServices::IService::sptr m_imageAdaptor { nullptr };

    /// Contains the model series adaptor.
    ::fwServices::IService::sptr m_modelSeriesAdaptor { nullptr };

    ::fwCom::SlotBase::sptr m_slotRecSelected { nullptr };

    ::fwCom::SlotBase::sptr m_slotEmptySelection { nullptr };

};
