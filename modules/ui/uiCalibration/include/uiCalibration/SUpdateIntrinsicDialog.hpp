/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
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

#ifndef __UICALIBRATION_SUPDATEINTRINSICDIALOG_HPP__
#define __UICALIBRATION_SUPDATEINTRINSICDIALOG_HPP__

#include "uiCalibration/config.hpp"

#include <QObject>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>

#include <array>

namespace uiCalibration
{
/**
 * @brief The SUpdateIntrinsicDialog class displays a QDialog. The user can change the calibration resolution which leads to an automatic computation of the new calibration parameters. By validating the user entry, the calibration will be updated.
 */
class UICALIBRATION_CLASS_API SUpdateIntrinsicDialog : public QDialog
{

Q_OBJECT;

public:

    /// constructor
    SUpdateIntrinsicDialog();

    /// destructor
    ~SUpdateIntrinsicDialog();

    /**
     * @brief setParameters
     * @param calibration parameters in a array< double , 12>
     */
    void setParameters(std::array< double, 12>& parameters);

Q_SIGNALS:

    void newCalibration(std::array< double, 12 >& newParameters);


private Q_SLOTS:

    /**
     * @brief onPushCompute compute the calibration with new resolution
     */
    void onPushCompute();

    /**
     * @brief onPushReset reset the original calibration
     */
    void onPushReset();

    /**
     * @brief onValidate send the signal newCalibration
     */
    void onValidate();

private:

    /**
     * @brief updateInfos display new calibration
     */
    void updateInfos();

    /**
     * @brief concatenation of resolution (0-1) , intrinsic parameters (2-5), distortions (6-10), and skew(11)
     */
    std::array< double, 12 > m_calibration;
    /**
     * @brief store the original calibration (to reset)
     */
    std::array< double, 12 > m_originCalibration;

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
     * @brief Field of view x.
     */
    QLabel* m_fx;

    /**
     * @brief Field of view y.
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
    double m_ratio;


};

} //namespace uiCalibration

#endif // __UICALIBRATION_SUPDATEINTRINSICDIALOG_HPP__
