/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "Tuto09MesherWithGenericSceneQml/config.hpp"

#include <fwCom/SlotBase.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwServices/AppManager.hpp>

#include <fwVTKQml/FrameBufferItem.hpp>

#include <QObject>

namespace Tuto09MesherWithGenericSceneQml
{

/**
 * @brief   This class manages the services used by the Application. It is launched by the qml file 'ui.qml'
 */
class TUTO09MESHERWITHGENERICSCENEQML_CLASS_API AppManager : public QObject,
                                                             public ::fwServices::AppManager
{

typedef ::fwVTKQml::FrameBufferItem FrameBufferItem;
Q_OBJECT;
Q_PROPERTY(FrameBufferItem* frameBuffer MEMBER m_frameBuffer)
public:
    /// Constructor.
    TUTO09MESHERWITHGENERICSCENEQML_API AppManager() noexcept;

    /// Destructor. Do nothing.
    TUTO09MESHERWITHGENERICSCENEQML_API ~AppManager() noexcept;

public Q_SLOTS:
    void initialize();
    void uninitialize();
    void createVtkScene();
    void onOpenImage();
    void onSaveMesh();
    void applyMesher(unsigned int reduction);
    void onUpdateSliceMode(int mode);
    void onShowScan(bool isShown);
    void onServiceCreated(const QVariant& obj);

Q_SIGNALS:

    void imageLoaded();

private:

    bool m_vtkSceneCreated {false};
    ::fwVTKQml::FrameBufferItem* m_frameBuffer;
    ::fwMedData::ImageSeries::sptr m_loadedImageSeries;
    ::fwMedData::ModelSeries::sptr m_modelSeries;

    ::fwServices::IService::sptr m_imageAdaptor;
    ::fwServices::IService::sptr m_modelSeriesAdaptor;

    ::fwCom::SlotBase::sptr m_slotRecSelected;
    ::fwCom::SlotBase::sptr m_slotEmptySelection;

};

} // namespace Tuto09MesherWithGenericSceneQml
