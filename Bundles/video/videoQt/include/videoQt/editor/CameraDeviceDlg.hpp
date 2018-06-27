/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "videoQt/config.hpp"

#include <fwCore/macros.hpp>

#include <QCameraInfo>
#include <QCameraViewfinderSettings>
#include <QComboBox>
#include <QDialog>
#include <QListWidget>
#include <QObject>

namespace arData
{
class Camera;
}

namespace videoQt
{
namespace editor
{

/**
 * @brief The CameraDeviceDlg class displays a QDialog to choose camera device.
 */
class VIDEOQT_CLASS_API CameraDeviceDlg : public QDialog
{
Q_OBJECT;

public:

    /// constructor
    VIDEOQT_API CameraDeviceDlg();

    /// destructor
    VIDEOQT_API ~CameraDeviceDlg();

    VIDEOQT_API bool getSelectedCamera( SPTR(::arData::Camera)& camera);

private Q_SLOTS:

    void onSelectDevice(int index);

private:
    QComboBox* m_devicesComboBox;
    QListWidget* m_camSettings;

};

} // editor
} // videoQt
