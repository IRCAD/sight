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

#include "ui/qt/layout/cardinal.hpp"

#include "ui/qt/App.hpp"

#include <core/base.hpp>

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>
#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QBoxLayout>
#include <QDockWidget>
#include <QGroupBox>
#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>

// cspell: ignore QWIDGETSIZE_MAX

SIGHT_REGISTER_GUI(sight::ui::qt::layout::cardinal, sight::ui::layout::cardinal::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void cardinal::createLayout(ui::container::widget::sptr parent, const std::string& id)
{
    m_parentContainer = std::dynamic_pointer_cast<ui::qt::container::widget>(parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parentContainer);
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
                "multiple center views are not managed in Qt version of cardinal",
                !hasCentral
            );

            if(!viewInfo.m_backgroundColor.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
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
                    data::tools::color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                    std::stringstream ss;
                    ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                    << static_cast<std::int16_t>(rgba[1]) << ','
                    << static_cast<std::int16_t>(rgba[2]) << ','
                    << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                    const QString style = QString::fromStdString(ss.str());
                    scrollArea->setStyleSheet(style + qApp->styleSheet());
                }

                scrollArea->setMinimumSize(
                    std::max(viewInfo.m_minSize.first, 0),
                    std::max(viewInfo.m_minSize.second, 0)
                );

                scrollArea->setMaximumSize(
                    std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
                    std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
                );

                m_qtWindow->setCentralWidget(scrollArea);
            }
            else
            {
                widget->setMinimumSize(
                    std::max(viewInfo.m_minSize.first, 0),
                    std::max(viewInfo.m_minSize.second, 0)
                );

                widget->setMaximumSize(
                    std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
                    std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
                );

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

                dockWidget->setTitleBarWidget(widget);
            }

            // Use an intermediate widget to avoid unwanted interaction with other child layout when resizing
            // This allow us to correctly "swap" child content when switching between central and docked
            auto* dockChild       = new QWidget(dockWidget);
            auto* dockChildLayout = new QHBoxLayout(dockChild);
            dockChildLayout->setContentsMargins(0, 0, 0, 0);
            dockChild->setLayout(dockChildLayout);
            dockWidget->setWidget(dockChild);

            if(viewInfo.m_useScrollBar)
            {
                scrollArea = new QScrollArea(dockChild);
                scrollArea->setWidget(insideWidget);
                scrollArea->setWidgetResizable(true);
                if(viewInfo.m_backgroundColor != "default")
                {
                    const QString style = QString::fromStdString(
                        "QWidget { background-color: " + viewInfo.m_backgroundColor + ";}"
                    );
                    scrollArea->setStyleSheet(style);
                }

                dockChildLayout->addWidget(scrollArea);
            }
            else
            {
                dockChildLayout->addWidget(insideWidget);
            }

            dockChild->setMinimumSize(
                std::max(viewInfo.m_minSize.first, 0),
                std::max(viewInfo.m_minSize.second, 0)
            );

            dockChild->setMaximumSize(
                std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
                std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
            );

            if(!viewInfo.m_visible)
            {
                dockWidget->setVisible(false);
            }

            if(!viewInfo.m_toolTip.empty())
            {
                dockWidget->setToolTip(QString::fromStdString(viewInfo.m_toolTip));
            }

            dockWidget->adjustSize();

            if(!viewInfo.m_isResizable)
            {
                // Freeze the size of the dock widget
                // Do it at the very end to be sure everything is correctly initialized
                QTimer::singleShot(
                    0,
                    [dockWidget]()
                    {
                        const auto& size = dockWidget->size();
                        dockWidget->setFixedSize(size.isValid() ? size : dockWidget->sizeHint());
                    });
            }
        }

        idx++;

        //TODO
        // - viewInfo.m_position
        // - viewInfo.m_layer
        // - viewInfo.m_row
        // - viewInfo.m_visible

        ui::qt::container::widget::sptr subContainer = ui::qt::container::widget::make();
        subContainer->setQtContainer(insideWidget);
        m_subViews.push_back(subContainer);
    }

    m_qtWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

//-----------------------------------------------------------------------------

void cardinal::destroyLayout()
{
    this->destroySubViews();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

void cardinal::modifyLayout(const ui::parameter_t& parameter, const std::string& key)
{
    if(key == "swap")
    {
        // Get the wid of the container to switch to central or maximize/restore.
        SIGHT_THROW_IF("WID parameter is missing.", !std::holds_alternative<std::string>(parameter));
        const auto& wid = std::get<std::string>(parameter);

        // Get the container from the registry.
        const auto& container = sight::ui::registry::getWIDContainer(wid);
        SIGHT_THROW_IF("Cannot retrieve container '" << wid << "' in the registry.", !container);

        const auto& qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(container);
        SIGHT_ASSERT("Container '" << wid << "' is not a widget.", qt_container);

        // Finally, get the underlying QWidget
        auto* const widget = qt_container->getQtContainer();
        SIGHT_ASSERT("Container '" << wid << "' have no QWidget.", widget);

        if(m_qtWindow->centralWidget() == widget)
        {
            // Already central. Just maximize/minimize it
            for(auto* const child : m_qtWindow->children())
            {
                if(auto* const dock_widget = qobject_cast<QDockWidget*>(child); dock_widget)
                {
                    dock_widget->setVisible(!dock_widget->isVisible());
                }
            }
        }
        else
        {
            // Not central. Move it to the central widget
            // Save the state to restore the sizes later
            const auto state = m_qtWindow->saveState();

            // Retrieve the dock child widget containing the widget
            auto* const dockChild = widget->parentWidget();

            // Swap the central widget with the docked widget
            auto* dockChildLayout = dockChild->layout();
            dockChildLayout->removeWidget(widget);
            auto* const centralWidget = m_qtWindow->takeCentralWidget();
            m_qtWindow->setCentralWidget(widget);
            dockChildLayout->addWidget(centralWidget);

            // Restore the state at the end when layouts have been updated
            QTimer::singleShot(
                0,
                [this, state]()
                {
                    m_qtWindow->restoreState(state);
                });
        }
    }
    else
    {
        view::modifyLayout(parameter, key);
    }
}

} // namespace sight::ui::qt::layout
