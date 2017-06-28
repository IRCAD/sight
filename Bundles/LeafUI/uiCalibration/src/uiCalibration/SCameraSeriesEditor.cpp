/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
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
fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiCalibration::SCameraSeriesEditor, ::arData::CameraSeries);

const ::fwCom::Slots::SlotKeyType SCameraSeriesEditor::s_UPDATE_INFOS_SLOT = "updateInfos";
// -------------------------------------------------------------------------

SCameraSeriesEditor::SCameraSeriesEditor() throw () :
    m_camIndex(1)
{
    m_slotUpdateInfos = ::fwCom::newSlot(&SCameraSeriesEditor::updateInformations, this);
    ::fwCom::HasSlots::m_slots(s_UPDATE_INFOS_SLOT, m_slotUpdateInfos);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::configuring() throw (fwTools::Failed)
{
    fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr cfgIdx = m_configuration->findConfigurationElement("camIndex");
    if(cfgIdx)
    {
        std::string idxStr = cfgIdx->getValue();
        SLM_ASSERT("'camIndex' is empty.", !idxStr.empty());
        m_camIndex = std::stoul(idxStr);
    }
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::starting() throw (fwTools::Failed)
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_description = new QLabel("");

    mainLayout->addWidget(m_description);

    QGridLayout* gridLayout = new QGridLayout();

    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
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

void SCameraSeriesEditor::stopping() throw (fwTools::Failed)
{
    this->destroy();
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::swapping() throw (::fwTools::Failed)
{
    updateInformations();
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::updateInformations()
{
    ::arData::CameraSeries::sptr cameraSeries = this->getObject< ::arData::CameraSeries >();

    //IS CALIBRATED
    ::fwData::TransformationMatrix3D::sptr matrix = cameraSeries->getExtrinsicMatrix(m_camIndex);
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

    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
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

::fwServices::IService::KeyConnectionsType SCameraSeriesEditor::getObjSrvConnections() const
{
    ::fwServices::IService::KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::arData::CameraSeries::s_ADDED_CAMERA_SIG, s_UPDATE_INFOS_SLOT ) );
    connections.push_back( std::make_pair( ::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG, s_UPDATE_INFOS_SLOT ) );
    connections.push_back( std::make_pair( ::arData::CameraSeries::s_REMOVED_CAMERA_SIG, s_UPDATE_INFOS_SLOT ) );
    return connections;
}

} // uiCalibration
