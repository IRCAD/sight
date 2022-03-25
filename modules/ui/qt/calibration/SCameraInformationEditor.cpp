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

#include "modules/ui/qt/calibration/SCameraInformationEditor.hpp"

#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QBoxLayout>
#include <QGridLayout>

#include <sstream>

namespace sight::module::ui::qt::calibration
{

// -------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SCameraInformationEditor::s_UPDATE_INFOS_SLOT = "updateInfos";

// -------------------------------------------------------------------------

SCameraInformationEditor::SCameraInformationEditor() noexcept
{
    newSlot(s_UPDATE_INFOS_SLOT, &SCameraInformationEditor::updateInformations, this);
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::configuring()
{
    sight::ui::base::IGuiContainer::initialize();
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::starting()
{
    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());
    qtContainer->getQtContainer()->setObjectName(serviceID);

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QGridLayout* gridLayout = new QGridLayout();
    QLabel* desc            = new QLabel("description: ");
    m_description = new QLabel();
    m_description->setObjectName(serviceID + "/description");
    gridLayout->addWidget(desc, 0, 0);
    gridLayout->addWidget(m_description, 0, 1);

    QBoxLayout* titleLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_isCalibrated = new QLabel();
    m_isCalibrated->setObjectName(serviceID + "/isCalibrated");
    titleLayout->addWidget(m_isCalibrated);

    QGridLayout* infoLayout = new QGridLayout;
    infoLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(infoLayout);

    m_width = new QLabel();
    m_width->setObjectName(serviceID + "/width");
    m_height = new QLabel();
    m_height->setObjectName(serviceID + "/height");
    m_cx = new QLabel();
    m_cx->setObjectName(serviceID + "/cx");
    m_cy = new QLabel();
    m_cy->setObjectName(serviceID + "/cy");
    m_fx = new QLabel();
    m_fx->setObjectName(serviceID + "/fx");
    m_fy = new QLabel();
    m_fy->setObjectName(serviceID + "/fy");
    m_k1 = new QLabel();
    m_k1->setObjectName(serviceID + "/k1");
    m_k2 = new QLabel();
    m_k2->setObjectName(serviceID + "/k2");
    m_p1 = new QLabel();
    m_p1->setObjectName(serviceID + "/p1");
    m_p2 = new QLabel();
    m_p2->setObjectName(serviceID + "/p2");
    m_k3 = new QLabel();
    m_k3->setObjectName(serviceID + "/k3");
    m_skew = new QLabel();
    m_skew->setObjectName(serviceID + "/skew");

    infoLayout->addWidget(m_width, 0, 0);
    infoLayout->addWidget(m_height, 0, 1);
    infoLayout->addWidget(m_skew, 0, 2);

    infoLayout->addWidget(m_cx, 1, 0);
    infoLayout->addWidget(m_cy, 1, 1);
    infoLayout->addWidget(m_fx, 1, 2);
    infoLayout->addWidget(m_fy, 1, 3);

    infoLayout->addWidget(m_k1, 2, 0);
    infoLayout->addWidget(m_k2, 2, 1);
    infoLayout->addWidget(m_p1, 2, 2);
    infoLayout->addWidget(m_p2, 2, 3);
    infoLayout->addWidget(m_k3, 2, 4);

    qtContainer->setLayout(mainLayout);

    updateInformations();
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::stopping()
{
    this->destroy();
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::updateInformations()
{
    const auto camera = m_camera.lock();
    std::stringstream out;

    m_description->setText(QString::fromStdString(camera->getDescription()));

    //IS CALIBRATED
    if(camera->getIsCalibrated())
    {
        m_isCalibrated->setText("<b>The camera is calibrated.</b>");
    }
    else
    {
        m_isCalibrated->setText("<b>The camera is not calibrated.</b>");
        this->clearLabels();
        return;
    }

    //Height
    out << "Height: <font color='#0066CC'>" << camera->getHeight() << "</font>";
    m_height->setText(out.str().c_str());

    out.str("");

    //Width
    out << "Width: <font color='#0066CC'>" << camera->getWidth() << "</font>";
    m_width->setText(out.str().c_str());

    out.str("");
    //CX
    out << "Cx: <font color='#0066CC'>" << camera->getCx() << "</font>";
    m_cx->setText(out.str().c_str());

    out.str("");

    //CY
    out << "Cy: <font color='#0066CC'>" << camera->getCy() << "</font>";
    m_cy->setText(out.str().c_str());

    out.str("");

    //FX
    out << "Fx: <font color='#0066CC'>" << camera->getFx() << "</font>";
    m_fx->setText(out.str().c_str());

    out.str("");

    //FY
    out << "Fy: <font color='#0066CC'>" << camera->getFy() << "</font>";
    m_fy->setText(out.str().c_str());

    const data::Camera::DistArrayType& dist = camera->getDistortionCoefficient();

    out.str("");

    //K1
    out << "K1: <font color='#0066CC'>" << dist[0] << "</font>";
    m_k1->setText(out.str().c_str());

    out.str("");

    //K2
    out << "K2: <font color='#0066CC'>" << dist[1] << "</font>";
    m_k2->setText(out.str().c_str());

    out.str("");

    //P1
    out << "P1: <font color='#0066CC'>" << dist[2] << "</font>";
    m_p1->setText(out.str().c_str());

    out.str("");

    //P2
    out << "P2: <font color='#0066CC'>" << dist[3] << "</font>";
    m_p2->setText(out.str().c_str());

    out.str("");

    //K3
    out << "K3: <font color='#0066CC'>" << dist[4] << "</font>";
    m_k3->setText(out.str().c_str());

    out.str("");

    //SKEW
    out << "Skew: <font color='#0066CC'>" << camera->getSkew() << "</font>";
    m_skew->setText(out.str().c_str());
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::clearLabels()
{
    m_width->setText("");
    m_height->setText("");
    m_cx->setText("");
    m_cy->setText("");
    m_fx->setText("");
    m_fy->setText("");
    m_k1->setText("");
    m_k2->setText("");
    m_p1->setText("");
    m_p2->setText("");
    m_k3->setText("");
    m_skew->setText("");
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SCameraInformationEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_CAMERA, data::Camera::s_ID_MODIFIED_SIG, s_UPDATE_INFOS_SLOT);
    connections.push(s_CAMERA, data::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_INFOS_SLOT);

    return connections;
}

} // uiCalibration
