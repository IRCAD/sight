/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQt/layoutManager/CardinalLayoutManager.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QBoxLayout>
#include <QDockWidget>
#include <QGroupBox>
#include <QMainWindow>
#include <QScrollArea>

fwGuiRegisterMacro( ::fwGui::CardinalLayoutManager,
                    ::fwGui::layoutManager::CardinalLayoutManagerBase::REGISTRY_KEY );

namespace fwGui
{

//-----------------------------------------------------------------------------

CardinalLayoutManager::CardinalLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

CardinalLayoutManager::~CardinalLayoutManager()
{
}

//-----------------------------------------------------------------------------

void CardinalLayoutManager::createLayout( ::fwGui::container::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    m_qtWindow = new QMainWindow(  );

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget( m_qtWindow );

    m_parentContainer->setLayout(layout);

    const std::list< ViewInfo>& views = this->getViewsInfo();

    bool hasCentral = false;

    for ( ViewInfo viewInfo : views)
    {
        QWidget* insideWidget;
        QScrollArea* scrollArea;

        if(viewInfo.m_align == CENTER)
        {
            if(viewInfo.m_caption.first)
            {
                QGroupBox* groupbox = new QGroupBox(m_qtWindow);
                groupbox->setTitle(QString::fromStdString(viewInfo.m_caption.second));
                insideWidget = groupbox;
            }
            else
            {
                insideWidget = new QWidget(m_qtWindow);
            }

            QWidget* widget = insideWidget;
            SLM_ASSERT("multiple center views are not managed in Qt version of CardinalLayoutManager",
                       !hasCentral);

            if(viewInfo.m_backgroundColor != "default")
            {
                const QString style = QString::fromStdString(
                    "QWidget { background-color: " + viewInfo.m_backgroundColor + ";}");
                widget->setStyleSheet(style);
            }

            if (viewInfo.m_useScrollBar)
            {
                scrollArea = new QScrollArea(m_qtWindow);
                scrollArea->setWidget(widget);
                scrollArea->setWidgetResizable( true );
                m_qtWindow->setCentralWidget(scrollArea);
            }
            else
            {
                m_qtWindow->setCentralWidget(widget);
            }

            insideWidget->setVisible(viewInfo.m_visible);

            if(!viewInfo.m_toolTip.empty())
            {
                insideWidget->setToolTip(QString::fromStdString(viewInfo.m_toolTip));
            }
            hasCentral = true;
        }
        else
        {
            QDockWidget* dockWidget = new QDockWidget(m_qtWindow);
            insideWidget = new QWidget(dockWidget);
            QDockWidget::DockWidgetFeatures features;

            features = QDockWidget::DockWidgetMovable;

            Qt::DockWidgetArea area;

            if(viewInfo.m_align == RIGHT)
            {
                area = Qt::RightDockWidgetArea;
            }
            else if(viewInfo.m_align == LEFT)
            {
                area = Qt::LeftDockWidgetArea;
            }
            else if(viewInfo.m_align == BOTTOM)
            {
                area = Qt::BottomDockWidgetArea;
            }
            else if(viewInfo.m_align == TOP)
            {
                area = Qt::TopDockWidgetArea;
            }

            m_qtWindow->addDockWidget(area, dockWidget);
            dockWidget->setFeatures(features);

            if(viewInfo.m_caption.first)
            {
                dockWidget->setWindowTitle( QString::fromStdString(viewInfo.m_caption.second) );
                dockWidget->setMinimumSize(0, 0);
            }
            else
            {
                // Remove title bar
                // Ensure widget->sizeHint() doesn't return a -1 size that will trigger a runtime warning:
                // As setTitleBarWidget require a widget that have a valid QWidget::sizeHint()
                // and QWidget::sizeHint() return -1 for widget without a layout...
                QHBoxLayout* layout = new QHBoxLayout;
                layout->setSpacing(0);
                layout->setMargin(0);

                QWidget* widget = new QWidget;
                widget->setLayout(layout);

                dockWidget->setMinimumSize(std::max(viewInfo.m_minSize.first, 0),
                                           std::max(viewInfo.m_minSize.second, 0));
                dockWidget->setTitleBarWidget(widget);
            }

            if (viewInfo.m_useScrollBar)
            {
                scrollArea = new QScrollArea(dockWidget);
                scrollArea->setWidget(insideWidget);
                scrollArea->setWidgetResizable(true);
                dockWidget->setWidget(scrollArea);
            }
            else
            {
                dockWidget->setWidget(insideWidget);
            }

            if(false == viewInfo.m_visible)
            {
                dockWidget->setVisible(false);
            }

            if(!viewInfo.m_toolTip.empty())
            {
                dockWidget->setToolTip(QString::fromStdString(viewInfo.m_toolTip));
            }
        }

        if(!viewInfo.m_isResizable)
        {
            //TODO
        }

        if (viewInfo.m_useScrollBar)
        {
            scrollArea->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
        }
        else
        {
            insideWidget->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
        }

        //TODO
        // - viewInfo.m_position
        // - viewInfo.m_layer
        // - viewInfo.m_row
        // - viewInfo.m_visible

        ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
        subContainer->setQtContainer(insideWidget);
        m_subViews.push_back(subContainer);
    }

    m_qtWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

//-----------------------------------------------------------------------------

void CardinalLayoutManager::destroyLayout()
{
    this->destroySubViews();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace fwGui
