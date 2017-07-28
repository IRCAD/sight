/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SCameraInformationEditor.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QBoxLayout>
#include <QGridLayout>

#include <algorithm>
#include <sstream>

namespace uiCalibration
{
fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SCameraInformationEditor, ::arData::Camera);
const ::fwCom::Slots::SlotKeyType SCameraInformationEditor::s_UPDATE_INFOS_SLOT = "updateInfos";
// -------------------------------------------------------------------------

SCameraInformationEditor::SCameraInformationEditor() noexcept
{
    newSlot(s_UPDATE_INFOS_SLOT, &SCameraInformationEditor::updateInformations, this);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::configuring()
{
    fwGui::IGuiContainerSrv::initialize();
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::starting()
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QGridLayout* gridLayout = new QGridLayout();
    QLabel* desc            = new QLabel("description: ");
    m_description = new QLabel();
    gridLayout->addWidget(desc, 0, 0);
    gridLayout->addWidget(m_description, 0, 1);

    QBoxLayout* titleLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_isCalibrated = new QLabel();
    titleLayout->addWidget(m_isCalibrated);

    QGridLayout* infoLayout = new QGridLayout;
    infoLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(infoLayout);

    m_width  = new QLabel();
    m_height = new QLabel();
    m_cx     = new QLabel();
    m_cy     = new QLabel();
    m_fx     = new QLabel();
    m_fy     = new QLabel();
    m_k1     = new QLabel();
    m_k2     = new QLabel();
    m_p1     = new QLabel();
    m_p2     = new QLabel();
    m_k3     = new QLabel();
    m_skew   = new QLabel();

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

void SCameraInformationEditor::swapping()
{
    updateInformations();
}

// -------------------------------------------------------------------------

void SCameraInformationEditor::updateInformations()
{
    ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >("camera");
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

    const ::arData::Camera::DistArrayType& dist = camera->getDistortionCoefficient();

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

::fwServices::IService::KeyConnectionsMap SCameraInformationEditor::getAutoConnections() const
{

    KeyConnectionsMap connections;

    connections.push( "camera", ::arData::Camera::s_ID_MODIFIED_SIG, s_UPDATE_INFOS_SLOT );
    connections.push( "camera", ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_UPDATE_INFOS_SLOT );

    return connections;
}

} // uiCalibration
