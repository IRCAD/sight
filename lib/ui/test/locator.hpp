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

#include <QAbstractItemView>
#include <QModelIndex>
#include <QObject>

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

namespace sight::ui::test
{

/**
 * @brief Writes a locator, checked at once and usable wherever a verb takes one.
 *
 * @code{.cpp}
 * ui.click(loc("main_scene_view") >> "scene_srv/view");
 * ui.click(dialog().type<QPushButton>().text("OK"));
 * @endcode
 */
class SIGHT_UI_TEST_CLASS_API locator_expression
{
public:

    /**
     * @brief Parses a locator at once: a typo fails where it is written, not at the first verb.
     *
     * @param _expression the string form.
     */
    SIGHT_UI_TEST_API explicit locator_expression(std::string _expression);

    /**
     * @brief Narrows to a descendant, as `>>` does in a string.
     *
     * @code{.cpp}
     * ui.click(dialog() >> "fileTypeCombo");
     * @endcode
     *
     * @param _name the object name of the descendant.
     * @return the narrower locator.
     */
    [[nodiscard]] SIGHT_UI_TEST_API locator_expression operator>>(std::string_view _name) const;

    /**
     * @brief Keeps the objects of this class or a subclass, for widgets outside the naming contract.
     *
     * @code{.cpp}
     * ui.click(dialog().type<QPushButton>().text("OK"));
     * @endcode
     *
     * @return the filtered locator.
     */
    template<std::derived_from<QObject> Object>
    [[nodiscard]] locator_expression type() const
    {
        return type(Object::staticMetaObject.className());
    }

    /**
     * @brief Keeps the widgets showing exactly this text, for widgets outside the naming contract, such as Qt's
     * dialogs.
     *
     * @code{.cpp}
     * ui.click(dialog().type<QPushButton>().text("OK"));
     * @endcode
     *
     * @param _text the exact, case-sensitive text.
     * @return the filtered locator.
     */
    [[nodiscard]] SIGHT_UI_TEST_API locator_expression text(std::string_view _text) const;

    /**
     * @brief Keeps the widgets that are not visible, which an action would refuse: for read() and expect().
     *
     * @code{.cpp}
     * ui.expect(loc("parameter_view").hidden()).hidden();
     * @endcode
     *
     * @return the filtered locator.
     */
    [[nodiscard]] SIGHT_UI_TEST_API locator_expression hidden() const;

    /**
     * @brief The string form.
     *
     * @return the string, which lives as long as this object, so as the verb it is passed to.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::string_view expression() const noexcept;

    /**
     * @brief The string form, implicit on purpose: each verb takes a locator as a string_view.
     *
     * @return the string form.
     */
    // NOLINTNEXTLINE(google-explicit-constructor)
    SIGHT_UI_TEST_API operator std::string_view() const noexcept;

private:

    [[nodiscard]] SIGHT_UI_TEST_API locator_expression type(std::string_view _class_name) const;

