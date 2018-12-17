/************************************************************************
 *
 * Copyright (C) 2017-2018 IRCAD France
 * Copyright (C) 2017-2018 IHU Strasbourg
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

#include "uiTools/editor/SMatrixViewer.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace uiTools
{

namespace editor
{

static const ::fwServices::IService::KeyType s_MATRIX_INPUT = "matrix";

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiTools::editor::SMatrixViewer);

// ------------------------------------------------------------------------------

SMatrixViewer::SMatrixViewer() noexcept :
    m_title("matrix")
{
}

// ------------------------------------------------------------------------------

SMatrixViewer::~SMatrixViewer() noexcept
{
}

// ------------------------------------------------------------------------------

void SMatrixViewer::configuring()
{
    ::fwGui::IGuiContainerSrv::initialize();

    m_title = this->getConfigTree().get<std::string>("title", "matrix");
}

// ------------------------------------------------------------------------------

void SMatrixViewer::starting()
{
    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QPointer< QLabel > description = new QLabel(QString::fromStdString(m_title));

    mainLayout->addWidget(description);

    QGridLayout* gridLayout = new QGridLayout();

    for(unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j = 0; j < 4; ++j)
        {
            QLabel* label = new QLabel("");
            m_matrixLabels.push_back(label);
            gridLayout->addWidget(label, i, j);
        }
    }
    mainLayout->addLayout(gridLayout);

    qtContainer->setLayout(mainLayout);

    this->updateFromMatrix();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::stopping()
{
    this->destroy();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::updating()
{
    this->updateFromMatrix();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::updateFromMatrix()
{
    auto matrix = this->getInput< ::fwData::TransformationMatrix3D >(s_MATRIX_INPUT);
    for(unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j = 0; j < 4; ++j)
        {
            m_matrixLabels[i*4 + j]->setText(QString("%1").arg(matrix->getCoefficient(i, j)));
        }
    }
}

// ------------------------------------------------------------------------------

void SMatrixViewer::clearLabels()
{
    for(unsigned int i = 0; i < 4; ++i)
    {
        for(unsigned int j = 0; j < 4; ++j)
        {
            m_matrixLabels[i*4 + j]->setText(QString(""));
        }
    }
}

// ------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMatrixViewer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MATRIX_INPUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ------------------------------------------------------------------------------

}// namespace editor

} // namespace uiTools
