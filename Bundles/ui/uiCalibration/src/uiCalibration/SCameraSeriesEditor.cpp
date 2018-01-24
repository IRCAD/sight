/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SCameraSeriesEditor.hpp"

#include <arData/CameraSeries.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/Object.hpp>

#include <QBoxLayout>
#include <QGridLayout>

#include <algorithm>
#include <sstream>

namespace uiCalibration
{
fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiCalibration::SCameraSeriesEditor, ::arData::CameraSeries);

const ::fwCom::Slots::SlotKeyType SCameraSeriesEditor::s_UPDATE_INFOS_SLOT = "updateInfos";
// -------------------------------------------------------------------------

SCameraSeriesEditor::SCameraSeriesEditor() noexcept :
    m_camIndex(1)
{
    newSlot(s_UPDATE_INFOS_SLOT, &SCameraSeriesEditor::updateInformations, this);
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::configuring()
{
    fwGui::IGuiContainerSrv::initialize();

    ::fwServices::IService::ConfigType config = this->getConfigTree();
    m_camIndex                                = config.get<size_t>("index", 1);
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::starting()
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_description = new QLabel("");

    mainLayout->addWidget(m_description);

    QGridLayout* gridLayout = new QGridLayout();

    for (std::uint8_t i = 0; i < 4; ++i)
    {
        for (std::uint8_t j = 0; j < 4; ++j)
        {
            QLabel* label = new QLabel("");
            m_matrixLabels.push_back(label);
            gridLayout->addWidget(label, i, j);
        }
    }

    mainLayout->addLayout(gridLayout);

    qtContainer->setLayout(mainLayout);

    this->updateInformations();

}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::stopping()
{
    this->destroy();
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::swapping()
{
    updateInformations();
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::updateInformations()
{
    ::arData::CameraSeries::csptr cameraSeries = this->getInput< ::arData::CameraSeries >("cameraSeries");

    //IS CALIBRATED
    ::fwData::TransformationMatrix3D::csptr matrix = cameraSeries->getExtrinsicMatrix(m_camIndex);
    if(matrix)
    {
        m_description->setText("<b>The cameras are calibrated.</b>");
    }
    else
    {
        m_description->setText("<b>The cameras are not calibrated.</b>");
        this->clearLabels();
        return;
    }

    for (std::uint8_t i = 0; i < 4; ++i)
    {
        for (std::uint8_t j = 0; j < 4; ++j)
        {
            m_matrixLabels[i*4 + j]->setText(QString("%1").arg(matrix->getCoefficient(i, j)));
        }
    }
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::clearLabels()
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            m_matrixLabels[i*4 + j]->setText(QString(""));
        }
    }
}

// ----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SCameraSeriesEditor::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "cameraSeries", ::arData::CameraSeries::s_ADDED_CAMERA_SIG, s_UPDATE_INFOS_SLOT  );
    connections.push( "cameraSeries", ::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG, s_UPDATE_INFOS_SLOT  );
    connections.push( "cameraSeries", ::arData::CameraSeries::s_REMOVED_CAMERA_SIG, s_UPDATE_INFOS_SLOT  );
    return connections;
}

// ----------------------------------------------------------------------------

} // uiCalibration
