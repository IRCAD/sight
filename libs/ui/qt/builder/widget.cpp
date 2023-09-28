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

void widget::createContainer(ui::container::widget::sptr parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    ui::qt::container::widget::sptr qtContainer = ui::qt::container::widget::make();
    auto* widget                                = new QWidget();
    qtContainer->setQtContainer(widget);

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget);

    m_parent->setLayout(layout);

    m_container = qtContainer;
}

//-----------------------------------------------------------------------------

void widget::destroyContainer()
{
    SIGHT_ASSERT("The Container is not initialized", m_container);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    m_container->destroyContainer();
}

//-----------------------------------------------------------------------------

void widget::setParent(ui::container::widget::sptr parent)
{
    SIGHT_ASSERT("The widget is not yet initialized, cleaning is thus impossible", m_container);
    ui::qt::container::widget::sptr parentContainer = std::dynamic_pointer_cast<ui::qt::container::widget>(
        parent
    );
    SIGHT_ASSERT("dynamicCast widget to widget failed", parentContainer);
    ui::qt::container::widget::sptr container = std::dynamic_pointer_cast<ui::qt::container::widget>(
        m_container
    );
    SIGHT_ASSERT("dynamicCast widget to widget failed", container);

    QWidget* qtContainer = container->getQtContainer();
    SIGHT_ASSERT("The widget is not yet initialized", qtContainer);
    QWidget* qtParent = parentContainer->getQtContainer();
    SIGHT_ASSERT("The parent's widget is not yet initialized", qtParent);

    if(qtParent != m_parent->getQtContainer())
    {
        if(qtParent->layout() == nullptr)
        {
            auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
            layout->setContentsMargins(0, 0, 0, 0);
            qtParent->setLayout(layout);
        }

        qtParent->layout()->addWidget(qtContainer);
        qtContainer->update();

        m_parent = parentContainer;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
