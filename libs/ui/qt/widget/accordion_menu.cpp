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

#include "accordion_menu.hpp"

#include <core/runtime/path.hpp>
#include <core/spy_log.hpp>

#include <QAbstractButton>
#include <QApplication>
#include <QEvent>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStyle>
#include <QToolButton>

//------------------------------------------------------------------------------

[[nodiscard]] static QPixmap rotate(QPixmap _pixmap, double _angle_in_degrees)
{
    return _pixmap.transformed(QTransform().rotate(_angle_in_degrees));
}

namespace sight::ui::qt::widget
{

accordion_menu::accordion_menu(QWidget* _parent, Qt::Orientation _orientation) :
    QWidget(_parent),
    m_orientation(_orientation)
{
    m_bracket->setObjectName("Bracket");
    // TODO: load bracket.svg or bracket_white following current stylesheet ?
    m_pixmap =
        QPixmap(
            QString::fromStdString(
                core::runtime::get_library_resource_file_path(
                    "sight::ui::qt/Bracket_white.svg"
                ).string()
            )
        );
    m_pixmap = m_pixmap.scaled(m_pixmap.size() / 20);
    m_bracket->setIcon(m_pixmap);
    m_bracket->setParent(this);
    m_bracket->setAttribute(Qt::WA_TransparentForMouseEvents);
    setProperty("folded", true);
    setProperty("class", "accordion_menu");
    QObject::connect(
        m_animationGroup,
        &QAnimationGroup::finished,
        [this]
        {
            if(m_folded)
            {
                std::vector<QWidget*> widgets = childrenWidgets();
                // Hide all widgets except the first one
                std::for_each(widgets.begin() + 1, widgets.end(), std::mem_fn(&QWidget::hide));
                setProperty("folded", true);
                qApp->style()->unpolish(this);
                qApp->style()->polish(this);
            }
        });
}

accordion_menu::accordion_menu(QToolBar* _tool_bar) :
    accordion_menu(_tool_bar, _tool_bar->orientation())
{
}

//------------------------------------------------------------------------------

void accordion_menu::fold()
{
    if(m_folded)
    {
        return;
    }

    m_folded = true;
    m_animationGroup->setDirection(QAbstractAnimation::Backward);
    m_animationGroup->start();
}

//------------------------------------------------------------------------------

void accordion_menu::unfold()
{
    if(!m_folded)
    {
        return;
    }

    if(childrenWidgets().size() < 2)
    {
        return;
    }

    m_folded = false;
    setProperty("folded", false);
    qApp->style()->unpolish(this);
    qApp->style()->polish(this);
    std::ranges::for_each(childrenWidgets(), &QWidget::show);
    m_animationGroup->setDirection(QAbstractAnimation::Forward);
    m_animationGroup->start();
}

//------------------------------------------------------------------------------

void accordion_menu::setFolded(bool _folded)
{
    if(_folded)
    {
        fold();
    }
    else
    {
        unfold();
    }
}

//------------------------------------------------------------------------------

void accordion_menu::setUnfolded(bool _unfolded)
{
    setFolded(!_unfolded);
}

//------------------------------------------------------------------------------

void accordion_menu::toggleFold()
{
    setFolded(!m_folded);
}

//------------------------------------------------------------------------------

bool accordion_menu::isFolded() const
{
    return m_folded;
}

//------------------------------------------------------------------------------

void accordion_menu::addWidget(QWidget* _w)
{
    _w->setParent(this);
}

//------------------------------------------------------------------------------

void accordion_menu::update()
{
    QObject::disconnect(m_firstButtonConnection);
    int current_time = m_animationGroup->currentTime();
    m_animationGroup->clear();
    m_animationGroup->setCurrentTime(current_time);
    std::vector<QWidget*> widgets = childrenWidgets();
    if(widgets.empty())
    {
        return;
    }

    widgets[0]->removeEventFilter(this);

    auto* first_button = qobject_cast<QAbstractButton*>(widgets[0]);
    SIGHT_ASSERT("The first widget must be a button", first_button);
    if(m_orientation == Qt::Horizontal)
    {
        first_button->move(0, (height() - first_button->height()) / 2);
    }
    else
    {
        first_button->move((width() - first_button->width()) / 2, 0);
    }

    first_button->installEventFilter(this);
    first_button->show();
    // Show/hide all widgets except the first one
    std::for_each(widgets.begin() + 1, widgets.end(), [this](QWidget* _w){_w->setVisible(!m_folded);});
    m_firstButtonConnection =
        QObject::connect(first_button, &QAbstractButton::toggled, this, &accordion_menu::setUnfolded);

    std::for_each(
        widgets.begin() + 1,
        widgets.end(),
        [&first_button, this](QWidget* _w)
        {
            if(m_orientation == Qt::Horizontal)
            {
                _w->setMinimumHeight(first_button->height());
            }
            else
            {
                _w->setMinimumWidth(first_button->width());
            }

            _w->adjustSize();
        });

    m_bracket->adjustSize();
    auto* min_accordion_size_anim = new QPropertyAnimation(
        this,
        m_orientation == Qt::Horizontal ? "minimumWidth" : "minimumHeight"
    );
    min_accordion_size_anim->setStartValue(
        m_orientation == Qt::Horizontal ? first_button->width() : first_button->height()
    );
    int total_size = 0;
    std::ranges::for_each(
        widgets,
        [this, &total_size](QWidget* _w)
        {
            total_size += m_orientation == Qt::Horizontal ? _w->width() : _w->height();
        });
    min_accordion_size_anim->setEndValue(total_size);
    m_animationGroup->addAnimation(min_accordion_size_anim);
    if(m_orientation == Qt::Horizontal)
    {
        setMinimumWidth(min_accordion_size_anim->currentValue().toInt());
    }
    else
    {
        setMinimumHeight(min_accordion_size_anim->currentValue().toInt());
    }

    int offset = m_orientation == Qt::Horizontal ? first_button->width() : first_button->height();
    for(std::size_t i = 1 ; i < widgets.size() ; i++)
    {
        auto* anim           = new QPropertyAnimation(widgets[i], "pos");
        int margin_to_center =
            (m_orientation == Qt::Horizontal ? height() - widgets[i]->height() : width() - widgets[i]->width()) / 2;
        anim->setStartValue(
            m_orientation
            == Qt::Horizontal ? QPoint(0, margin_to_center) : QPoint(margin_to_center, 0)
        );
        anim->setEndValue(
            m_orientation
            == Qt::Horizontal ? QPoint(offset, margin_to_center) : QPoint(margin_to_center, offset)
        );
        offset += m_orientation == Qt::Horizontal ? widgets[i]->width() : widgets[i]->height();
        m_animationGroup->addAnimation(anim);
        widgets[i]->move(anim->currentValue().toPoint());
    }

    if(auto* tool_button = qobject_cast<QToolButton*>(first_button);
       !first_button->text().isEmpty() && tool_button != nullptr
       && tool_button->toolButtonStyle() != Qt::ToolButtonIconOnly)
    {
        m_bracket->move(
            (first_button->size().width() - first_button->iconSize().width()) / 2 + first_button->iconSize().width() - m_bracket->iconSize().width(),
            first_button->iconSize().height() - m_bracket->iconSize().height()
        );
    }
    else
    {
        m_bracket->move(
            first_button->width() - m_bracket->width() - 2,
            first_button->height() - m_bracket->height() - 2
        );
    }

    m_bracket->raise();
    auto* bracket_anim = new QVariantAnimation;
    bracket_anim->setStartValue(0);
    bracket_anim->setEndValue(360);
    QObject::connect(
        bracket_anim,
        &QVariantAnimation::valueChanged,
        [this](const QVariant& _rotation)
        {
            m_bracket->setIcon(rotate(m_pixmap, _rotation.toDouble()));
        });
    m_animationGroup->addAnimation(bracket_anim);
}

//------------------------------------------------------------------------------

bool accordion_menu::eventFilter(QObject* _o, QEvent* _e)
{
    if(QWidget* w = qobject_cast<QWidget*>(_o); w != nullptr && _e->type() == QEvent::EnabledChange)
    {
        m_bracket->setEnabled(w->isEnabled());
    }

    return false;
}

//------------------------------------------------------------------------------

void accordion_menu::resizeEvent(QResizeEvent* _e)
{
    if((m_orientation == Qt::Horizontal && _e->oldSize().height() != _e->size().height())
       || (m_orientation == Qt::Vertical && _e->oldSize().width() != _e->size().width()))
    {
        update();
    }
}

//------------------------------------------------------------------------------

void accordion_menu::childEvent(QChildEvent* _e)
{
    if(_e->added() || _e->removed())
    {
        update();
    }
}

//------------------------------------------------------------------------------

std::vector<QWidget*> accordion_menu::childrenWidgets() const
{
    std::vector<QWidget*> res;
    std::ranges::for_each(
        children(),
        [&res](QObject* _o)
        {
            if(_o->objectName() == "Bracket")
            {
                // Do not count the bracket in this list
                return;
            }

            if(auto* w = qobject_cast<QWidget*>(_o))
            {
                res.push_back(w);
            }
        });
    return res;
}

} // namespace sight::ui::qt::widget
