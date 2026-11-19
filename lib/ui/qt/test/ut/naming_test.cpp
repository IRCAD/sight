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

#include <ui/qt/naming.hpp>

#include <QObject>
#include <QProperty>
#include <QVariant>

#include <doctest/doctest.h>

#include <memory>
#include <stdexcept>

namespace sight::ui::qt::ut
{

TEST_SUITE("sight::ui::qt::naming")
{
    TEST_CASE("tells_a_snake_case_part")
    {
        for(const char16_t* const part : {u"a", u"ok", u"new_group", u"board_width", u"k1", u"axis_2d"})
        {
            CAPTURE(QString::fromUtf16(part).toStdString());
            CHECK(detail::is_part(part));
        }

        // Captions, camelCase, paths and ordinals are not parts.
        for(const char16_t* const part :
            {u"", u"_a", u"a_", u"a__b", u"New Group", u"isCalibrated", u"A", u"1st", u"a/b", u"a#2", u"a-b"})
        {
            CAPTURE(QString::fromUtf16(part).toStdString());
            CHECK_FALSE(detail::is_part(part));
        }
    }

    //------------------------------------------------------------------------------

    TEST_CASE("accepts_a_name_from_a_sanctioned_path_until_it_changes")
    {
        QObject xml;
        detail::set_name(xml, "top_toolbar_view/Snapshot");
        CHECK(xml.objectName() == "top_toolbar_view/Snapshot");
        CHECK(detail::has_sanctioned_name(xml));

        QObject role;
        detail::set_name(role, "annotation_srv/button");
        CHECK(detail::has_sanctioned_name(role));

        // A later rename, the same text set by hand again aside, voids the record.
        xml.setObjectName("top_toolbar_view/Other");
        CHECK_FALSE(detail::has_sanctioned_name(xml));
        xml.setObjectName("top_toolbar_view/Snapshot");
        CHECK(detail::has_sanctioned_name(xml));
    }

    //------------------------------------------------------------------------------

    TEST_CASE("refuses_a_name_set_by_hand")
    {
        QObject object;
        object.setObjectName("annotation_srv/New Group");
        CHECK_FALSE(detail::has_sanctioned_name(object));

        // A property set by hand under any name is not a record.
        object.setProperty("_sight_name_record", QStringLiteral("annotation_srv/New Group"));
        CHECK_FALSE(detail::has_sanctioned_name(object));

        // Neither is an empty name, even recorded.
        QObject unnamed;
        detail::set_name(unnamed, {});
        CHECK_FALSE(detail::has_sanctioned_name(unnamed));
    }

    //------------------------------------------------------------------------------

    TEST_CASE("names_a_part_after_its_owner")
    {
        QObject object;
        detail::set_part_name(object, "calibrationInfoEditorSrv", u"nb_captures");
        CHECK(object.objectName() == "calibrationInfoEditorSrv/nb_captures");
        CHECK(detail::has_sanctioned_name(object));
    }

    //------------------------------------------------------------------------------

    TEST_CASE("refuses_a_part_that_is_not_snake_case")
    {
        QObject object;
        object.setObjectName("before");
        CHECK_THROWS_AS(detail::set_part_name(object, "annotation_srv", u"New Group"), std::invalid_argument);
        CHECK_THROWS_AS(detail::set_part_name(object, "annotation_srv", QStringView()), std::invalid_argument);

        // Nothing changed: the object neither took the name nor a record.
        CHECK(object.objectName() == "before");
        CHECK_FALSE(detail::has_sanctioned_name(object));

        QObject owner;
        CHECK_THROWS_AS(detail::bind_part_name(object, owner, u"binButton"), std::invalid_argument);
        CHECK_FALSE(object.bindableObjectName().hasBinding());
    }

    //------------------------------------------------------------------------------

    TEST_CASE("binds_a_part_to_the_name_of_its_owner")
    {
        QObject owner;
        QObject part;
        detail::bind_part_name(part, owner, u"toggle");

        // No name while the owner has none: nothing to report but a missing name.
        CHECK(part.objectName().isEmpty());
        CHECK_FALSE(detail::has_sanctioned_name(part));

        owner.setObjectName("section_widget/section");
        CHECK(part.objectName() == "section_widget/section/toggle");
        CHECK(detail::has_sanctioned_name(part));

        // It follows every rename of its owner.
        owner.setObjectName("other/section");
        CHECK(part.objectName() == "other/section/toggle");
        CHECK(detail::has_sanctioned_name(part));
    }

    //------------------------------------------------------------------------------

    TEST_CASE("voids_a_bound_part_renamed_by_hand_or_without_owner")
    {
        auto owner = std::make_unique<QObject>();
        owner->setObjectName("section_widget/section");
        QObject renamed;
        detail::bind_part_name(renamed, *owner, u"toggle");

        // Setting a name breaks the binding, and the record with it.
        renamed.setObjectName("section_widget/section/Toggle me");
        CHECK_FALSE(renamed.bindableObjectName().hasBinding());
        CHECK_FALSE(detail::has_sanctioned_name(renamed));

        QObject orphan;
        detail::bind_part_name(orphan, *owner, u"toggle");
        CHECK(detail::has_sanctioned_name(orphan));
        owner.reset();
        CHECK_FALSE(detail::has_sanctioned_name(orphan));
    }
}

} // namespace sight::ui::qt::ut
