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

#include <ui/qt/widget/notification_label.hpp>
#include <ui/test/gui_fixture.hpp>

#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <QCoreApplication>
#include <QFontMetrics>
#include <QIcon>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <utility>

using sight::ui::qt::widget::notification_label;

namespace
{

constexpr auto STYLE      = "background-color: black; color: white; font-size: 20px;";
constexpr auto SHORT_TEXT = "Short message";

/// A hidden window holding a label: laid out and painted without being shown, which headless runs need.
struct window
{
    /// A content-sized label is as high as its text, otherwise it fills the window.
    window(const QSize& _size, bool _content_sized, const QString& _text = SHORT_TEXT) :
        label(new notification_label(&widget))
    {
        auto* const layout = new QVBoxLayout(&widget);
        layout->setContentsMargins(0, 0, 0, 0);

        label->setWordWrap(true);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(STYLE);
        label->setText(_text);
        layout->addWidget(label);

        if(_content_sized)
        {
            layout->addStretch();
        }

        widget.resize(_size);
    }

    /// Lay out and paint until the label settles.
    void settle()
    {
        for(int i = 0 ; i < 3 ; ++i)
        {
            widget.layout()->activate();
            widget.grab();
            QCoreApplication::processEvents();
        }
    }

    QWidget widget;
    notification_label* label {nullptr};
};

} // namespace

//------------------------------------------------------------------------------

static QIcon red_icon()
{
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::red);
    return {pixmap};
}

//------------------------------------------------------------------------------

/// Runs a test body in the thread owning the widgets.
static void run(const std::function<void()>& _body)
{
    sight::core::thread::get_default_worker()->post_task<void>(_body).get();
}

//------------------------------------------------------------------------------

static int font_height(const notification_label& _label)
{
    return QFontMetrics(_label.font()).height();
}

//------------------------------------------------------------------------------

static int gap(const notification_label& _label)
{
    return _label.text_rect().left() - (_label.icon_rect().left() + _label.icon_rect().width());
}

//------------------------------------------------------------------------------

/// Horizontal extent of what is painted right of the icon, on the black background.
static std::pair<int, int> text_ink(notification_label& _label)
{
    const QImage image = _label.grab().toImage();
    const qreal ratio  = image.devicePixelRatio();
    // A few pixels past the icon, clear of its edge.
    const int start  = static_cast<int>((_label.icon_rect().right() + 3) * ratio);
    const QRect area = _label.contentsRect();

    int left  = image.width();
    int right = -1;
    for(int y = static_cast<int>(area.top() * ratio) ; y < static_cast<int>((area.bottom() + 1) * ratio) ; ++y)
    {
        for(int x = std::max(start, 0) ; x < image.width() ; ++x)
        {
            if(qGray(image.pixel(x, y)) > 64)
            {
                left  = std::min(left, x);
                right = std::max(right, x);
            }
        }
    }

    return {static_cast<int>(left / ratio), static_cast<int>(right / ratio)};
}

