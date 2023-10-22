/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "camera.hpp"

#include "modules/ui/qt/config.hpp"

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

} // namespace sight::data

namespace sight::module::ui::qt::video
{

/**
 * @brief The CameraDeviceDlg class displays a QDialog to choose camera device.
 */
class MODULE_UI_QT_CLASS_API CameraDeviceDlg : public QDialog
{
Q_OBJECT;

public:

    /// constructor
    MODULE_UI_QT_API CameraDeviceDlg(std::string _xml_resolution = "");

    /// destructor
    MODULE_UI_QT_API ~CameraDeviceDlg() override = default;

    MODULE_UI_QT_API bool getSelectedCamera(SPTR(data::camera) & _camera, std::string& _resolution_xml_option);
    // Filter the list of supported resolution to extract the lowest, highest and medium resolution in relation to
    // `resolutionType`
    MODULE_UI_QT_API QSize getResolution(
        const std::string& _resolution_xml_option,
        const QList<QSize>& _supported_resolutions
    );

private Q_SLOTS:

    void onSelectDevice(int _index);

private:

    QComboBox* m_devicesComboBox;
    QListWidget* m_camSettings;
};

} // namespace sight::module::ui::qt::video
