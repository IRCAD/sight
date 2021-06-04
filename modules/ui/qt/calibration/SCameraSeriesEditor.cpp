/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "modules/ui/qt/calibration/SCameraSeriesEditor.hpp"

#include <core/base.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/Worker.hpp>
#include <core/tools/Object.hpp>

#include <data/CameraSeries.hpp>
#include <data/Matrix4.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QBoxLayout>
#include <QGridLayout>

#include <algorithm>
#include <sstream>

namespace sight::module::ui::qt::calibration
{

const core::com::Slots::SlotKeyType SCameraSeriesEditor::s_UPDATE_INFOS_SLOT = "updateInfos";
// -------------------------------------------------------------------------

SCameraSeriesEditor::SCameraSeriesEditor() noexcept :
    m_camIndex(1)
{
    newSlot(s_UPDATE_INFOS_SLOT, &SCameraSeriesEditor::updateInformations, this);
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::configuring()
{
    sight::ui::base::IGuiContainer::initialize();

    service::IService::ConfigType config = this->getConfigTree();
    m_camIndex = config.get<size_t>("index", 1);
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::starting()
{
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_description = new QLabel("");

    mainLayout->addWidget(m_description);

    QGridLayout* gridLayout = new QGridLayout();

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
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
    data::CameraSeries::csptr cameraSeries = this->getInput<data::CameraSeries>("cameraSeries");

    //IS CALIBRATED
    data::Matrix4::csptr matrix = cameraSeries->getExtrinsicMatrix(m_camIndex);
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

    for(std::uint8_t i = 0 ; i < 4 ; ++i)
    {
        for(std::uint8_t j = 0 ; j < 4 ; ++j)
        {
            m_matrixLabels[i * 4 + j]->setText(QString("%1").arg(matrix->getCoefficient(i, j)));
        }
    }
}

// -------------------------------------------------------------------------

void SCameraSeriesEditor::clearLabels()
{
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            m_matrixLabels[i * 4 + j]->setText(QString(""));
        }
    }
}

// ----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SCameraSeriesEditor::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push("cameraSeries", data::CameraSeries::s_ADDED_CAMERA_SIG, s_UPDATE_INFOS_SLOT);
    connections.push("cameraSeries", data::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG, s_UPDATE_INFOS_SLOT);
    connections.push("cameraSeries", data::CameraSeries::s_REMOVED_CAMERA_SIG, s_UPDATE_INFOS_SLOT);
    return connections;
}

// ----------------------------------------------------------------------------

} // uiCalibration
