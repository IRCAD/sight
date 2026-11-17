/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "ui/qt/widget/notification_label.hpp"

#include <QEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QTextDocument>
#include <QTextOption>

#include <algorithm>
#include <cmath>

namespace sight::ui::qt::widget
{

//------------------------------------------------------------------------------

void notification_label::set_icon(const QIcon& _icon)
{
    m_icon = _icon;

    this->update_margins();
    this->updateGeometry();
    this->update();
}

//------------------------------------------------------------------------------

void notification_label::set_icon_size(int _size)
{
    m_icon_size = _size;

    this->update_margins();
    this->updateGeometry();
    this->update();
}

//------------------------------------------------------------------------------

int notification_label::icon_side() const
{
    return this->compute_placement().side;
}

//------------------------------------------------------------------------------

QRect notification_label::icon_rect() const
{
    return this->compute_placement().icon;
}

//------------------------------------------------------------------------------

QRect notification_label::text_rect() const
{
    return this->compute_placement().text;
}

//------------------------------------------------------------------------------

int notification_label::icon_room(int _height) const
{
    if(m_icon.isNull())
    {
        return 0;
    }

    const QMargins frame = this->frame_margins(QRect(0, 0, this->width(), _height));
    return this->wanted_side(_height - frame.top() - frame.bottom()) + GAP;
}

//------------------------------------------------------------------------------

int notification_label::heightForWidth(int _width) const
{
    return std::max(QLabel::heightForWidth(_width + this->hint_extra_width()), this->icon_min_height());
}

//------------------------------------------------------------------------------

QSize notification_label::sizeHint() const
{
    const QSize hint = QLabel::sizeHint();
    return {hint.width() - this->hint_extra_width(), std::max(hint.height(), this->icon_min_height())};
}

//------------------------------------------------------------------------------

QSize notification_label::minimumSizeHint() const
{
    const QSize hint = QLabel::minimumSizeHint();
    return {hint.width() - this->hint_extra_width(), std::max(hint.height(), this->icon_min_height())};
}

//------------------------------------------------------------------------------

void notification_label::paintEvent(QPaintEvent* _event)
{
    if(m_icon.isNull())
    {
        QLabel::paintEvent(_event);
        return;
    }

    // QLabel::setText() is not virtual: text changes are only seen here.
    const auto current = this->compute_placement();
    this->setContentsMargins(current.margins);

    QLabel::paintEvent(_event);

    QPainter painter(this);
    painter.drawPixmap(current.icon, m_icon.pixmap(current.icon.size(), this->devicePixelRatioF()));
}

//------------------------------------------------------------------------------

void notification_label::resizeEvent(QResizeEvent* _event)
{
    QLabel::resizeEvent(_event);
    this->update_margins();
}

//------------------------------------------------------------------------------

void notification_label::changeEvent(QEvent* _event)
{
    QLabel::changeEvent(_event);

    if(_event->type() == QEvent::FontChange || _event->type() == QEvent::StyleChange)
    {
        this->update_margins();
    }
}

//------------------------------------------------------------------------------

notification_label::placement notification_label::compute_placement() const
{
    const QMargins frame = this->frame_margins(this->rect());
    const QRect area     = this->rect().marginsRemoved(frame);

    placement result;
    result.margins = frame;

    if(!m_icon.isNull())
    {
        result.side = this->wanted_side(area.height());

        // Back to the text height when the text no longer fits beside a larger icon.
        if(m_icon_size <= 0)
        {
            const int width   = area.width() - result.side - GAP;
            const QSizeF text = this->measure_text(width);
            if(text.width() > width || text.height() > area.height())
            {
                result.side = this->fontMetrics().height();
            }
        }

        result.margins.setLeft(frame.left() + result.side + GAP);
    }

    // Place the text as QLabel does, in the area left by the icon.
    const QRect text_area = area.adjusted(result.margins.left() - frame.left(), 0, 0, 0);
    const auto align      = QStyle::visualAlignment(this->layoutDirection(), this->alignment());

    // QLabel indents left or right aligned text, by half an 'x' by default when it has a frame.
    int indent = 0;
    if(align.testFlag(Qt::AlignLeft) || align.testFlag(Qt::AlignRight))
    {
        indent = this->indent();
        if(indent < 0)
        {
            indent = this->frameWidth() > 0 ? this->fontMetrics().horizontalAdvance(u'x') / 2 : 0;
        }
    }

    const int available  = text_area.width() - indent;
    const int measured   = static_cast<int>(std::ceil(this->measure_text(available).width()));
    const int text_width = std::clamp(measured, 0, std::max(available, 0));

    int left = text_area.left() + (text_area.width() - text_width) / 2;
    if(align.testFlag(Qt::AlignLeft))
    {
        left = text_area.left() + indent;
    }
    else if(align.testFlag(Qt::AlignRight))
    {
        left = text_area.right() + 1 - indent - text_width;
    }

    result.text = QRect(left, area.top(), text_width, area.height());

    if(result.side > 0)
    {
        result.icon = QRect(
            left - GAP - result.side,
            area.top() + (area.height() - result.side) / 2,
            result.side,
            result.side
        );
    }

    return result;
}

//------------------------------------------------------------------------------

void notification_label::update_margins()
{
    this->setContentsMargins(this->compute_placement().margins);
}

//------------------------------------------------------------------------------

QMargins notification_label::frame_margins(const QRect& _rect) const
{
    QStyleOptionFrame option;
    this->initStyleOption(&option);

    // initStyleOption() uses frameRect(), which depends on the current contents margins.
    option.rect = _rect;

    const QRect contents = this->style()->subElementRect(QStyle::SE_ShapedFrameContents, &option, this);
    return {
        contents.left() - _rect.left(),
        contents.top() - _rect.top(),
        _rect.right() - contents.right(),
        _rect.bottom() - contents.bottom()
    };
}

//------------------------------------------------------------------------------

QSizeF notification_label::measure_text(int _width) const
{
    QTextDocument document;
    document.setDefaultFont(this->font());
    document.setDocumentMargin(0);

    QTextOption option = document.defaultTextOption();
    option.setWrapMode(this->wordWrap() ? QTextOption::WordWrap : QTextOption::NoWrap);
    document.setDefaultTextOption(option);

    // Same format selection as QLabel.
    const auto format = this->textFormat() == Qt::AutoText
                        ? (Qt::mightBeRichText(this->text()) ? Qt::RichText : Qt::PlainText)
                        : this->textFormat();

    if(format == Qt::RichText)
    {
        document.setHtml(this->text());
    }
    else if(format == Qt::MarkdownText)
    {
        document.setMarkdown(this->text());
    }
    else
    {
        document.setPlainText(this->text());
    }

    document.setTextWidth(this->wordWrap() ? std::max(_width, 0) : -1);
    return {document.idealWidth(), document.size().height()};
}

//------------------------------------------------------------------------------

int notification_label::wanted_side(int _height) const
{
    // An explicit size grows the label through its hints, but a fixed height (popups) still caps it.
    return m_icon_size > 0
           ? std::min(m_icon_size, _height)
           : std::max(this->fontMetrics().height(), _height * AUTO_ICON_PERCENT / 100);
}

//------------------------------------------------------------------------------

int notification_label::hint_extra_width() const
{
    if(m_icon.isNull() || m_icon_size > 0)
    {
        return 0;
    }

    const int room = this->contentsMargins().left() - this->frame_margins(this->rect()).left();
    return std::max(room - (this->fontMetrics().height() + GAP), 0);
}

//------------------------------------------------------------------------------

int notification_label::icon_min_height() const
{
    if(m_icon.isNull() || m_icon_size <= 0)
    {
        return 0;
    }

    const QMargins frame = this->frame_margins(this->rect());
    return m_icon_size + frame.top() + frame.bottom();
}

} // namespace sight::ui::qt::widget
