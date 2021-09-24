/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/qt/builder/ToolBarBuilder.hpp"

#include "ui/qt/App.hpp"
#include "ui/qt/container/QtContainer.hpp"
#include "ui/qt/container/QtToolBarContainer.hpp"

#include <data/tools/Color.hpp>

#include <ui/base/registry/macros.hpp>

#include <QHBoxLayout>
#include <QMainWindow>
#include <QToolBar>

fwGuiRegisterMacro(
    sight::ui::base::builder::ToolBarBuilder,
    sight::ui::base::builder::IToolBarBuilder::REGISTRY_KEY
);

namespace sight::ui::base
{

namespace builder
{

//-----------------------------------------------------------------------------

ToolBarBuilder::ToolBarBuilder(ui::base::GuiBaseObject::Key)
{
}

//-----------------------------------------------------------------------------

ToolBarBuilder::~ToolBarBuilder()
{
}

//-----------------------------------------------------------------------------

void ToolBarBuilder::createToolBar(ui::base::container::fwContainer::sptr parent)
{
    m_parent = ui::qt::container::QtContainer::dynamicCast(parent);
    SIGHT_ASSERT("The parent container is not a QtContainer", m_parent);
    QMainWindow* window = qobject_cast<QMainWindow*>(m_parent->getQtContainer());

    QToolBar* toolBar = new QToolBar(QObject::tr("ToolBar"));
    toolBar->setIconSize(QSize(m_toolBitmapSize.first, m_toolBitmapSize.second));
    toolBar->setFloatable(false);

    if(!m_backgroundColor.empty())
    {
        std::uint8_t rgba[4];
        data::tools::Color::hexaStringToRGBA(m_backgroundColor, rgba);
        std::stringstream ss;
        ss << "QToolBar { background-color: rgba(" << static_cast<short>(rgba[0]) << ','
        << static_cast<short>(rgba[1]) << ','
        << static_cast<short>(rgba[2]) << ','
        << (static_cast<float>(rgba[3]) / 255.f) * 100 << "%); } ";
        const QString style = QString::fromStdString(ss.str());
        toolBar->setStyleSheet(qApp->styleSheet() + style);
    }

    ui::qt::container::QtToolBarContainer::sptr toolBarContainer = ui::qt::container::QtToolBarContainer::New();
    if(window)
    {
        bool visible = window->isVisible();

        Qt::ToolBarArea area;
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

            case LEFT:
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
        QBoxLayout* layout = qobject_cast<QBoxLayout*>(widget->layout());
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
        layout->insertWidget(0, toolBar, 0);
    }

    toolBarContainer->setQtToolBar(toolBar);
    m_toolBar = toolBarContainer;
}

//-----------------------------------------------------------------------------

void ToolBarBuilder::destroyToolBar()
{
    SIGHT_ASSERT("The ToolBar is not initialized", m_toolBar);
    SIGHT_ASSERT("The parent's container is not a QtContainer", m_parent);
    QMainWindow* window = qobject_cast<QMainWindow*>(m_parent->getQtContainer());

    if(window)
    {
        ui::qt::container::QtToolBarContainer::sptr qtToolBar =
            ui::qt::container::QtToolBarContainer::dynamicCast(m_toolBar);
        QToolBar* toolbar = qtToolBar->getQtToolBar();
        window->removeToolBar(toolbar);
    }

    m_toolBar->destroyContainer();
}

//-----------------------------------------------------------------------------

} // namespace builder

} // namespace sight::ui::base
