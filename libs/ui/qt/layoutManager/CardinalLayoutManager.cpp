/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/qt/layoutManager/CardinalLayoutManager.hpp"

#include "ui/qt/App.hpp"

#include <core/base.hpp>

#include <data/tools/Color.hpp>

#include <ui/base/registry/macros.hpp>

#include <QBoxLayout>
#include <QDockWidget>
#include <QGroupBox>
#include <QMainWindow>
#include <QScrollArea>

// cspell: ignore QWIDGETSIZE_MAX

SIGHT_REGISTER_GUI(
    sight::ui::qt::CardinalLayoutManager,
    sight::ui::base::layoutManager::CardinalLayoutManagerBase::REGISTRY_KEY
);

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

CardinalLayoutManager::CardinalLayoutManager(ui::base::GuiBaseObject::Key /*key*/)
{
}

//-----------------------------------------------------------------------------

CardinalLayoutManager::~CardinalLayoutManager()
= default;

//-----------------------------------------------------------------------------

void CardinalLayoutManager::createLayout(ui::base::container::fwContainer::sptr parent, const std::string& id)
{
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);
    const QString qId = QString::fromStdString(id);
    m_parentContainer->getQtContainer()->setObjectName(qId);

    m_qtWindow = new QMainWindow();

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_qtWindow);

    m_parentContainer->setLayout(layout);

    const std::list<ViewInfo>& views = this->getViewsInfo();

    [[maybe_unused]] bool hasCentral = false;

    for(std::size_t idx = 0 ; const ViewInfo& viewInfo : views)
    {
        QWidget* insideWidget   = nullptr;
        QScrollArea* scrollArea = nullptr;

        if(viewInfo.m_align == CENTER)
        {
            if(viewInfo.m_caption.first)
            {
                auto* groupbox = new QGroupBox(m_qtWindow);
                groupbox->setObjectName(qId + '/' + viewInfo.m_caption.second.c_str());
                groupbox->setTitle(QString::fromStdString(viewInfo.m_caption.second));
                insideWidget = groupbox;
            }
            else
            {
                insideWidget = new QWidget(m_qtWindow);
                insideWidget->setObjectName(qId + '/' + QString("%1").arg(idx));
            }

            QWidget* widget = insideWidget;
            SIGHT_ASSERT(
                "multiple center views are not managed in Qt version of CardinalLayoutManager",
                !hasCentral
            );

            if(!viewInfo.m_backgroundColor.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                widget->setStyleSheet(style + qApp->styleSheet());
            }

            if(viewInfo.m_useScrollBar)
            {
                scrollArea = new QScrollArea(m_qtWindow);
                scrollArea->setWidget(widget);
                scrollArea->setWidgetResizable(true);
                if(!viewInfo.m_backgroundColor.empty())
                {
                    std::array<std::uint8_t, 4> rgba {};
                    data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                    std::stringstream ss;
                    ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                    << static_cast<std::int16_t>(rgba[1]) << ','
                    << static_cast<std::int16_t>(rgba[2]) << ','
                    << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                    const QString style = QString::fromStdString(ss.str());
                    scrollArea->setStyleSheet(style + qApp->styleSheet());
                }

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
            auto* dockWidget = new QDockWidget(m_qtWindow);
            insideWidget = new QWidget(dockWidget);
            QDockWidget::DockWidgetFeatures features;

            features = QDockWidget::DockWidgetMovable;

            Qt::DockWidgetArea area {Qt::NoDockWidgetArea};

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
            else
            {
                area = Qt::NoDockWidgetArea;
            }

            m_qtWindow->addDockWidget(area, dockWidget);
            dockWidget->setFeatures(features);

            if(viewInfo.m_caption.first)
            {
                dockWidget->setWindowTitle(QString::fromStdString(viewInfo.m_caption.second));
                dockWidget->setObjectName(qId + '/' + viewInfo.m_caption.second.c_str());
                dockWidget->setMinimumSize(0, 0);
            }
            else
            {
                // Remove title bar
                // Ensure widget->sizeHint() doesn't return a -1 size that will trigger a runtime warning:
                // As setTitleBarWidget require a widget that have a valid QWidget::sizeHint()
                // and QWidget::sizeHint() return -1 for widget without a layout...
                auto* anotherLayout = new QHBoxLayout;
                anotherLayout->setSpacing(0);
                anotherLayout->setMargin(0);

                auto* widget = new QWidget;
                widget->setObjectName(qId + '/' + QString("%1").arg(idx));
                widget->setLayout(anotherLayout);

                dockWidget->setMinimumSize(
                    std::max(viewInfo.m_minSize.first, 0),
                    std::max(viewInfo.m_minSize.second, 0)
                );
                dockWidget->setMaximumSize(
                    std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
                    std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
                );
                dockWidget->setTitleBarWidget(widget);
            }

            if(viewInfo.m_useScrollBar)
            {
                scrollArea = new QScrollArea(dockWidget);
                scrollArea->setWidget(insideWidget);
                scrollArea->setWidgetResizable(true);
                if(viewInfo.m_backgroundColor != "default")
                {
                    const QString style = QString::fromStdString(
                        "QWidget { background-color: " + viewInfo.m_backgroundColor + ";}"
                    );
                    scrollArea->setStyleSheet(style);
                }

                dockWidget->setWidget(scrollArea);
            }
            else
            {
                dockWidget->setWidget(insideWidget);
            }

            if(!viewInfo.m_visible)
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

        if(viewInfo.m_useScrollBar)
        {
            scrollArea->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
            scrollArea->setMaximumSize(
                std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
                std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
            );
        }
        else
        {
            insideWidget->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
            insideWidget->setMaximumSize(
                std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
                std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
            );
        }

        idx++;

        //TODO
        // - viewInfo.m_position
        // - viewInfo.m_layer
        // - viewInfo.m_row
        // - viewInfo.m_visible

        ui::qt::container::QtContainer::sptr subContainer = ui::qt::container::QtContainer::New();
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

} // namespace sight::ui::qt
