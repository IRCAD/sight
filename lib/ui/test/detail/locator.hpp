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

#pragma once

#include <sight/ui/test/config.hpp>

#include <core/runtime/types.hpp>

#include <QAction>
#include <QWidget>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sight::ui::test::detail
{

/// One object name in a locator, optionally narrowed to a 1-based role occurrence.
struct locator_segment
{
    /// The name as composed object names store it, escapes kept: compared verbatim with `objectName`.
    std::string object_name;
    /// The XML components of the name, split on unescaped '/' and unescaped.
    std::vector<std::string> components;
    /// Only on the last segment.
    std::optional<std::size_t> role_occurrence;
};

/// The kind of XML element that declares a locator name.
enum class locator_catalog_name_kind : std::uint8_t
{
    service,       ///< The uid of a service.
    view,          ///< The sid or wid of a view.
    menu_item,     ///< An item of a menu or toolbar.
    parameter_key, ///< A key of a settings service.
    dialog,        ///< The dialog of a service.
    dialog_role    ///< A field of the dialog of a service.
};

/// One XML name and the configuration that declared it.
struct locator_catalog_entry
{
    std::string configuration_id;                                        ///< The configuration that declares the name.
    std::vector<std::string> components;                                 ///< The XML components of the name.
    locator_catalog_name_kind kind {locator_catalog_name_kind::service}; ///< The element that declares it.
    std::string service_type;                                            ///< The service type, for a service name.
};

/// Names declared in application configuration XML.
class SIGHT_UI_TEST_CLASS_API locator_catalog
{
public:

    /**
     * @brief Adds service, view, menu-item and parameter names from one configuration tree.
     *
     * @param _configuration_id the id of the configuration.
     * @param _configuration    its XML tree.
     */
    SIGHT_UI_TEST_API void add_configuration(
        std::string_view _configuration_id,
        const core::runtime::config_t& _configuration
    );

    /**
     * @brief Adds several configurations, whose ids must be unique.
     *
     * @param _configurations the ids and the XML trees.
     */
    SIGHT_UI_TEST_API void add_configurations(
        const std::vector<std::pair<std::string, core::runtime::config_t> >& _configurations
    );

    /**
     * @brief Collects every registered application configuration from the loaded modules.
     *
     * @return the catalog.
     */
    [[nodiscard]] SIGHT_UI_TEST_API static locator_catalog from_loaded_modules();

    /**
     * @brief Every name the catalog holds.
     *
     * @return the names.
     */
    [[nodiscard]] SIGHT_UI_TEST_API const std::vector<locator_catalog_entry>& entries() const noexcept;

    /**
     * @brief Checks exact XML components; role and part names are added at runtime.
     *
     * @param _segment the segment of a locator.
     * @return whether the XML declares the name.
     */
    [[nodiscard]] SIGHT_UI_TEST_API bool contains(const locator_segment& _segment) const;

    /**
     * @brief Accepts a known XML name, or a runtime role or part below a known service that is not a menu.
     *
     * @param _segment the segment of a locator.
     * @return whether the name may exist.
     */
    [[nodiscard]] SIGHT_UI_TEST_API bool accepts(const locator_segment& _segment) const;

    /**
     * @brief Returns XML names near the requested segment, preferring the same parent component.
     *
     * @param _segment the segment of a locator.
     * @return the names, closest first.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::vector<std::string> candidates(const locator_segment& _segment) const;

    /**
     * @brief Tells whether a configuration declares an object of this uid, or passes one under this parameter name.
     *
     * @param _uid the uid or parameter name.
     * @return whether the object is known.
     */
    [[nodiscard]] SIGHT_UI_TEST_API bool knows_object(std::string_view _uid) const;

private:

    std::vector<locator_catalog_entry> m_entries;
    std::set<std::string, std::less<> > m_objects;
};

/**
 * @brief Parses the string form of a GUI locator into its scoped object names and the filters that end it.
 *
 * `>>` separates the scopes, from the outermost; an object name keeps its slashes. The builders of locator.hpp write
 * these strings.
 *
 * @code{.cpp}
 * "main_scene_view >> scene_srv/view"
 * "@dialog >> name >> @type(QSlider) >> @text(Open) >> @hidden"
 * @endcode
 */
class SIGHT_UI_TEST_CLASS_API locator
{
public:

    /**
     * @brief Parses a locator; a malformed one throws std::invalid_argument, which says why.
     *
     * @param _expression the string form.
     */
    SIGHT_UI_TEST_API explicit locator(std::string_view _expression);

    /**
     * @brief The parsed object-name scopes, from the outermost; empty when dialog() alone is the scope.
     *
     * @return the scopes.
     */
    [[nodiscard]] SIGHT_UI_TEST_API const std::vector<locator_segment>& segments() const noexcept;

    /**
     * @brief Tells whether the locator looks inside the innermost modal dialog.
     *
     * @return true for a locator that starts with dialog().
     */
    [[nodiscard]] SIGHT_UI_TEST_API bool in_modal_dialog() const noexcept;

    /**
     * @brief Resolves the widgets by exact name and scope, on the GUI thread; use the result only in that call.
     *
     * @param _include_hidden whether hidden widgets match too.
     * @return the matching widgets.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::vector<QWidget*> resolve_widgets(bool _include_hidden = false) const;

    /**
     * @brief Counts the hidden matches in scope, on the GUI thread.
     *
     * An ordinal applies only to visible widgets, and is ignored here.
     *
     * @return the number of hidden matches.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::size_t count_hidden_widgets() const;

    /**
     * @brief Resolves the actions by exact name and scope, on the GUI thread; use the result only in that call.
     *
     * @return the matching actions.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::vector<QAction*> resolve_actions() const;

private:

    /// Whether a widget passes the filters of the locator, whatever its name.
    [[nodiscard]] bool passes_filters(const QWidget& _widget) const;

    std::vector<locator_segment> m_segments;
    bool m_in_modal_dialog {false};
    std::optional<std::string> m_type;
    std::optional<std::string> m_text;
    bool m_hidden {false};
};

} // namespace sight::ui::test::detail
