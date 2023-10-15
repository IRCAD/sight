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

#include "ui/qt/builder/toolbar.hpp"
#include "ui/qt/container/toolbar.hpp"

#include "ui/qt/App.hpp"
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

void toolbar::createToolBar(ui::container::widget::sptr parent)
{
    m_parent = std::dynamic_pointer_cast<ui::qt::container::widget>(parent);
    SIGHT_ASSERT("The parent container is not a widget", m_parent);
    auto* window = qobject_cast<QMainWindow*>(m_parent->getQtContainer());

    auto* toolBar = new QToolBar(QObject::tr("ToolBar"));
    if(m_toolBitmapSize.first != -1)
    {
        toolBar->setIconSize(QSize(m_toolBitmapSize.first, m_toolBitmapSize.second));
    }

    toolBar->setFloatable(false);

    QString style;
    if(!m_backgroundColor.empty())
    {
        std::array<std::uint8_t, 4> rgba {};
        data::tools::color::hexaStringToRGBA(m_backgroundColor, rgba);
        std::stringstream ss;
        ss << "QToolBar { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
        << static_cast<std::int16_t>(rgba[1]) << ','
        << static_cast<std::int16_t>(rgba[2]) << ','
        << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
        style = QString::fromStdString(ss.str());
    }

    if(m_spacing != 0)
    {
        switch(m_alignment)
        {
            case TOP:
            case BOTTOM:
                style += QString("QToolButton{ padding-left: %1px; padding-right: %1px; }").arg(m_spacing);
                break;

            case RIGHT:
            default: // LEFT
                style += QString("QToolButton{ padding-top: %1px; padding-bottom: %1px; }").arg(m_spacing);
                break;
        }
    }

    toolBar->setStyleSheet(qApp->styleSheet() + style);

    ui::qt::container::toolbar::sptr toolBarContainer = ui::qt::container::toolbar::make();
    if(window != nullptr)
    {
        bool visible = window->isVisible();

        Qt::ToolBarArea area {Qt::NoToolBarArea};
        switch(m_alignment)
        {
            case TOP:
                area = Qt::TopToolBarArea;
                break;

            case BOTTOM:
                area = Qt::BottomToolBarArea;
                break;

            case RIGHT:
                area = Qt::RightToolBarArea;
                break;

            default: // LEFT
                area = Qt::LeftToolBarArea;
                break;
        }

        window->addToolBar(area, toolBar);

        //on Os X, the window is hidden (???)
        window->setVisible(visible);
    }
    else // parent is not a QMainWindow
    {
        QWidget* widget = m_parent->getQtContainer();
        SIGHT_ASSERT("Parent container must have a layout", widget->layout());
        auto* layout = qobject_cast<QBoxLayout*>(widget->layout());

        switch(m_alignment)
        {
            case TOP:
                layout->setDirection(QBoxLayout::TopToBottom);
                toolBar->setOrientation(Qt::Horizontal);
                break;

            case BOTTOM:
                layout->setDirection(QBoxLayout::BottomToTop);
                toolBar->setOrientation(Qt::Horizontal);
                break;

            case RIGHT:
                layout->setDirection(QBoxLayout::RightToLeft);
                toolBar->setOrientation(Qt::Vertical);
                break;

            case LEFT:
                layout->setDirection(QBoxLayout::LeftToRight);
                toolBar->setOrientation(Qt::Vertical);
                break;
        }

        SIGHT_ASSERT("Parent container layout must have be a QVBoxLayout", layout);
        layout->setSpacing(0);
        layout->insertWidget(0, toolBar, 0);
    }

    toolBarContainer->setQtToolBar(toolBar);
    m_toolBar = toolBarContainer;
}

//-----------------------------------------------------------------------------

void toolbar::destroyToolBar()
{
    SIGHT_ASSERT("The ToolBar is not initialized", m_toolBar);
    SIGHT_ASSERT("The parent's container is not a widget", m_parent);
    auto* window = qobject_cast<QMainWindow*>(m_parent->getQtContainer());

    if(window != nullptr)
    {
        ui::qt::container::toolbar::sptr qtToolBar =
            std::dynamic_pointer_cast<ui::qt::container::toolbar>(m_toolBar);
        QToolBar* toolbar = qtToolBar->getQtToolBar();
        window->removeToolBar(toolbar);
    }

    m_toolBar->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::builder
