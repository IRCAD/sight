/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "SMatrixViewer.hpp"

#include <service/macros.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace sight::module::ui::qt::viz
{

// ------------------------------------------------------------------------------

SMatrixViewer::SMatrixViewer() noexcept :
    m_title("matrix")
{
}

// ------------------------------------------------------------------------------

SMatrixViewer::~SMatrixViewer() noexcept =
    default;

// ------------------------------------------------------------------------------

void SMatrixViewer::configuring()
{
    sight::ui::base::IGuiContainer::initialize();

    m_title = this->getConfiguration().get<std::string>("title", "matrix");
}

// ------------------------------------------------------------------------------

void SMatrixViewer::starting()
{
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    auto* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QPointer<QLabel> description = new QLabel(QString::fromStdString(m_title));

    mainLayout->addWidget(description);

    auto* gridLayout = new QGridLayout();

    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            auto* label = new QLabel("");
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
    const auto matrix = m_matrix.lock();
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            m_matrixLabels[int(i * 4 + j)]->setText(QString("%1").arg((*matrix)(i, j), 0, 'f', 2));
        }
    }
}

// ------------------------------------------------------------------------------

void SMatrixViewer::clearLabels()
{
    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            m_matrixLabels[i * 4 + j]->setText(QString(""));
        }
    }
}

// ------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SMatrixViewer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_MATRIX, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
