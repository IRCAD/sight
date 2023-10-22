/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "image.hpp"

#include <core/runtime/path.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>

namespace sight::module::ui::qt::image
{

//-----------------------------------------------------------------------------

image::image() noexcept =
    default;

//-----------------------------------------------------------------------------

image::~image() noexcept =
    default;

//-----------------------------------------------------------------------------

void image::starting()
{
    this->create();

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* const layout = new QVBoxLayout();
    auto* const label  = new QLabel("");

    auto* const pixmap = new QPixmap();

    pixmap->load(m_path.string().c_str());

    if(pixmap->isNull())
    {
        SIGHT_ERROR("image could not be found: " << m_path);
    }

    if(m_width != -1 && m_height != -1)
    {
        label->setPixmap(pixmap->scaled(m_width, m_height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else if(m_width != -1)
    {
        label->setPixmap((pixmap->scaledToWidth(m_width, Qt::SmoothTransformation)));
    }
    else if(m_height != -1)
    {
        label->setPixmap((pixmap->scaledToHeight(m_height, Qt::SmoothTransformation)));
    }
    else
    {
        label->setPixmap(*pixmap);
    }

    layout->addWidget(label);
    qt_container->setLayout(layout);
}

//-----------------------------------------------------------------------------

void image::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void image::configuring()
{
    this->initialize();

    const config_t cfg = this->get_config();
    auto path_cfg      = cfg.get<std::string>("path");

    m_path = core::runtime::get_module_resource_file_path(path_cfg);

    m_width  = cfg.get<int>("width", m_width);
    m_height = cfg.get<int>("height", m_height);
}

//-----------------------------------------------------------------------------

void image::updating()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::image