    std::string m_expression;
};

/**
 * @brief Starts a locator from a name or a scoped string.
 *
 * @code{.cpp}
 * ui.click(loc("main_scene_view") >> "scene_srv/view");
 * @endcode
 *
 * @param _expression the name, or a scoped string.
 * @return the locator.
 */
[[nodiscard]] SIGHT_UI_TEST_API locator_expression loc(std::string_view _expression);

/**
 * @brief The innermost modal dialog, or a scope inside it.
 *
 * @code{.cpp}
 * ui.select(dialog() >> "fileTypeCombo", "PNG");
 * ui.close(dialog());
 * @endcode
 *
 * @return the locator.
 */
[[nodiscard]] SIGHT_UI_TEST_API locator_expression dialog();

namespace detail
{

/// The components joined by `/`, each escaped: name() writes it.
[[nodiscard]] SIGHT_UI_TEST_API std::string compose_name(std::initializer_list<std::string_view> _components);

} // namespace detail

/**
 * @brief Writes a name from its XML components, escaping what the locator syntax reserves: `/`, `#` and `\\`.
 *
 * @code{.cpp}
 * ui.click(name("top_toolbar_view", "Show/hide mesh"));
 * @endcode
 *
 * @param _components the XML components, unescaped.
 * @return the object name.
 */
// *INDENT-OFF* uncrustify breaks this requires clause.
template<std::convertible_to<std::string_view>... Components>
requires(sizeof...(Components) > 0)
[[nodiscard]] std::string name(const Components&... _components)
{
    return detail::compose_name({std::string_view(_components)...});
}
// *INDENT-ON*

/**
 * @brief The name of a menu item, or of a toolbar action.
 *
 * @code{.cpp}
 * ui.set(menu_item("annotation_tools_toolbar", "Add / edit distance"), true);
 * @endcode
 *
 * @param _menu the uid of the menu or toolbar.
 * @param _item the name of the item in its XML.
 * @return the object name.
 */
[[nodiscard]] SIGHT_UI_TEST_API std::string menu_item(std::string_view _menu, std::string_view _item);

class item_locator;

/**
 * @brief Builds an item locator from the stable displayed text of each level.
 *
 * @code{.cpp}
 * ui.select(item("series_list_srv", "Patient/Study/CT"));
 * @endcode
 *
 * @param _view the locator of the item view.
 * @param _path the displayed texts, one per level, separated by `/`.
 * @return the locator.
 */
[[nodiscard]] inline item_locator item(std::string_view _view, std::string_view _path);

/**
 * @brief Builds an item locator from zero-based rows, one per level, when the order is the subject.
 *
 * "Remove the first capture", whose text changes. Its failure lists nothing to choose from: prefer item() otherwise.
 *
 * @code{.cpp}
 * ui.select(item_at("intrinsicCameraView/list", "0"));
 * @endcode
 *
 * @param _view the locator of the item view.
 * @param _rows the rows, one per level, separated by `/`.
 * @return the locator.
 */
[[nodiscard]] inline item_locator item_at(std::string_view _view, std::string_view _rows);

/**
 * @brief Locates a model item of a view by its displayed texts, one per level, and an optional column.
 *
 * @code{.cpp}
 * ui.select(item("series_list_srv", "Patient/Study/CT"));
 * ui.set(item("organ_list", "Liver").column(1), QColor(Qt::red));
 * @endcode
 */
class SIGHT_UI_TEST_CLASS_API item_locator
{
public:

    /**
     * @brief Targets a column of the item.
     *
     * @code{.cpp}
     * ui.set(item("organ_list", "Liver").column(1), QColor(Qt::red));
     * @endcode
     *
     * @param _column the zero-based model column.
     * @return a copy targeting that column.
     */
    [[nodiscard]] SIGHT_UI_TEST_API item_locator column(std::size_t _column) const;

    /**
     * @brief The locator of the item view.
     *
     * @return the locator.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::string_view view_expression() const noexcept;

    /**
     * @brief The number of components in the item path.
     *
     * @return the number of levels.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::size_t depth() const noexcept;

    /**
     * @brief Resolves the matching indexes, on the GUI thread.
     *
     * @param _view the item view.
     * @return the indexes; empty when the item is not present now.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::vector<QModelIndex> matching_indexes(QAbstractItemView& _view) const;

    /**
     * @brief Resolves a prefix of the path; a partial path resolves to column zero.
     *
     * @param _view  the item view.
     * @param _depth the one-based number of levels.
     * @return the indexes.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::vector<QModelIndex> matching_prefix(
        QAbstractItemView& _view,
        std::size_t _depth
    ) const;

private:

    friend item_locator item(std::string_view _view, std::string_view _path);
    friend item_locator item_at(std::string_view _view, std::string_view _rows);

    /// Displayed texts, or zero-based rows; `\\/` is a slash of a text.
    SIGHT_UI_TEST_API item_locator(std::string_view _view, std::string_view _path, bool _by_row);

    std::string m_view_expression;
    std::vector<std::string> m_path;
    std::size_t m_column {0};
    bool m_by_row {false};
};

//------------------------------------------------------------------------------

inline item_locator item(std::string_view _view, std::string_view _path)
{
    return {_view, _path, false};
}

//------------------------------------------------------------------------------

inline item_locator item_at(std::string_view _view, std::string_view _rows)
{
    return {_view, _rows, true};
}

} // namespace sight::ui::test
