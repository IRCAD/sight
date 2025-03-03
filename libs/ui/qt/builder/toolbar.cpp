/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "ui/qt/builder/toolbar.hpp"
#include "ui/qt/container/toolbar.hpp"

#include "ui/qt/app.hpp"
#include "ui/qt/container/widget.hpp"

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>

#include <QHBoxLayout>
#include <QMainWindow>
#include <QToolBar>

SIGHT_REGISTER_GUI(sight::ui::qt::builder::toolbar, sight::ui::builder::toolbar::REGISTRY_KEY);

namespace sight::ui::qt::builder
{

//-----------------------------------------------------------------------------

void toolbar::create_tool_bar(ui::container::widget::sptr _parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);

    auto* const parent_widget = m_parent->get_qt_container();
    SIGHT_ASSERT("The parent have no QWidget", parent_widget != nullptr);

    auto* tool_bar = new QToolBar(parent_widget);

    if(m_tool_bitmap_size.first != -1)
    {
        tool_bar->setIconSize(QSize(m_tool_bitmap_size.first, m_tool_bitmap_size.second));
    }

    tool_bar->setFloatable(false);

    QString style;
    if(!m_background_color.empty())
    {
        std::array<std::uint8_t, 4> rgba {};
        data::tools::color::hexa_string_to_rgba(m_background_color, rgba);
        std::stringstream ss;
        ss << "QToolBar { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
        << static_cast<std::int16_t>(rgba[1]) << ','
        << static_cast<std::int16_t>(rgba[2]) << ','
        << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
        style = QString::fromStdString(ss.str());
    }

    if(!m_qss_class.empty())
    {
        tool_bar->setProperty("class", QString::fromStdString(m_qss_class));
    }

    if(m_spacing != 0)
    {
        switch(m_alignment)
        {
            case top:
            case bottom:
                style += QString("QToolButton{ padding-left: %1px; padding-right: %1px; }").arg(m_spacing);
                break;

            case right:
            default: // LEFT
                style += QString("QToolButton{ padding-top: %1px; padding-bottom: %1px; }").arg(m_spacing);
                break;
        }
    }

    tool_bar->setStyleSheet(qApp->styleSheet() + style);

    ui::qt::container::toolbar::sptr tool_bar_container = ui::qt::container::toolbar::make();
    if(auto* const window = qobject_cast<QMainWindow*>(m_parent->get_qt_container()); window != nullptr)
    {
        bool visible = window->isVisible();

        Qt::ToolBarArea area {Qt::NoToolBarArea};
        switch(m_alignment)
        {
            case top:
                area = Qt::TopToolBarArea;
                break;

            case bottom:
                area = Qt::BottomToolBarArea;
                break;

            case right:
                area = Qt::RightToolBarArea;
                break;

            default: // LEFT
                area = Qt::LeftToolBarArea;
                break;
        }

        window->addToolBar(area, tool_bar);

        //on Os X, the window is hidden (???)
        window->setVisible(visible);
    }
    else // parent is not a QMainWindow
    {
        QWidget* const widget = m_parent->get_qt_container();

        auto* const layout = qobject_cast<QBoxLayout*>(widget->layout());
        SIGHT_ASSERT("Parent container layout must be a QBoxLayout", layout);

        switch(m_alignment)
        {
            case top:
                layout->setDirection(QBoxLayout::TopToBottom);
                tool_bar->setOrientation(Qt::Horizontal);
                break;

            case bottom:
                layout->setDirection(QBoxLayout::BottomToTop);
                tool_bar->setOrientation(Qt::Horizontal);
                break;

            case right:
                layout->setDirection(QBoxLayout::RightToLeft);
                tool_bar->setOrientation(Qt::Vertical);
                break;

            case left:
                layout->setDirection(QBoxLayout::LeftToRight);
                tool_bar->setOrientation(Qt::Vertical);
                break;
        }

        layout->setSpacing(0);
        layout->insertWidget(0, tool_bar, 0);
    }

    tool_bar_container->set_qt_tool_bar(tool_bar);
    m_tool_bar = tool_bar_container;
}

//-----------------------------------------------------------------------------

void toolbar::destroy_tool_bar()
{
    SIGHT_ASSERT("The tool_bar is not initialized", m_tool_bar);
    SIGHT_ASSERT("The parent's container is not a widget", m_parent);
    auto* window = qobject_cast<QMainWindow*>(m_parent->get_qt_container());

    if(window != nullptr)
    {
        ui::qt::container::toolbar::sptr qt_tool_bar =
            std::dynamic_pointer_cast<ui::qt::container::toolbar>(m_tool_bar);
        QToolBar* toolbar = qt_tool_bar->get_qt_tool_bar();
        window->removeToolBar(toolbar);
    }

    m_tool_bar->destroy_container();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
