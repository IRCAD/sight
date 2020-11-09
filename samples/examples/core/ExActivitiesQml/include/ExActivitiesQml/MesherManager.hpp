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
 * @brief AppManager for a Qml application that allows to generate models from a 3D image mask.
 *
 * It is associated to `ExMesher.qml` file.
 */
class EXACTIVITIESQML_CLASS_API MesherManager :
    public ::fwQml::IQmlAppManager,
    public ::fwCom::HasSignals
{

Q_OBJECT

typedef ::fwVTKQml::FrameBufferItem FrameBufferItem;

Q_PROPERTY(FrameBufferItem* frameBuffer MEMBER m_frameBuffer)

public:

    /// Adds requires objects.
    EXACTIVITIESQML_API MesherManager() noexcept;

    /// Calls initialize().
    EXACTIVITIESQML_API ~MesherManager() noexcept override;

public Q_SLOTS:

    /// Initializes the manager.
    void initialize() override;

    /// Uninitializes the manager.
    void uninitialize() override;

    /// Creates the VTK scene and its adaptors.
    void createVtkScene();

    /// Registers the services instanciated from Qml.
    void onServiceCreated(const QVariant& obj) override;

    /// Tells the image adaptor to change the slice mode.
    void onUpdateSliceMode(int mode);

    /// Tells the image adaptor to show/hide image slices.
    void onShowScan(bool isShown);

    /// Generates the mesh.
    void applyMesher(unsigned int reduction);

private:

    typedef ::fwCom::Signal<void ()> VoidSignalType;

    /// Defines if scene has been created.
    bool m_vtkSceneCreated { false };

    /// Contains the frame buffer given to the interactor manager.
    ::fwVTKQml::FrameBufferItem* m_frameBuffer { nullptr };

    /// Contains the negato adaptor.
    ::fwServices::IService::sptr m_imageAdaptor { nullptr };

    /// Contains the model series adaptor.
    ::fwServices::IService::sptr m_modelSeriesAdaptor { nullptr };

};
