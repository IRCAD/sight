/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "ui/qt/layoutManager/OverlayLayoutManager.hpp"

#include <ui/base/registry/macros.hpp>

#include <QBoxLayout>
#include <QResizeEvent>
#include <QWidget>

SIGHT_REGISTER_GUI(
    sight::ui::qt::OverlayLayoutManager,
    sight::ui::base::layoutManager::OverlayLayoutManager::REGISTRY_KEY
)

namespace sight::ui::qt
{

//------------------------------------------------------------------------------

using Coord = base::layoutManager::OverlayLayoutManager::View::Coord;

//------------------------------------------------------------------------------

static int calculateOffset(Coord configuredOffset, int parentSize, int widgetSize)
{
    int res = configuredOffset.relative ? static_cast<int>((std::abs(configuredOffset.value) / 100.) * parentSize)
                                        : std::abs(configuredOffset.value);
    // If the size is negative, then the widget must be aligned to the right or the bottom.
    // In Qt, the position of a widget is defined by its top left corner, therefore we must substract the size of the
    // widget from the offset.
    if(configuredOffset.value <= 0 && configuredOffset.negative)
    {
        res = std::max(0, parentSize - widgetSize - res);
    }
    else
    {
        res = std::min(res, parentSize - widgetSize);
    }

    return res;
}

//------------------------------------------------------------------------------

static int calculateSize(Coord configuredSize, int parentSize)
{
    return configuredSize.relative ? static_cast<int>((configuredSize.value / 100.) * parentSize)
                                   : configuredSize.value;
}

/// Event filter which resizes the overlay if its parent size changed
class UI_QT_CLASS_QT_API OverlayResizeFilter : public QObject
{
private:

    QWidget* m_child = nullptr; // The widget to be resized
    Coord m_x;
    Coord m_y;
    Coord m_width;
    Coord m_height;

public:

    OverlayResizeFilter(
        QWidget* child,
        Coord x,
        Coord y,
        Coord width,
        Coord height
    ) :
        QObject(child),
        m_child(child),
        m_x(x),
        m_y(y),
        m_width(width),
        m_height(height)
    {
    }

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* target, QEvent* event) override
    {
        if(event->type() == QEvent::Resize)
        {
            auto& resizeEvent = static_cast<QResizeEvent&>(*event);
            if(m_width.value > 0 && m_width.relative)
            {
                m_child->setFixedWidth(calculateSize(m_width, resizeEvent.size().width()));
            }

            if(m_height.value > 0 && m_height.relative)
            {
                m_child->setFixedHeight(calculateSize(m_height, resizeEvent.size().height()));
            }

            if((m_x.value <= 0 && m_x.negative) || m_x.relative || (m_y.value <= 0 && m_y.negative) || m_y.relative)
            {
                int x = calculateOffset(m_x, resizeEvent.size().width(), m_child->width());
                int y = calculateOffset(m_y, resizeEvent.size().height(), m_child->height());
                m_child->move(x, y);
            }

            m_child->adjustSize();
        }

        return QObject::eventFilter(target, event);
    }
};

//-----------------------------------------------------------------------------

OverlayLayoutManager::OverlayLayoutManager(ui::base::GuiBaseObject::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

void OverlayLayoutManager::createLayout(ui::base::container::fwContainer::sptr parent, const std::string& id)
{
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);
    const QString qId = QString::fromStdString(id);
    m_parentContainer->getQtContainer()->setObjectName(qId);

    auto* layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_parentContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* viewport = new QWidget;
    layout->addWidget(viewport);
    auto viewportContainer = ui::qt::container::QtContainer::New();
    viewportContainer->setQtContainer(viewport);
    m_subViews.push_back(viewportContainer);
    for(std::size_t i = 1 ; i < views().size() ; i++)
    {
        const View& view = views()[i];
        auto* widget     = new QWidget(m_parentContainer->getQtContainer());

        if(view.width.value > 0)
        {
            widget->setFixedWidth(calculateSize(view.width, m_parentContainer->getQtContainer()->width()));
        }

        if(view.height.value > 0)
        {
            widget->setFixedHeight(calculateSize(view.height, m_parentContainer->getQtContainer()->height()));
        }

        if(view.minWidth > 0)
        {
            widget->setMinimumWidth(view.minWidth);
        }

        if(view.minHeight > 0)
        {
            widget->setMinimumHeight(view.minHeight);
        }

        if(view.opacity == 0.F)
        {
            widget->setStyleSheet("background-color: none");
        }

        int x = calculateOffset(view.x, m_parentContainer->getQtContainer()->width(), widget->width());
        int y = calculateOffset(view.y, m_parentContainer->getQtContainer()->height(), widget->height());
        widget->move(x, y);
        if((view.x.value <= 0 && view.x.negative) || view.x.relative
           || (view.y.value <= 0 && view.y.negative) || view.y.relative
           || (view.width.value > 0 && view.width.relative) || (view.height.value > 0 && view.height.relative))
        {
            m_parentContainer->getQtContainer()->installEventFilter(
                new OverlayResizeFilter(
                    widget,
                    view.x,
                    view.y,
                    view.width,
                    view.height
                )
            );
        }

        auto widgetContainer = ui::qt::container::QtContainer::New();
        widgetContainer->setQtContainer(widget);
        if(!view.visible)
        {
            widgetContainer->setVisible(false);
        }

        m_subViews.push_back(widgetContainer);
    }
}

//-----------------------------------------------------------------------------

void OverlayLayoutManager::destroyLayout()
{
    this->destroySubViews();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt
