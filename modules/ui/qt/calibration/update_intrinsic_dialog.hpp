/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QObject>

#include <array>

namespace sight::module::ui::qt::calibration
{

/**
 * @brief The update_intrinsic_dialog class displays a QDialog. The user can change the calibration resolution which
 * leads to an automatic computation of the new calibration parameters. By validating the user entry, the calibration
 * will be updated.
 */
class MODULE_UI_QT_CLASS_API update_intrinsic_dialog : public QDialog
{
Q_OBJECT;

public:

    /// constructor
    update_intrinsic_dialog();

    /// destructor
    ~update_intrinsic_dialog() override;

    /**
     * @brief setParameters
     * @param calibration parameters in a array< double , 12>
     */
    void set_parameters(std::array<double, 12>& _parameters);

Q_SIGNALS:

    void new_calibration(std::array<double, 12>& _new_parameters);

private Q_SLOTS:

    /**
     * @brief onPushCompute compute the calibration with new resolution
     */
    void on_push_compute();

    /**
     * @brief onPushReset reset the original calibration
     */
    void on_push_reset();

    /**
     * @brief onValidate send the signal newCalibration
     */
    void on_validate();

private:

    /**
     * @brief updateInfos display new calibration
     */
    void update_infos();

    /**
     * @brief concatenation of resolution (0-1) , intrinsic parameters (2-5), distortions (6-10), and skew(11)
     */
    std::array<double, 12> m_calibration {};
    /**
     * @brief store the original calibration (to reset)
     */
    std::array<double, 12> m_origin_calibration {};

    /**
     * @brief Width of the video.
     */
    QLineEdit* m_width;

    /**
     * @brief Height of the video.
     */
    QLineEdit* m_height;

    /**
     * @brief Optical center x.
     */
    QLabel* m_cx;

    /**
     * @brief Optical center y.
     */
    QLabel* m_cy;

    /**
     * @brief field of view x.
     */
    QLabel* m_fx;

    /**
     * @brief field of view y.
     */
    QLabel* m_fy;

    /**
     * @brief Distortion coefficient k1.
     */
    QLabel* m_k1;

    /**
     * @brief Distortion coefficient k2.
     */
    QLabel* m_k2;

    /**
     * @brief Distortion coefficient p1.
     */
    QLabel* m_p1;

    /**
     * @brief Distortion coefficient p2.
     */
    QLabel* m_p2;

    /**
     * @brief Distortion coefficient k3.
     */
    QLabel* m_k3;

    /**
     * @brief Camera skew.
     */
    QLabel* m_skew;

    /// ratio of the calibration resolution
    double m_ratio {0.};
};

} //namespace sight::module::ui::qt::calibration
