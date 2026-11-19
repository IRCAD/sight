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

#include <ui/test/detail/locator.hpp>
#include <ui/test/locator.hpp>

#include <doctest/doctest.h>

#include <boost/property_tree/xml_parser.hpp>

#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace sight::ui::test::detail
{

TEST_SUITE("sight::ui::test::locator")
{
    TEST_CASE("parses_a_name_and_a_scope_chain")
    {
        const locator single("top_toolbar_left/Load Files");
        REQUIRE(single.segments().size() == 1);
        CHECK(single.segments()[0].object_name == "top_toolbar_left/Load Files");
        CHECK(single.segments()[0].components == std::vector<std::string> {"top_toolbar_left", "Load Files"});
        CHECK_FALSE(single.segments()[0].role_occurrence.has_value());

        const locator scoped("axial_scene_view  >> annotation_srv/slider#2");
        REQUIRE(scoped.segments().size() == 2);
        CHECK(scoped.segments()[0].object_name == "axial_scene_view");
        CHECK_FALSE(scoped.segments()[0].role_occurrence.has_value());
        CHECK(scoped.segments()[1].object_name == "annotation_srv/slider");
        REQUIRE(scoped.segments()[1].role_occurrence.has_value());
        CHECK(*scoped.segments()[1].role_occurrence == 2);
    }

    TEST_CASE("keeps_escapes_in_the_object_name")
    {
        // Composed object names store their components escaped, so the name is compared verbatim.
        const locator escaped(R"(top_toolbar_left/Show\/hide mesh\#1\\x#3)");
        REQUIRE(escaped.segments().size() == 1);
        CHECK(escaped.segments()[0].object_name == R"(top_toolbar_left/Show\/hide mesh\#1\\x)");
        const std::vector<std::string> components {"top_toolbar_left", R"(Show/hide mesh#1\x)"};
        CHECK(escaped.segments()[0].components == components);
        REQUIRE(escaped.segments()[0].role_occurrence.has_value());
        CHECK(*escaped.segments()[0].role_occurrence == 3);
    }

    TEST_CASE("rejects_malformed_scopes_and_escapes")
    {
        CHECK_THROWS_AS(locator(""), std::invalid_argument);
        CHECK_THROWS_AS(locator(" >> child"), std::invalid_argument);
        CHECK_THROWS_AS(locator("parent >> "), std::invalid_argument);
        CHECK_THROWS_AS(locator("parent >>> child"), std::invalid_argument);
        CHECK_THROWS_AS(locator("name\\"), std::invalid_argument);
        CHECK_THROWS_AS(locator("name\\q"), std::invalid_argument);
        CHECK_THROWS_AS(locator("name\tx"), std::invalid_argument);
    }

    TEST_CASE("rejects_malformed_name_parts")
    {
        CHECK_THROWS_AS(locator("/name"), std::invalid_argument);
        CHECK_THROWS_AS(locator("uid/"), std::invalid_argument);
        CHECK_THROWS_AS(locator("uid//part"), std::invalid_argument);
        CHECK_THROWS_AS(locator("uid/ part"), std::invalid_argument);
        CHECK_THROWS_AS(locator("uid/part "), std::invalid_argument);
        CHECK_THROWS_AS(locator(" uid/part"), std::invalid_argument);
        CHECK_THROWS_AS(locator("uid/part #2"), std::invalid_argument);
    }

    TEST_CASE("validates_item_paths")
    {
        const item_locator target = item("tree", R"(Root/Child\/one)");
        CHECK(target.view_expression() == "tree");

        CHECK_THROWS_AS(static_cast<void>(item("tree", "")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(item("tree", "/Root")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(item("tree", "Root//Child")), std::invalid_argument);
        CHECK_THROWS_AS(static_cast<void>(item("tree", R"(Root\q)")), std::invalid_argument);
    }

    TEST_CASE("rejects_misplaced_role_occurrences")
    {
        CHECK_THROWS_AS(locator("name#"), std::invalid_argument);
        CHECK_THROWS_AS(locator("name#0"), std::invalid_argument);
        CHECK_THROWS_AS(locator("name#2#3"), std::invalid_argument);
        CHECK_THROWS_AS(locator("name#2/part"), std::invalid_argument);
        CHECK_THROWS_AS(locator("#2"), std::invalid_argument);
        CHECK_THROWS_AS(locator("scope#2 >> name"), std::invalid_argument);
    }

    TEST_CASE("catalogs_names_from_configuration_xml")
    {
        std::istringstream xml(
            R"(<config>
                <service uid="toolbar" type="sight::module::ui::toolbar">
                    <gui><layout><menu_item name="Show/hide mesh"/></layout></gui>
                    <registry><menu_item sid="mesh_action"/></registry>
                </service>
                <service uid="root">
                    <registry>
                        <view sid="scene"/>
                        <view wid="child_view"/>
                    </registry>
                </service>
                <service uid="legacy_toolbar">
                    <gui><layout><menuItem name="old action"/></layout></gui>
                </service>
                <service uid="sections" type="sight::module::ui::view">
                    <gui>
                        <layout type="sight::ui::layout::toolbox">
                            <view caption="First"/>
                            <view caption="Second"/>
                        </layout>
                    </gui>
                    <registry>
                        <view sid="first_section"/>
                        <view wid="second_section"/>
                    </registry>
                </service>
            </config>)"
        );
        core::runtime::config_t parsed_xml;
        boost::property_tree::read_xml(xml, parsed_xml);

        locator_catalog catalog;
        catalog.add_configuration("sample", parsed_xml.get_child("config"));

        CHECK(catalog.contains(locator(R"(toolbar/Show\/hide mesh)").segments().front()));
        CHECK(catalog.contains(locator("scene").segments().front()));
        CHECK(catalog.contains(locator("child_view").segments().front()));
        CHECK(catalog.contains(locator("legacy_toolbar/old action").segments().front()));
        CHECK_FALSE(catalog.contains(locator("toolbar/mesh_action").segments().front()));
        CHECK_FALSE(catalog.accepts(locator("toolbar/Show").segments().front()));
        CHECK(
            catalog.candidates(locator("toolbar/Show").segments().front())
            == std::vector<std::string> {R"(toolbar/Show\/hide mesh)"
            });
        CHECK(catalog.accepts(locator("root/slider").segments().front()));

        // A toolbox names the header of each section after its view, and only there.
        CHECK(catalog.contains(locator("first_section/toggle").segments().front()));
        CHECK(catalog.contains(locator("second_section/toggle").segments().front()));
        CHECK_FALSE(catalog.contains(locator("scene/toggle").segments().front()));
        CHECK_FALSE(catalog.accepts(locator("first_sectio/toggle").segments().front()));
        CHECK(catalog.entries().front().configuration_id == "sample");
    }

    TEST_CASE("catalogs_settings_and_preference_keys_and_rejects_unknown_ones")
    {
        std::istringstream xml(
            R"(<config>
                <service uid="settings" type="sight::module::ui::qt::settings">
                    <inout key="input" uid="bound_object"/>
                    <item data="${settings}.fallback_value" name="Fallback"/>
                    <item data="${input}" name="Bound object"/>
                    <item data="${settings}.unused" key="explicit_key" name="Explicit"/>
                </service>
                <service uid="preferences" type="sight::module::ui::qt::preferences_configuration">
                    <preference><key>saved_value</key></preference>
                </service>
            </config>)"
        );
        core::runtime::config_t parsed_xml;
        boost::property_tree::read_xml(xml, parsed_xml);

        locator_catalog catalog;
        catalog.add_configuration("settings", parsed_xml.get_child("config"));

        CHECK(catalog.contains(locator("settings/fallback_value").segments().front()));
        CHECK(catalog.contains(locator("settings/bound_object").segments().front()));
        CHECK(catalog.contains(locator("settings/explicit_key").segments().front()));
        CHECK(catalog.accepts(locator("settings/explicit_key/slider").segments().front()));
        CHECK_FALSE(catalog.accepts(locator("settings/missing").segments().front()));

        CHECK(catalog.contains(locator("preferences/dialog/saved_value").segments().front()));
        CHECK(catalog.accepts(locator("preferences/dialog/saved_value/line_edit").segments().front()));
        CHECK(catalog.contains(locator("preferences/dialog/ok").segments().front()));
        CHECK_FALSE(catalog.accepts(locator("preferences/dialog/missing").segments().front()));
    }

    TEST_CASE("rejects_invalid_names_in_configuration_xml")
    {
        for(const std::string_view invalid_name : {"bad>>name", " padded"})
        {
            std::istringstream xml("<config><service uid=\"" + std::string(invalid_name) + "\"/></config>");
            core::runtime::config_t parsed_xml;
            boost::property_tree::read_xml(xml, parsed_xml);

            locator_catalog catalog;
            CHECK_THROWS_AS(catalog.add_configuration("broken", parsed_xml.get_child("config")), std::invalid_argument);
        }
    }
} // TEST_SUITE

TEST_SUITE("sight::ui::test::locator_builder")
{
    TEST_CASE("writes_the_same_scope_as_a_string")
    {
        CHECK((loc("view") >> "service/label").expression() == "view >> service/label");
        CHECK(locator(loc("view") >> "service/label").segments().size() == 2);
    }

    TEST_CASE("fails_where_a_wrong_locator_is_written")
    {
        CHECK_THROWS_AS(std::ignore = loc(""), std::invalid_argument);
        CHECK_THROWS_AS(std::ignore = (loc("a#2") >> "b"), std::invalid_argument);
        CHECK_THROWS_AS(std::ignore = (loc("a") >> ""), std::invalid_argument);
        CHECK_THROWS_AS(std::ignore = (loc("a").hidden() >> "b"), std::invalid_argument);
        CHECK_THROWS_AS(locator("a >> @dialog"), std::invalid_argument);
        CHECK_THROWS_AS(locator("@hidden"), std::invalid_argument);
        CHECK_THROWS_AS(locator("a >> @nope"), std::invalid_argument);
        CHECK_THROWS_AS(locator("a >> @type()"), std::invalid_argument);
        CHECK_THROWS_AS(locator("a >> @text(open"), std::invalid_argument);
    }

    TEST_CASE("escapes_the_xml_components_of_a_name")
    {
        CHECK(name("properties1_srv", "nonlin", "slider") == "properties1_srv/nonlin/slider");
        CHECK(menu_item("top_toolbar_left", "Show/hide mesh") == R"(top_toolbar_left/Show\/hide mesh)");
        CHECK(name(std::string("a#1"), "b\\c") == R"(a\#1/b\\c)");

        const locator parsed(menu_item("top_toolbar_left", "Show/hide mesh"));
        REQUIRE(parsed.segments().size() == 1);
        CHECK(parsed.segments().front().components == std::vector<std::string> {"top_toolbar_left", "Show/hide mesh"});
    }

    TEST_CASE("keeps_a_text_filter_whole")
    {
        // A separator, a closing parenthesis and a backslash in a text are only a text.
        const std::string awkward = R"(Open >> (now) \ here)";
        const locator parsed(loc("a").text(awkward));
        CHECK(parsed.segments().size() == 1);
    }
} // TEST_SUITE

} // namespace sight::ui::test::detail
