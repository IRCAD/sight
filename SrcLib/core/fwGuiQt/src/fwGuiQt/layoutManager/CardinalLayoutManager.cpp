/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

