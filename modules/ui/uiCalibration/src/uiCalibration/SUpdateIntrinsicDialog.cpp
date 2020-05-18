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

#include "uiCalibration/SUpdateIntrinsicDialog.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QObject>

#include <sstream>


namespace uiCalibration
{
//-----------------------------------------------------------------------------

SUpdateIntrinsicDialog::SUpdateIntrinsicDialog() : QDialog(), m_ratio(0.)
{
    //Design of the QDialog
    QHBoxLayout              *validateButtonLayout;
    QHBoxLayout              *computeButtonLayout;
    QHBoxLayout              *resolutionLayout;
    QVBoxLayout              *mainLayout;
    QGridLayout              *parametersLayout;
    QPushButton              *validateButton;
    QPushButton              *cancelButton;
    QPushButton              *computeButton;
    QPushButton              *resetButton;

    parametersLayout = new QGridLayout;
    resolutionLayout = new QHBoxLayout;

    m_width  = new QLineEdit();
    m_height = new QLineEdit();

    QLabel* wLabel = new QLabel("Width :");
    QLabel* hLabel = new QLabel("Height :");

    resolutionLayout->addWidget(wLabel);
    resolutionLayout->addWidget(m_width);
    resolutionLayout->addWidget(hLabel);
    resolutionLayout->addWidget(m_height);

    m_cx   = new QLabel();
    m_cy   = new QLabel();
    m_fx   = new QLabel();
    m_fy   = new QLabel();
    m_k1   = new QLabel();
    m_k2   = new QLabel();
    m_p1   = new QLabel();
    m_p2   = new QLabel();
    m_k3   = new QLabel();
    m_skew = new QLabel();

    parametersLayout->addWidget(m_skew, 0, 2);

    parametersLayout->addWidget(m_cx, 1, 0);
    parametersLayout->addWidget(m_cy, 1, 1);
    parametersLayout->addWidget(m_fx, 1, 2);
    parametersLayout->addWidget(m_fy, 1, 3);

    parametersLayout->addWidget(m_k1, 2, 0);
    parametersLayout->addWidget(m_k2, 2, 1);
    parametersLayout->addWidget(m_p1, 2, 2);
    parametersLayout->addWidget(m_p2, 2, 3);
    parametersLayout->addWidget(m_k3, 2, 4);

    validateButtonLayout = new QHBoxLayout();
    computeButtonLayout  = new QHBoxLayout();
    mainLayout           = new QVBoxLayout();

    computeButton = new QPushButton("Compute");
    resetButton   = new QPushButton("Reset");

    validateButton = new QPushButton("Validate");
    cancelButton   = new QPushButton("Cancel");

    computeButtonLayout->addWidget(computeButton);
    computeButtonLayout->addWidget(resetButton);

    validateButtonLayout->addWidget(validateButton);
    validateButtonLayout->addWidget(cancelButton);

    mainLayout->addLayout(resolutionLayout);
    mainLayout->addLayout(computeButtonLayout);
    mainLayout->addLayout(parametersLayout);
    mainLayout->addLayout(validateButtonLayout);

    this->setModal(true);
    this->setLayout(mainLayout);
    this->setWindowTitle("Calibration Edition");

    QObject::connect(computeButton, SIGNAL(clicked()), this, SLOT(onPushCompute()));
    QObject::connect(resetButton, SIGNAL(clicked()), this, SLOT(onPushReset()));
    QObject::connect(validateButton, SIGNAL(clicked()), this, SLOT(onValidate()));
    QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

//-----------------------------------------------------------------------------

SUpdateIntrinsicDialog::~SUpdateIntrinsicDialog()
{
}

//-----------------------------------------------------------------------------

void SUpdateIntrinsicDialog::setParameters(std::array< double, 12 > &parameters)
{
    m_calibration       = parameters;
    m_originCalibration = parameters;

    m_ratio = m_calibration[0] / m_calibration[1];

    this->updateInfos();
}

//-----------------------------------------------------------------------------

void SUpdateIntrinsicDialog::onValidate()
{
    Q_EMIT newCalibration(m_calibration);

    this->close();
}

//-----------------------------------------------------------------------------

void SUpdateIntrinsicDialog::onPushCompute()
{

    double height = m_height->text().toDouble();
    double width  = m_width->text().toDouble();

    double ratio = width / height;

    //new resolution don't respect the original ratio
    if(std::abs(m_ratio - ratio) > 0.0001)
    {
        ::fwGui::dialog::MessageDialog::sptr warningMess = ::fwGui::dialog::MessageDialog::New();
        warningMess->showMessageDialog("Warning","The new resolution don't respect the original resolution ratio !"
                                       ,::fwGui::dialog::IMessageDialog::WARNING);
        return;
    }

    //alpha : original resolution / new resolution (!! Ratio should be kept (16/9, 4/3 ...) !!!)

    double alpha = m_originCalibration[1]/ height;

    double fx_new, fy_new, cx_new, cy_new, k1_new, k2_new, p1_new, p2_new;

    //    fx_new = fx_old / alpha
    //    fy_new = fy_old / alpha
    //    cx_new = cx_old / alpha
    //    cy_new = cy_old / alpha

    fx_new = m_calibration[2] / alpha;
    fy_new = m_calibration[3] / alpha;
    cx_new = m_calibration[4] / alpha;
    cy_new = m_calibration[5] / alpha;

    //    k1_new = k1_old*alpha^2
    //    k2_new = k2_old*alpha^4
    //    t1_new = t1_old*alpha^2
    //    t2_new = t2_old*alpha^2
    k1_new = m_calibration[6]*(alpha*alpha);
    k2_new = m_calibration[7]*(alpha*alpha*alpha*alpha);
    p1_new = m_calibration[8]*(alpha*alpha);
    p2_new = m_calibration[9]*(alpha*alpha);

    m_calibration[0] = width;
    m_calibration[1] = height;
    m_calibration[2] = fx_new;
    m_calibration[3] = fy_new;
    m_calibration[4] = cx_new;
    m_calibration[5] = cy_new;
    m_calibration[6] = k1_new;
    m_calibration[7] = k2_new;
    m_calibration[8] = p1_new;
    m_calibration[9] = p2_new;

    this->updateInfos();
}

//-----------------------------------------------------------------------------

void SUpdateIntrinsicDialog::onPushReset()
{
    for(unsigned int i = 0; i< m_originCalibration.size(); ++i)
    {
        m_calibration[i] = m_originCalibration[i];
    }

    this->updateInfos();
}

//-----------------------------------------------------------------------------

void SUpdateIntrinsicDialog::updateInfos()
{
    std::stringstream out;

    out<<m_calibration[0];
    m_width->setText(out.str().c_str());
    out.str("");

    out<<m_calibration[1];
    m_height->setText(out.str().c_str());
    out.str("");

    //CX
    out << "Cx: <font color='#0066CC'>" << m_calibration[4] << "</font>";
    m_cx->setText(out.str().c_str());

    out.str("");

    //CY
    out << "Cy: <font color='#0066CC'>" << m_calibration[5] << "</font>";
    m_cy->setText(out.str().c_str());

    out.str("");

    //FX
    out << "Fx: <font color='#0066CC'>" << m_calibration[2] << "</font>";
    m_fx->setText(out.str().c_str());

    out.str("");

    //FY
    out << "Fy: <font color='#0066CC'>" << m_calibration[3] << "</font>";
    m_fy->setText(out.str().c_str());

    out.str("");

    //K1
    out << "K1: <font color='#0066CC'>" << m_calibration[6] << "</font>";
    m_k1->setText(out.str().c_str());

    out.str("");

    //K2
    out << "K2: <font color='#0066CC'>" << m_calibration[7] << "</font>";
    m_k2->setText(out.str().c_str());

    out.str("");

    //P1
    out << "P1: <font color='#0066CC'>" << m_calibration[8] << "</font>";
    m_p1->setText(out.str().c_str());

    out.str("");

    //P2
    out << "P2: <font color='#0066CC'>" << m_calibration[9] << "</font>";
    m_p2->setText(out.str().c_str());

    out.str("");

    //K3
    out << "K3: <font color='#0066CC'>" << m_calibration[10] << "</font>";
    m_k3->setText(out.str().c_str());

    out.str("");

    //SKEW
    out << "Skew: <font color='#0066CC'>" << m_calibration[11] << "</font>";
    m_skew->setText(out.str().c_str());
}

//-----------------------------------------------------------------------------

} //namespace uiCalibration
