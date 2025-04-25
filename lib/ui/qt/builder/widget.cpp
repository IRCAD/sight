/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ui/qt/builder/widget.hpp"
#include "ui/qt/container/widget.hpp"

#include <ui/__/macros.hpp>

#include <QMainWindow>
#include <QVBoxLayout>
#include <QWidget>

SIGHT_REGISTER_GUI(sight::ui::qt::builder::widget, sight::ui::builder::widget::REGISTRY_KEY);

namespace sight::ui::qt::builder
{

//-----------------------------------------------------------------------------

widget::~widget()
= default;

//-----------------------------------------------------------------------------

void widget::create_container(ui::container::widget::sptr _parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    ui::qt::container::widget::sptr qt_container = ui::qt::container::widget::make();
    auto* widget                                 = new QWidget();
    qt_container->set_qt_container(widget);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget);

    m_parent->set_layout(layout);

    m_container = qt_container;
}

//-----------------------------------------------------------------------------

void widget::destroy_container()
{
    SIGHT_ASSERT("The Container is not initialized", m_container);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    m_container->destroy_container();
}

//-----------------------------------------------------------------------------

void widget::set_parent(ui::container::widget::sptr _parent)
{
    SIGHT_ASSERT("The widget is not yet initialized, cleaning is thus impossible", m_container);
    ui::qt::container::widget::sptr parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(
        _parent
    );
    SIGHT_ASSERT("dynamicCast widget to widget failed", parent_container);
    ui::qt::container::widget::sptr container = std::dynamic_pointer_cast<ui::qt::container::widget>(
        m_container
    );
    SIGHT_ASSERT("dynamicCast widget to widget failed", container);

    QWidget* qt_container = container->get_qt_container();
    SIGHT_ASSERT("The widget is not yet initialized", qt_container);
    QWidget* qt_parent = parent_container->get_qt_container();
    SIGHT_ASSERT("The parent's widget is not yet initialized", qt_parent);

    if(qt_parent != m_parent->get_qt_container())
    {
        if(qt_parent->layout() == nullptr)
        {
            auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
            layout->setContentsMargins(0, 0, 0, 0);
            qt_parent->setLayout(layout);
        }

        qt_parent->layout()->addWidget(qt_container);
        qt_container->update();

        m_parent = parent_container;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
