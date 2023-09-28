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

#include "AccordionMenu.hpp"

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

[[nodiscard]] static QPixmap rotate(QPixmap pixmap, double angleInDegrees)
{
    return pixmap.transformed(QTransform().rotate(angleInDegrees));
}

namespace sight::ui::qt::widget
{

AccordionMenu::AccordionMenu(QWidget* parent, Qt::Orientation orientation) :
    QWidget(parent),
    m_orientation(orientation)
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
    setProperty("class", "AccordionMenu");
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

AccordionMenu::AccordionMenu(QToolBar* toolBar) :
    AccordionMenu(toolBar, toolBar->orientation())
{
}

//------------------------------------------------------------------------------

void AccordionMenu::fold()
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

void AccordionMenu::unfold()
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

void AccordionMenu::setFolded(bool folded)
{
    if(folded)
    {
        fold();
    }
    else
    {
        unfold();
    }
}

//------------------------------------------------------------------------------

void AccordionMenu::setUnfolded(bool unfolded)
{
    setFolded(!unfolded);
}

//------------------------------------------------------------------------------

void AccordionMenu::toggleFold()
{
    setFolded(!m_folded);
}

//------------------------------------------------------------------------------

bool AccordionMenu::isFolded() const
{
    return m_folded;
}

//------------------------------------------------------------------------------

void AccordionMenu::addWidget(QWidget* w)
{
    w->setParent(this);
}

//------------------------------------------------------------------------------

void AccordionMenu::update()
{
    QObject::disconnect(m_firstButtonConnection);
    int currentTime = m_animationGroup->currentTime();
    m_animationGroup->clear();
    m_animationGroup->setCurrentTime(currentTime);
    std::vector<QWidget*> widgets = childrenWidgets();
    if(widgets.empty())
    {
        return;
    }

    widgets[0]->removeEventFilter(this);

    auto* firstButton = qobject_cast<QAbstractButton*>(widgets[0]);
    SIGHT_ASSERT("The first widget must be a button", firstButton);
    if(m_orientation == Qt::Horizontal)
    {
        firstButton->move(0, (height() - firstButton->height()) / 2);
    }
    else
    {
        firstButton->move((width() - firstButton->width()) / 2, 0);
    }

    firstButton->installEventFilter(this);
    firstButton->show();
    // Show/hide all widgets except the first one
    std::for_each(widgets.begin() + 1, widgets.end(), [this](QWidget* w){w->setVisible(!m_folded);});
    m_firstButtonConnection =
        QObject::connect(firstButton, &QAbstractButton::toggled, this, &AccordionMenu::setUnfolded);

    std::for_each(
        widgets.begin() + 1,
        widgets.end(),
        [&firstButton, this](QWidget* w)
        {
            if(m_orientation == Qt::Horizontal)
            {
                w->setMinimumHeight(firstButton->height());
            }
            else
            {
                w->setMinimumWidth(firstButton->width());
            }

            w->adjustSize();
        });

    m_bracket->adjustSize();
    auto* minAccordionSizeAnim = new QPropertyAnimation(
        this,
        m_orientation == Qt::Horizontal ? "minimumWidth" : "minimumHeight"
    );
    minAccordionSizeAnim->setStartValue(m_orientation == Qt::Horizontal ? firstButton->width() : firstButton->height());
    int totalSize = 0;
    std::ranges::for_each(
        widgets,
        [this, &totalSize](QWidget* w)
        {
            totalSize += m_orientation == Qt::Horizontal ? w->width() : w->height();
        });
    minAccordionSizeAnim->setEndValue(totalSize);
    m_animationGroup->addAnimation(minAccordionSizeAnim);
    if(m_orientation == Qt::Horizontal)
    {
        setMinimumWidth(minAccordionSizeAnim->currentValue().toInt());
    }
    else
    {
        setMinimumHeight(minAccordionSizeAnim->currentValue().toInt());
    }

    int offset = m_orientation == Qt::Horizontal ? firstButton->width() : firstButton->height();
    for(std::size_t i = 1 ; i < widgets.size() ; i++)
    {
        auto* anim         = new QPropertyAnimation(widgets[i], "pos");
        int marginToCenter =
            (m_orientation == Qt::Horizontal ? height() - widgets[i]->height() : width() - widgets[i]->width()) / 2;
        anim->setStartValue(m_orientation == Qt::Horizontal ? QPoint(0, marginToCenter) : QPoint(marginToCenter, 0));
        anim->setEndValue(
            m_orientation
            == Qt::Horizontal ? QPoint(offset, marginToCenter) : QPoint(marginToCenter, offset)
        );
        offset += m_orientation == Qt::Horizontal ? widgets[i]->width() : widgets[i]->height();
        m_animationGroup->addAnimation(anim);
        widgets[i]->move(anim->currentValue().toPoint());
    }

    if(auto* toolButton = qobject_cast<QToolButton*>(firstButton);
       !firstButton->text().isEmpty() && toolButton != nullptr
       && toolButton->toolButtonStyle() != Qt::ToolButtonIconOnly)
    {
        m_bracket->move(
            (firstButton->size().width() - firstButton->iconSize().width()) / 2 + firstButton->iconSize().width() - m_bracket->iconSize().width(),
            firstButton->iconSize().height() - m_bracket->iconSize().height()
        );
    }
    else
    {
        m_bracket->move(firstButton->width() - m_bracket->width() - 2, firstButton->height() - m_bracket->height() - 2);
    }

    m_bracket->raise();
    auto* bracketAnim = new QVariantAnimation;
    bracketAnim->setStartValue(0);
    bracketAnim->setEndValue(360);
    QObject::connect(
        bracketAnim,
        &QVariantAnimation::valueChanged,
        [this](const QVariant& rotation)
        {
            m_bracket->setIcon(rotate(m_pixmap, rotation.toDouble()));
        });
    m_animationGroup->addAnimation(bracketAnim);
}

//------------------------------------------------------------------------------

bool AccordionMenu::eventFilter(QObject* o, QEvent* e)
{
    if(QWidget* w = qobject_cast<QWidget*>(o); w != nullptr && e->type() == QEvent::EnabledChange)
    {
        m_bracket->setEnabled(w->isEnabled());
    }

    return false;
}

//------------------------------------------------------------------------------

void AccordionMenu::resizeEvent(QResizeEvent* e)
{
    if((m_orientation == Qt::Horizontal && e->oldSize().height() != e->size().height())
       || (m_orientation == Qt::Vertical && e->oldSize().width() != e->size().width()))
    {
        update();
    }
}

//------------------------------------------------------------------------------

void AccordionMenu::childEvent(QChildEvent* e)
{
    if(e->added() || e->removed())
    {
        update();
    }
}

//------------------------------------------------------------------------------

std::vector<QWidget*> AccordionMenu::childrenWidgets() const
{
    std::vector<QWidget*> res;
    std::ranges::for_each(
        children(),
        [&res](QObject* o)
        {
            if(o->objectName() == "Bracket")
            {
                // Do not count the bracket in this list
                return;
            }

            if(auto* w = qobject_cast<QWidget*>(o))
            {
                res.push_back(w);
            }
        });
    return res;
}

} // namespace sight::ui::qt::widget
