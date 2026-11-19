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

#include <ui/test/qt_application.hpp>

#include <ui/qt/slice_selector.hpp>

#include <doctest/doctest.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QToolButton>
#include <QWidget>

#include <cstdint>

using sight::ui::qt::slice_selector;

//------------------------------------------------------------------------------

namespace
{

struct selector_parts
{
    QLineEdit* label {nullptr};
    QSlider* slider {nullptr};
    QToolButton* toggle {nullptr};
};

} // namespace

//------------------------------------------------------------------------------

/// The parts of a selector named after `_name` or, without a name, whatever their names.
static selector_parts parts_of(const slice_selector& _selector, const QString& _name = {})
{
    const auto name = [&_name](const char* const _part)
                      {
                          return _name.isEmpty() ? QString() : _name + "/" + _part;
                      };
    const selector_parts parts {
        .label  = _selector.findChild<QLineEdit*>(name("label")),
        .slider = _selector.findChild<QSlider*>(name("slider")),
        .toggle = _selector.findChild<QToolButton*>(name("label/toggle"))
    };
    REQUIRE(parts.label != nullptr);
    REQUIRE(parts.slider != nullptr);
    REQUIRE(parts.toggle != nullptr);
    CHECK(parts.toggle->parentWidget() == parts.label);
    return parts;
}

//------------------------------------------------------------------------------

static void check_unnamed(const selector_parts& _parts)
{
    CHECK(_parts.label->objectName().isEmpty());
    CHECK(_parts.slider->objectName().isEmpty());
    CHECK(_parts.toggle->objectName().isEmpty());
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::ui::qt::slice_selector")
{
    TEST_CASE("parts_follow_both_selector_variants")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        QWidget parent;
        slice_selector index(false, false, std::uint8_t {1}, &parent);
        slice_selector position(false, false, 0.0, &parent);
        const selector_parts index_parts = parts_of(index);
        check_unnamed(index_parts);
        check_unnamed(parts_of(position));

        // Distinct owners may coexist and change names independently.
        index.setObjectName("index");
        position.setObjectName("position");
        static_cast<void>(parts_of(index, "index"));
        static_cast<void>(parts_of(position, "position"));
        CHECK(parent.findChildren<QLineEdit*>("index/label").size() == 1);
        CHECK(parent.findChildren<QLineEdit*>("position/label").size() == 1);

        index.setObjectName("renamed");
        static_cast<void>(parts_of(index, "renamed"));
        static_cast<void>(parts_of(position, "position"));
        CHECK(parent.findChild<QLineEdit*>("index/label") == nullptr);
        CHECK(parent.findChild<QToolButton*>("index/label/toggle") == nullptr);

        index.setObjectName({});
        check_unnamed(index_parts);
        static_cast<void>(parts_of(position, "position"));
    }

    //------------------------------------------------------------------------------

    TEST_CASE("named_parts_keep_their_behaviour")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        slice_selector selector(false, false, std::uint8_t {1});
        selector.setObjectName("negato_slicer_srv");
        selector.set_slice_range(0, 133);
        selector.set_slice_value(67);
        selector.set_image_info(0., 1.6);
        selector.set_prefix("S");

        const selector_parts parts = parts_of(selector, "negato_slicer_srv");
        CHECK(parts.label->text() == "67 / 133");
        CHECK(parts.slider->value() == 67);

        int changes = 0;
        selector.set_change_label_callback([&changes]{++changes;});
        parts.toggle->click();
        CHECK(changes == 1);
        CHECK(parts.label->text() == "S : 107.20 mm");
    }

    //------------------------------------------------------------------------------

    TEST_CASE("position_parts_keep_their_behaviour")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        slice_selector selector(false, false, 0.0);
        selector.setObjectName("negato_slicer_srv");
        selector.set_position_range(0, 133);
        selector.set_position_value(67);
        selector.set_image_info(0., 1.6);
        selector.set_prefix("S");
        selector.set_position_text(67);

        const selector_parts parts = parts_of(selector, "negato_slicer_srv");
        CHECK(parts.label->text() == "S : 107.20 mm");
        CHECK(parts.slider->value() == 67);

        // Toggling the position display still reports the index.
        int changes = 0;
        selector.set_change_label_callback([&changes]{++changes;});
        parts.toggle->click();
        CHECK(changes == 1);
        CHECK(parts.label->text() == "67 / 133");
    }

    //------------------------------------------------------------------------------

    TEST_CASE("a_crowded_selector_keeps_room_for_its_slider")
    {
        const sight::ui::test::qt_application application {"ui_qt_test"};
        constexpr int handle = 14;
        for(const bool position : {false, true})
        {
            CAPTURE(position);

            // As the application style sheets do: a handle width without a min-width gives a slider no minimum length.
            QWidget window;
            window.setStyleSheet(QString("QSlider::handle { width: %1px; height: %1px; }").arg(handle));
            auto* const selector = position ? new slice_selector(true, false, 0.0, &window)
                                            : new slice_selector(true, false, std::uint8_t {1}, &window);
            auto* const layout = new QHBoxLayout(&window);
            layout->setContentsMargins(0, 0, 0, 0);
            layout->addWidget(selector);

            const selector_parts parts = parts_of(*selector);
            const auto* const axis     = selector->findChild<QComboBox*>();
            REQUIRE(axis != nullptr);

            // Less room than the other parts need: the slider must not be the one left with nothing.
            window.setFixedWidth(axis->minimumSizeHint().width() + parts.label->minimumSizeHint().width());
            window.show();
            layout->activate();

            CHECK(parts.slider->width() > 2 * handle);
        }
    }
}
