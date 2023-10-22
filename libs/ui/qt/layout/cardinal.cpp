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

#include "ui/qt/app.hpp"

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

void cardinal::createLayout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parentContainer = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parentContainer);
    const QString q_id = QString::fromStdString(_id);
    m_parentContainer->getQtContainer()->setObjectName(q_id);

    m_qtWindow = new QMainWindow();

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_qtWindow);

    m_parentContainer->setLayout(layout);

    const std::list<ViewInfo>& views = this->getViewsInfo();

    [[maybe_unused]] bool has_central = false;

    for(std::size_t idx = 0 ; const ViewInfo& view_info : views)
    {
        QWidget* inside_widget   = nullptr;
        QScrollArea* scroll_area = nullptr;

        if(view_info.m_align == CENTER)
        {
            if(view_info.m_caption.first)
            {
                auto* groupbox = new QGroupBox(m_qtWindow);
                groupbox->setObjectName(q_id + '/' + view_info.m_caption.second.c_str());
                groupbox->setTitle(QString::fromStdString(view_info.m_caption.second));
                inside_widget = groupbox;
            }
            else
            {
                inside_widget = new QWidget(m_qtWindow);
                inside_widget->setObjectName(q_id + '/' + QString("%1").arg(idx));
            }

            QWidget* widget = inside_widget;
            SIGHT_ASSERT(
                "multiple center views are not managed in Qt version of cardinal",
                !has_central
            );

            if(!view_info.m_backgroundColor.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexaStringToRGBA(view_info.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                widget->setStyleSheet(style + qApp->styleSheet());
            }

            if(view_info.m_useScrollBar)
            {
                scroll_area = new QScrollArea(m_qtWindow);
                scroll_area->setWidget(widget);
                scroll_area->setWidgetResizable(true);
                if(!view_info.m_backgroundColor.empty())
                {
                    std::array<std::uint8_t, 4> rgba {};
                    data::tools::color::hexaStringToRGBA(view_info.m_backgroundColor, rgba);
                    std::stringstream ss;
                    ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                    << static_cast<std::int16_t>(rgba[1]) << ','
                    << static_cast<std::int16_t>(rgba[2]) << ','
                    << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                    const QString style = QString::fromStdString(ss.str());
                    scroll_area->setStyleSheet(style + qApp->styleSheet());
                }

                scroll_area->setMinimumSize(
                    std::max(view_info.m_minSize.first, 0),
                    std::max(view_info.m_minSize.second, 0)
                );

                scroll_area->setMaximumSize(
                    std::min(view_info.m_maxSize.first, QWIDGETSIZE_MAX),
                    std::min(view_info.m_maxSize.second, QWIDGETSIZE_MAX)
                );

                m_qtWindow->setCentralWidget(scroll_area);
            }
            else
            {
                widget->setMinimumSize(
                    std::max(view_info.m_minSize.first, 0),
                    std::max(view_info.m_minSize.second, 0)
                );

                widget->setMaximumSize(
                    std::min(view_info.m_maxSize.first, QWIDGETSIZE_MAX),
                    std::min(view_info.m_maxSize.second, QWIDGETSIZE_MAX)
                );

                m_qtWindow->setCentralWidget(widget);
            }

            inside_widget->setVisible(view_info.m_visible);

            if(!view_info.m_toolTip.empty())
            {
                inside_widget->setToolTip(QString::fromStdString(view_info.m_toolTip));
            }

            has_central = true;
        }
        else
        {
            auto* dock_widget = new QDockWidget(m_qtWindow);
            inside_widget = new QWidget(dock_widget);
            QDockWidget::DockWidgetFeatures features;

            features = QDockWidget::DockWidgetMovable;

            Qt::DockWidgetArea area {Qt::NoDockWidgetArea};

            if(view_info.m_align == RIGHT)
            {
                area = Qt::RightDockWidgetArea;
            }
            else if(view_info.m_align == LEFT)
            {
                area = Qt::LeftDockWidgetArea;
            }
            else if(view_info.m_align == BOTTOM)
            {
                area = Qt::BottomDockWidgetArea;
            }
            else if(view_info.m_align == TOP)
            {
                area = Qt::TopDockWidgetArea;
            }
            else
            {
                area = Qt::NoDockWidgetArea;
            }

            m_qtWindow->addDockWidget(area, dock_widget);
            dock_widget->setFeatures(features);

            if(view_info.m_caption.first)
            {
                dock_widget->setWindowTitle(QString::fromStdString(view_info.m_caption.second));
                dock_widget->setObjectName(q_id + '/' + view_info.m_caption.second.c_str());
                dock_widget->setMinimumSize(0, 0);
            }
            else
            {
                // Remove title bar
                // Ensure widget->sizeHint() doesn't return a -1 size that will trigger a runtime warning:
                // As setTitleBarWidget require a widget that have a valid QWidget::sizeHint()
                // and QWidget::sizeHint() return -1 for widget without a layout...
                auto* another_layout = new QHBoxLayout;
                another_layout->setSpacing(0);
                another_layout->setMargin(0);

                auto* widget = new QWidget;
                widget->setObjectName(q_id + '/' + QString("%1").arg(idx));
                widget->setLayout(another_layout);

                dock_widget->setTitleBarWidget(widget);
            }

            // Use an intermediate widget to avoid unwanted interaction with other child layout when resizing
            // This allow us to correctly "swap" child content when switching between central and docked
            auto* dock_child        = new QWidget(dock_widget);
            auto* dock_child_layout = new QHBoxLayout(dock_child);
            dock_child_layout->setContentsMargins(0, 0, 0, 0);
            dock_child->setLayout(dock_child_layout);
            dock_widget->setWidget(dock_child);

            if(view_info.m_useScrollBar)
            {
                scroll_area = new QScrollArea(dock_child);
                scroll_area->setWidget(inside_widget);
                scroll_area->setWidgetResizable(true);
                if(view_info.m_backgroundColor != "default")
                {
                    const QString style = QString::fromStdString(
                        "QWidget { background-color: " + view_info.m_backgroundColor + ";}"
                    );
                    scroll_area->setStyleSheet(style);
                }

                dock_child_layout->addWidget(scroll_area);
            }
            else
            {
                dock_child_layout->addWidget(inside_widget);
            }

            dock_child->setMinimumSize(
                std::max(view_info.m_minSize.first, 0),
                std::max(view_info.m_minSize.second, 0)
            );

            dock_child->setMaximumSize(
                std::min(view_info.m_maxSize.first, QWIDGETSIZE_MAX),
                std::min(view_info.m_maxSize.second, QWIDGETSIZE_MAX)
            );

            if(!view_info.m_visible)
            {
                dock_widget->setVisible(false);
            }

            if(!view_info.m_toolTip.empty())
            {
                dock_widget->setToolTip(QString::fromStdString(view_info.m_toolTip));
            }

            dock_widget->adjustSize();

            if(!view_info.m_isResizable)
            {
                // Freeze the size of the dock widget
                // Do it at the very end to be sure everything is correctly initialized
                QTimer::singleShot(
                    0,
                    [dock_widget]()
                    {
                        const auto& size = dock_widget->size();
                        dock_widget->setFixedSize(size.isValid() ? size : dock_widget->sizeHint());
                    });
            }
        }

        idx++;

        //TODO
        // - viewInfo.m_position
        // - viewInfo.m_layer
        // - viewInfo.m_row
        // - viewInfo.m_visible

        ui::qt::container::widget::sptr sub_container = ui::qt::container::widget::make();
        sub_container->setQtContainer(inside_widget);
        m_subViews.push_back(sub_container);
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

void cardinal::modifyLayout(const ui::parameter_t& _parameter, const std::string& _key)
{
    if(_key == "swap")
    {
        // Get the wid of the container to switch to central or maximize/restore.
        SIGHT_THROW_IF("WID parameter is missing.", !std::holds_alternative<std::string>(_parameter));
        const auto& wid = std::get<std::string>(_parameter);

        // Get the container from the registry.
        const auto& container = sight::ui::registry::get_wid_container(wid);
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
            auto* const dock_child = widget->parentWidget();

            // Swap the central widget with the docked widget
            auto* dock_child_layout = dock_child->layout();
            dock_child_layout->removeWidget(widget);
            auto* const central_widget = m_qtWindow->takeCentralWidget();
            m_qtWindow->setCentralWidget(widget);
            dock_child_layout->addWidget(central_widget);

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
        view::modifyLayout(_parameter, _key);
    }
}

} // namespace sight::ui::qt::layout