TEST_SUITE("sight::ui::qt::widget::notification_label")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "auto_icon_takes_available_height")
    {
        run(
            []
        {
            window w({600, 120}, false);
            w.label->set_icon(red_icon());
            w.settle();

            const int side = w.label->contentsRect().height() * notification_label::AUTO_ICON_PERCENT / 100;
            CHECK_GT(side, font_height(*w.label));
            CHECK_EQ(w.label->icon_side(), side);
            CHECK(w.label->rect().contains(w.label->icon_rect()));
            CHECK_EQ(w.label->icon_room(120), side + notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "auto_icon_never_smaller_than_font")
    {
        run(
            []
        {
            window w({600, 300}, true);
            w.label->set_icon(red_icon());
            w.settle();

            CHECK_EQ(w.label->icon_side(), font_height(*w.label));
            CHECK_EQ(w.label->icon_room(0), font_height(*w.label) + notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "auto_icon_falls_back_when_text_does_not_fit")
    {
        run(
            []
        {
            // A single word, just wide enough to fit beside the smallest icon only.
            const QString word = "Unbreakablenotificationword";
            window w({100, 120}, false, word);
            w.label->set_icon(red_icon());
            w.label->ensurePolished();

            const int word_width = QFontMetrics(w.label->font()).horizontalAdvance(word);
            w.widget.resize(font_height(*w.label) + notification_label::GAP + word_width + 10, 120);
            w.settle();

            CHECK_EQ(w.label->icon_side(), font_height(*w.label));
            CHECK_FALSE(w.label->text_rect().intersects(w.label->icon_rect()));
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "icon_and_text_centered_as_one_group")
    {
        run(
            []
        {
            window w({800, 300}, true);
            w.label->set_icon(red_icon());

            for(const int width : {800, 500})
            {
                w.widget.resize(width, 300);
                w.settle();

                CHECK_EQ(gap(*w.label), notification_label::GAP);

                const int group_center = (w.label->icon_rect().left() + w.label->text_rect().right() + 1) / 2;
                CHECK_LE(std::abs(group_center - (w.label->rect().width() / 2)), 1);
            }
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "left_and_right_alignment")
    {
        run(
            []
        {
            window w({800, 300}, true);
            w.label->set_icon(red_icon());

            w.label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            w.settle();
            CHECK_EQ(w.label->icon_rect().left(), 0);
            CHECK_EQ(gap(*w.label), notification_label::GAP);

            w.label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            w.settle();
            CHECK_EQ(w.label->text_rect().right(), w.label->rect().right());
            CHECK_EQ(gap(*w.label), notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "multiline_rich_text")
    {
        run(
            []
        {
            window w({800, 300}, true, "Line one<br/>A much <u>longer</u> line two");
            w.label->set_icon(red_icon());
            w.settle();

            CHECK_EQ(gap(*w.label), notification_label::GAP);
            CHECK_GT(w.label->contentsRect().height(), 2 * font_height(*w.label) - 2);

            // The text block is the widest line, as painted.
            const auto [left, right] = text_ink(*w.label);
            CHECK_LE(std::abs(left - w.label->text_rect().left()), 3);
            CHECK_LE(std::abs(right - w.label->text_rect().right()), 3);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "no_icon_keeps_the_plain_label_layout")
    {
        run(
            []
        {
            window w({600, 300}, true);
            w.label->setStyleSheet(QString(STYLE) + "border: 4px solid white;");
            w.settle();

            QLabel plain(SHORT_TEXT);
            plain.setWordWrap(true);
            plain.setAlignment(Qt::AlignCenter);
            plain.setStyleSheet(w.label->styleSheet());
            plain.ensurePolished();

            CHECK_EQ(w.label->contentsMargins(), plain.contentsMargins());
            CHECK_EQ(w.label->sizeHint(), plain.sizeHint());
            CHECK_EQ(w.label->icon_side(), 0);
            CHECK(w.label->icon_rect().isEmpty());
            CHECK_EQ(w.label->icon_room(120), 0);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "icon_removal_and_replacement")
    {
        run(
            []
        {
            window w({600, 300}, true);
            w.settle();
            const QMargins without = w.label->contentsMargins();

            w.label->set_icon(red_icon());
            w.settle();
            const QMargins with = w.label->contentsMargins();
            CHECK_EQ(with.left(), without.left() + w.label->icon_side() + notification_label::GAP);

            w.label->set_icon({});
            w.settle();
            CHECK_EQ(w.label->contentsMargins(), without);
            CHECK(w.label->icon_rect().isEmpty());

            w.label->set_icon(red_icon());
            w.settle();
            CHECK_EQ(w.label->contentsMargins(), with);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "font_change_updates_minimum")
    {
        run(
            []
        {
            window w({800, 300}, true);
            w.label->set_icon(red_icon());
            w.settle();
            const int before = w.label->icon_side();

            w.label->setStyleSheet(QString(STYLE) + "font-size: 40px;");
            w.settle();

            CHECK_GT(w.label->icon_side(), before);
            CHECK_EQ(w.label->icon_side(), font_height(*w.label));
            CHECK_EQ(gap(*w.label), notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "explicit_size_is_honored")
    {
        run(
            []
        {
            window w({600, 300}, true);
            w.label->set_icon(red_icon());
            w.label->set_icon_size(64);
            w.settle();

            CHECK_EQ(w.label->icon_side(), 64);
            CHECK_GE(w.label->height(), 64);
            CHECK(w.label->rect().contains(w.label->icon_rect()));
            CHECK_EQ(w.label->icon_room(300), 64 + notification_label::GAP);

            // Explicit sizes ignore the available height.
            w.widget.resize(600, 300);
            qobject_cast<QVBoxLayout*>(w.widget.layout())->setStretch(0, 1);
            w.settle();
            CHECK_EQ(w.label->icon_side(), 64);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "explicit_size_is_capped_by_a_fixed_height")
    {
        run(
            []
        {
            window w({600, 300}, true);
            w.label->setFixedHeight(40);
            w.label->set_icon(red_icon());
            w.label->set_icon_size(64);
            w.settle();

            CHECK_EQ(w.label->icon_side(), 40);
            CHECK(w.label->rect().contains(w.label->icon_rect()));
            CHECK_EQ(w.label->icon_room(40), 40 + notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "style_sheet_border_is_kept")
    {
        run(
            []
        {
            window w({600, 300}, true);
            w.label->setStyleSheet(QString(STYLE) + "border: 4px solid white;");
            w.label->set_icon(red_icon());
            w.settle();

            const QMargins margins = w.label->contentsMargins();
            CHECK_EQ(margins.top(), 4);
            CHECK_EQ(margins.bottom(), 4);
            CHECK_EQ(margins.right(), 4);
            CHECK_EQ(margins.left(), 4 + w.label->icon_side() + notification_label::GAP);
            CHECK_EQ(gap(*w.label), notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "size_hints_ignore_a_larger_automatic_icon")
    {
        run(
            []
        {
            window w({600, 120}, false);
            w.label->set_icon(red_icon());
            w.settle();
            CHECK_GT(w.label->icon_side(), font_height(*w.label));

            const QSize hint         = w.label->sizeHint();
            const QSize minimum_hint = w.label->minimumSizeHint();
            const int height         = w.label->heightForWidth(300);

            // The same hints as with the smallest icon.
            w.label->set_icon_size(font_height(*w.label));
            w.settle();
            CHECK_EQ(w.label->sizeHint(), hint);
            CHECK_EQ(w.label->minimumSizeHint(), minimum_hint);
            CHECK_EQ(w.label->heightForWidth(300), height);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "layout_is_stable")
    {
        run(
            []
        {
            window w(
                {300, 400},
                true,
                "A rather long notification message, that has to wrap over several lines<br/>and more"
            );
            w.label->set_icon(red_icon());
            w.settle();

            const QRect geometry   = w.label->geometry();
            const QMargins margins = w.label->contentsMargins();
            w.settle();

            CHECK_EQ(w.label->geometry(), geometry);
            CHECK_EQ(w.label->contentsMargins(), margins);
            CHECK_FALSE(w.label->text_rect().intersects(w.label->icon_rect()));
            CHECK_GE(w.label->height(), w.label->heightForWidth(w.label->width()));
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "text_change_from_the_base_class_is_followed")
    {
        run(
            []
        {
            // Just wide enough for this word beside the smallest icon.
            const QString word = "Unbreakablenotificationword";
            window w({100, 120}, false);
            w.label->set_icon(red_icon());
            w.label->ensurePolished();

            const int word_width = QFontMetrics(w.label->font()).horizontalAdvance(word);
            w.widget.resize(font_height(*w.label) + notification_label::GAP + word_width + 10, 120);
            w.settle();
            CHECK_GT(w.label->icon_side(), font_height(*w.label));

            // Not virtual: the label is not told, it catches up when painting.
            static_cast<QLabel*>(w.label)->setText(word);
            w.settle();

            CHECK_EQ(w.label->icon_side(), font_height(*w.label));
            CHECK_EQ(w.label->contentsMargins().left(), font_height(*w.label) + notification_label::GAP);
            CHECK_EQ(gap(*w.label), notification_label::GAP);
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "icon_is_rendered_where_reported")
    {
        run(
            []
        {
            window w({800, 120}, false);
            w.label->set_icon(red_icon());
            w.settle();

            const QImage image = w.label->grab().toImage();
            const qreal ratio  = image.devicePixelRatio();
            const QPoint center((w.label->icon_rect().center().toPointF() * ratio).toPoint());
            CHECK_EQ(QColor(image.pixel(center)), QColor(Qt::red));

            // The text is painted right after the gap.
            const auto [left, right] = text_ink(*w.label);
            CHECK_LE(std::abs(left - w.label->text_rect().left()), 3);
            CHECK_LE(std::abs(right - w.label->text_rect().right()), 3);
        });
    }
} // TEST_SUITE
