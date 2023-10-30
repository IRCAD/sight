/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/ui/qt/calibration/update_intrinsic_dialog.hpp"

#include <ui/__/dialog/message.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QObject>
#include <QPushButton>
#include <QVBoxLayout>

#include <cmath>
#include <sstream>

namespace sight::module::ui::qt::calibration
{

//-----------------------------------------------------------------------------

update_intrinsic_dialog::update_intrinsic_dialog() :

    m_width(new QLineEdit()),
    m_height(new QLineEdit()),
    m_cx(new QLabel()),
    m_cy(new QLabel()),
    m_fx(new QLabel()),
    m_fy(new QLabel()),
    m_k1(new QLabel()),
    m_k2(new QLabel()),
    m_p1(new QLabel()),
    m_p2(new QLabel()),
    m_k3(new QLabel()),
    m_skew(new QLabel())
{
    //Design of the QDialog
    QHBoxLayout* validate_button_layout = nullptr;
    QHBoxLayout* compute_button_layout  = nullptr;
    QHBoxLayout* resolution_layout      = nullptr;
    QVBoxLayout* main_layout            = nullptr;
    QGridLayout* parameters_layout      = nullptr;
    QPushButton* validate_button        = nullptr;
    QPushButton* cancel_button          = nullptr;
    QPushButton* compute_button         = nullptr;
    QPushButton* reset_button           = nullptr;

    parameters_layout = new QGridLayout;
    resolution_layout = new QHBoxLayout;

    auto* w_label = new QLabel("Width :");
    auto* h_label = new QLabel("Height :");

    resolution_layout->addWidget(w_label);
    resolution_layout->addWidget(m_width);
    resolution_layout->addWidget(h_label);
    resolution_layout->addWidget(m_height);

    parameters_layout->addWidget(m_skew, 0, 2);

    parameters_layout->addWidget(m_cx, 1, 0);
    parameters_layout->addWidget(m_cy, 1, 1);
    parameters_layout->addWidget(m_fx, 1, 2);
    parameters_layout->addWidget(m_fy, 1, 3);

    parameters_layout->addWidget(m_k1, 2, 0);
    parameters_layout->addWidget(m_k2, 2, 1);
    parameters_layout->addWidget(m_p1, 2, 2);
    parameters_layout->addWidget(m_p2, 2, 3);
    parameters_layout->addWidget(m_k3, 2, 4);

    validate_button_layout = new QHBoxLayout();
    compute_button_layout  = new QHBoxLayout();
    main_layout            = new QVBoxLayout();

    compute_button = new QPushButton("Compute");
    reset_button   = new QPushButton("Reset");

    validate_button = new QPushButton("Validate");
    cancel_button   = new QPushButton("Cancel");

    compute_button_layout->addWidget(compute_button);
    compute_button_layout->addWidget(reset_button);

    validate_button_layout->addWidget(validate_button);
    validate_button_layout->addWidget(cancel_button);

    main_layout->addLayout(resolution_layout);
    main_layout->addLayout(compute_button_layout);
    main_layout->addLayout(parameters_layout);
    main_layout->addLayout(validate_button_layout);

    this->setModal(true);
    this->setLayout(main_layout);
    this->setWindowTitle("Calibration Edition");

    QObject::connect(compute_button, &QPushButton::clicked, this, &update_intrinsic_dialog::on_push_compute);
    QObject::connect(reset_button, &QPushButton::clicked, this, &update_intrinsic_dialog::on_push_reset);
    QObject::connect(validate_button, &QPushButton::clicked, this, &update_intrinsic_dialog::on_validate);
    QObject::connect(cancel_button, &QPushButton::clicked, this, &update_intrinsic_dialog::close);
}

//-----------------------------------------------------------------------------

update_intrinsic_dialog::~update_intrinsic_dialog()
= default;

//-----------------------------------------------------------------------------

void update_intrinsic_dialog::set_parameters(std::array<double, 12>& _parameters)
{
    m_calibration        = _parameters;
    m_origin_calibration = _parameters;

    m_ratio = m_calibration[0] / m_calibration[1];

    this->update_infos();
}

//-----------------------------------------------------------------------------

void update_intrinsic_dialog::on_validate()
{
    Q_EMIT new_calibration(m_calibration);

    this->close();
}

//-----------------------------------------------------------------------------

void update_intrinsic_dialog::on_push_compute()
{
    double height = m_height->text().toDouble();
    double width  = m_width->text().toDouble();

    double ratio = width / height;

    //new resolution don't respect the original ratio
    if(std::abs(m_ratio - ratio) > 0.0001)
    {
        sight::ui::dialog::message::sptr warning_mess =
            std::make_shared<sight::ui::dialog::message>();
        warning_mess->show(
            "Warning",
            "The new resolution don't respect the original resolution ratio !"
            ,
            sight::ui::dialog::message::warning
        );
        return;
    }

    //alpha : original resolution / new resolution (!! Ratio should be kept (16/9, 4/3 ...) !!!)

    double alpha = m_origin_calibration[1] / height;

    double fx_new = NAN;
    double fy_new = NAN;
    double cx_new = NAN;
    double cy_new = NAN;
    double k1_new = NAN;
    double k2_new = NAN;
    double p1_new = NAN;
    double p2_new = NAN;

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
    k1_new = m_calibration[6] * (alpha * alpha);
    k2_new = m_calibration[7] * (alpha * alpha * alpha * alpha);
    p1_new = m_calibration[8] * (alpha * alpha);
    p2_new = m_calibration[9] * (alpha * alpha);

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

    this->update_infos();
}

//-----------------------------------------------------------------------------

void update_intrinsic_dialog::on_push_reset()
{
    for(unsigned int i = 0 ; i < m_origin_calibration.size() ; ++i)
    {
        m_calibration[i] = m_origin_calibration[i];
    }

    this->update_infos();
}

//-----------------------------------------------------------------------------

void update_intrinsic_dialog::update_infos()
{
    std::stringstream out;

    out << m_calibration[0];
    m_width->setText(out.str().c_str());
    out.str("");

    out << m_calibration[1];
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

} //namespace sight::module::ui::qt::calibration
