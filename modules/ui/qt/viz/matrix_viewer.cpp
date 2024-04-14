/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "matrix_viewer.hpp"

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace sight::module::ui::qt::viz
{

// ------------------------------------------------------------------------------

matrix_viewer::matrix_viewer() noexcept :
    m_title("matrix")
{
}

// ------------------------------------------------------------------------------

matrix_viewer::~matrix_viewer() noexcept =
    default;

// ------------------------------------------------------------------------------

void matrix_viewer::configuring()
{
    sight::ui::service::initialize();

    m_title = this->get_config().get<std::string>("title", "matrix");
}

// ------------------------------------------------------------------------------

void matrix_viewer::starting()
{
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* main_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    main_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QPointer<QLabel> description = new QLabel(QString::fromStdString(m_title));

    main_layout->addWidget(description);

    auto* grid_layout = new QGridLayout();

    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            auto* label = new QLabel("");
            m_matrix_labels.push_back(label);
            grid_layout->addWidget(label, i, j);
        }
    }

    main_layout->addLayout(grid_layout);

    qt_container->set_layout(main_layout);

    this->update_from_matrix();
}

// ------------------------------------------------------------------------------

void matrix_viewer::stopping()
{
    this->destroy();
}

// ------------------------------------------------------------------------------

void matrix_viewer::updating()
{
    this->update_from_matrix();
}

// ------------------------------------------------------------------------------

void matrix_viewer::update_from_matrix()
{
    const auto matrix = m_matrix.lock();
    for(unsigned int i = 0 ; i < 4 ; ++i)
    {
        for(unsigned int j = 0 ; j < 4 ; ++j)
        {
            m_matrix_labels[int(i * 4 + j)]->setText(QString("%1").arg((*matrix)(i, j), 0, 'f', 2));
        }
    }
}

// ------------------------------------------------------------------------------

void matrix_viewer::clear_labels()
{
    for(int i = 0 ; i < 4 ; ++i)
    {
        for(int j = 0 ; j < 4 ; ++j)
        {
            m_matrix_labels[i * 4 + j]->setText(QString(""));
        }
    }
}

// ------------------------------------------------------------------------------

service::connections_t matrix_viewer::auto_connections() const
{
    connections_t connections;
    connections.push(MATRIX, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

// ------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::viz
