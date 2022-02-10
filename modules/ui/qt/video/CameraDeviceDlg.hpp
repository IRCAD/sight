/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "modules/ui/qt/config.hpp"

#include "SCamera.hpp"

#include <core/macros.hpp>

#include <QCameraInfo>
#include <QCameraViewfinderSettings>
#include <QComboBox>
#include <QDialog>
#include <QListWidget>
#include <QObject>

namespace sight::data
{

class Camera;

}

namespace sight::module::ui::qt
{

namespace video
{

/**
 * @brief The CameraDeviceDlg class displays a QDialog to choose camera device.
 */
class MODULE_UI_QT_CLASS_API CameraDeviceDlg : public QDialog
{
Q_OBJECT;

public:

    /// constructor
    MODULE_UI_QT_API CameraDeviceDlg(std::string xmlResolution = "");

    /// destructor
    MODULE_UI_QT_API ~CameraDeviceDlg() override = default;

    MODULE_UI_QT_API bool getSelectedCamera(SPTR(data::Camera) & camera, std::string& resolutionXMLOption);
    // Filter the list of supported resolution to extract the lowest, highest and medium resolution in relation to
    // `resolutionType`
    MODULE_UI_QT_API QSize getResolution(
        const std::string& resolutionXMLOption,
        const QList<QSize>& supportedResolutions
    );

private Q_SLOTS:

    void onSelectDevice(int index);

private:

    QComboBox* m_devicesComboBox;
    QListWidget* m_camSettings;
};

} // video

} // sight::module::ui::qt
