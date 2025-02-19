/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

class paint_filter final : public QObject
{
public:

    paint_filter()
    {
        qApp->installEventFilter(this);
    }

    ~paint_filter() final
    {
        qApp->removeEventFilter(this);
    }

protected:

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* /*_watched*/, QEvent* _event) final
    {
        return _event->type() == QEvent::Paint;
    }
};

//-----------------------------------------------------------------------------

void cardinal::create_layout(ui::container::widget::sptr _parent, const std::string& _id)
{
    m_parent_container = std::dynamic_pointer_cast<ui::qt::container::widget>(_parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parent_container);
    const QString q_id = QString::fromStdString(_id);
    m_parent_container->get_qt_container()->setObjectName(q_id);

    m_qt_window = new QMainWindow();

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_qt_window);

    m_parent_container->set_layout(layout);

    const std::list<view_info>& views = this->get_views_info();

    [[maybe_unused]] bool has_central = false;

    for(std::size_t idx = 0 ; const view_info& view_info : views)
    {
        QWidget* inside_widget   = nullptr;
        QScrollArea* scroll_area = nullptr;

        if(view_info.m_align == center)
        {
            if(view_info.m_caption.first)
            {
                auto* groupbox = new QGroupBox(m_qt_window);
                groupbox->setObjectName(q_id + '/' + view_info.m_caption.second.c_str());
                groupbox->setTitle(QString::fromStdString(view_info.m_caption.second));
                inside_widget = groupbox;
            }
            else
            {
                inside_widget = new QWidget(m_qt_window);
                inside_widget->setObjectName(q_id + '/' + QString("%1").arg(idx));
            }

            QWidget* widget = inside_widget;
            SIGHT_ASSERT(
                "multiple center views are not managed in Qt version of cardinal",
                !has_central
            );

            if(!view_info.m_background_color.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexa_string_to_rgba(view_info.m_background_color, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                widget->setStyleSheet(style + qApp->styleSheet());
            }

            if(!view_info.m_qss_key.empty())
            {
                widget->setProperty("class", QString::fromStdString(view_info.m_qss_key));
            }

            if(view_info.m_use_scroll_bar)
            {
                scroll_area = new QScrollArea(m_qt_window);
                scroll_area->setWidget(widget);
                scroll_area->setWidgetResizable(true);
                if(!view_info.m_background_color.empty())
                {
                    std::array<std::uint8_t, 4> rgba {};
                    data::tools::color::hexa_string_to_rgba(view_info.m_background_color, rgba);
                    std::stringstream ss;
                    ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                    << static_cast<std::int16_t>(rgba[1]) << ','
                    << static_cast<std::int16_t>(rgba[2]) << ','
                    << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                    const QString style = QString::fromStdString(ss.str());
                    scroll_area->setStyleSheet(style + qApp->styleSheet());
                }

                scroll_area->setMinimumSize(
                    std::max(view_info.m_min_size.first, 0),
                    std::max(view_info.m_min_size.second, 0)
                );

                scroll_area->setMaximumSize(
                    std::min(view_info.m_max_size.first, QWIDGETSIZE_MAX),
                    std::min(view_info.m_max_size.second, QWIDGETSIZE_MAX)
                );

                m_qt_window->setCentralWidget(scroll_area);
            }
            else
            {
                widget->setMinimumSize(
                    std::max(view_info.m_min_size.first, 0),
                    std::max(view_info.m_min_size.second, 0)
                );

                widget->setMaximumSize(
                    std::min(view_info.m_max_size.first, QWIDGETSIZE_MAX),
                    std::min(view_info.m_max_size.second, QWIDGETSIZE_MAX)
                );

                m_qt_window->setCentralWidget(widget);
            }

            inside_widget->setVisible(view_info.m_visible);

            if(!view_info.m_tool_tip.empty())
            {
                inside_widget->setToolTip(QString::fromStdString(view_info.m_tool_tip));
            }

            has_central = true;
        }
        else
        {
            auto* dock_widget = new QDockWidget(m_qt_window);
            inside_widget = new QWidget(dock_widget);
            QDockWidget::DockWidgetFeatures features;

            features = QDockWidget::DockWidgetMovable;

            Qt::DockWidgetArea area {Qt::NoDockWidgetArea};

            if(view_info.m_align == right)
            {
                area = Qt::RightDockWidgetArea;
            }
            else if(view_info.m_align == left)
            {
                area = Qt::LeftDockWidgetArea;
            }
            else if(view_info.m_align == bottom)
            {
                area = Qt::BottomDockWidgetArea;
            }
            else if(view_info.m_align == top)
            {
                area = Qt::TopDockWidgetArea;
            }
            else
            {
                area = Qt::NoDockWidgetArea;
            }

            m_qt_window->addDockWidget(area, dock_widget);
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
                dock_widget->setObjectName(q_id + "/QDockWidget/" + QString("%1").arg(idx));
            }

            // Use an intermediate widget to avoid unwanted interaction with other child layout when resizing
            // This allow us to correctly "swap" child content when switching between central and docked
            auto* dock_child        = new QWidget(dock_widget);
            auto* dock_child_layout = new QHBoxLayout(dock_child);
            dock_child_layout->setContentsMargins(0, 0, 0, 0);
            dock_child->setLayout(dock_child_layout);
            dock_widget->setWidget(dock_child);

            if(view_info.m_use_scroll_bar)
            {
                scroll_area = new QScrollArea(dock_child);
                scroll_area->setWidget(inside_widget);
                scroll_area->setWidgetResizable(true);
                if(view_info.m_background_color != "default")
                {
                    const QString style = QString::fromStdString(
                        "QWidget { background-color: " + view_info.m_background_color + ";}"
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
                std::max(view_info.m_min_size.first, 0),
                std::max(view_info.m_min_size.second, 0)
            );

            dock_child->setMaximumSize(
                std::min(view_info.m_max_size.first, QWIDGETSIZE_MAX),
                std::min(view_info.m_max_size.second, QWIDGETSIZE_MAX)
            );

            if(!view_info.m_visible)
            {
                dock_widget->setVisible(false);
            }

            if(!view_info.m_tool_tip.empty())
            {
                dock_widget->setToolTip(QString::fromStdString(view_info.m_tool_tip));
            }

            dock_widget->adjustSize();

            if(!view_info.m_is_resizable)
            {
                QPointer<QDockWidget> dock_widget_ptr = dock_widget;

                // Freeze the size of the dock widget
                // Do it at the very end to be sure everything is correctly initialized
                QTimer::singleShot(
                    0,
                    [dock_widget_ptr]()
                    {
                        if(dock_widget_ptr.isNull())
                        {
                            return;
                        }

                        const auto& size = dock_widget_ptr->size();
                        dock_widget_ptr->setFixedSize(size.isValid() ? size : dock_widget_ptr->sizeHint());
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
        sub_container->set_qt_container(inside_widget);
        m_sub_views.push_back(sub_container);
    }

    m_qt_window->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

//-----------------------------------------------------------------------------

void cardinal::destroy_layout()
{
    this->destroy_sub_views();
    m_parent_container->clean();
}

//------------------------------------------------------------------------------

void cardinal::modify_layout(const ui::parameter_t& _parameter, const std::string& _key)
{
    if(_key == "swap")
    {
        {
            paint_filter filter;

            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

            // Get the wid of the container to switch to central or maximize/restore.
            SIGHT_THROW_IF("WID parameter is missing.", !std::holds_alternative<std::string>(_parameter));
            const auto& wid = std::get<std::string>(_parameter);

            // Get the container from the registry.
            const auto& container = sight::ui::registry::get_wid_container(wid);
            SIGHT_THROW_IF("Cannot retrieve container '" << wid << "' in the registry.", !container);

            const auto& qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(container);
            SIGHT_ASSERT("Container '" << wid << "' is not a widget.", qt_container);

            // Finally, get the underlying QWidget
            auto* const widget = qt_container->get_qt_root();
            SIGHT_ASSERT("Container '" << wid << "' have no QWidget.", widget);

            if(m_qt_window->centralWidget() == widget)
            {
                // Already central. Just maximize/minimize it
                for(auto* const child : m_qt_window->children())
                {
                    if(auto* const dock_child = qobject_cast<QDockWidget*>(child); dock_child)
                    {
                        dock_child->setVisible(!dock_child->isVisible());
                    }
                }
            }
            else if(auto* const dock_child = qobject_cast<QDockWidget*>(widget->parentWidget()); dock_child)
            {
                // Not central. Move it to the central widget
                // Save the state to restore the sizes later
                const auto state = m_qt_window->saveState();

                // Swap the central widget with the docked widget
                auto* dock_child_layout = dock_child->layout();
                dock_child_layout->removeWidget(widget);
                auto* const central_widget = m_qt_window->takeCentralWidget();
                m_qt_window->setCentralWidget(widget);
                dock_child_layout->addWidget(central_widget);

                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

                m_qt_window->restoreState(state);
            }
        }

        // Force a repaint of the top level widget to compensate the ones we blocked
        m_qt_window->topLevelWidget()->repaint();
    }
    else
    {
        view::modify_layout(_parameter, _key);
    }
}

} // namespace sight::ui::qt::layout
