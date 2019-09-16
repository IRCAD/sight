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
 * @brief   This class is started when the bundles is loaded.
 */
class EXACTIVITIESQML_CLASS_API ImageDisplayingManager : public ::fwQml::IQmlAppManager,
                                                         public ::fwCom::HasSignals
{
Q_OBJECT
typedef ::fwVTKQml::FrameBufferItem FrameBufferItem;
Q_PROPERTY(FrameBufferItem* frameBuffer MEMBER m_frameBuffer)

public:
    /// Constructor.
    EXACTIVITIESQML_API ImageDisplayingManager() noexcept;

    /// Destructor. Do nothing.
    EXACTIVITIESQML_API ~ImageDisplayingManager() noexcept override;

public Q_SLOTS:
    // Initialize the manager
    void initialize() override;

    /// Uninitialize the manager
    void uninitialize() override;

    /// Create the VTK scene and its adaptors
    void createVtkScene();

    /// Register the services instanciated from Qml
    void onServiceCreated(const QVariant& obj) override;

    void onUpdateSliceMode(int mode);
    void onShowScan(bool isShown);

private:

    typedef ::fwCom::Signal<void ()> VoidSignalType;

    bool m_vtkSceneCreated {false};
    ::fwVTKQml::FrameBufferItem* m_frameBuffer;

    ::fwServices::IService::sptr m_imageAdaptor;
    ::fwServices::IService::sptr m_modelSeriesAdaptor;

    ::fwCom::SlotBase::sptr m_slotRecSelected;
    ::fwCom::SlotBase::sptr m_slotEmptySelection;
};
