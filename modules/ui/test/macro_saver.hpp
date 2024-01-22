/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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
// cspell:ignore modif

#pragma once

#include "modules/ui/test/config.hpp"

#include <ui/test/helper/select.hpp>

#include <QEvent>
#include <QList>
#include <qnamespace.h>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QVector>
#include <QWidget>

#include <cstdint>
#include <optional>

/// An enumeration which represents the type of a user interaction
enum interaction_t
{
    mouse_click,
    mouse_double_click,
    mouse_drag,
    mouse_wheel,
    keyboard_click,
    keyboard_sequence,
    model_view_select,
    number_input_modification,
    helper_api
};

/// An enumeration which represents how to find a widget, @see FindStrategy
enum class find_strategy_t
{
    root,                // The object to be found is root
    active_modal_widget, // The object to be found is the active modal widget
    object_name,         // Find an object using its object name
    global_type,         // Find an object using its type from the root
    action,              // Find an object via of its action, requires to find the action
    local_type,          // Find an object using its type from the parent, requires to find the parent
    nth_child,           // Find an object using its index in its parent children list, requires to find the parent
    cant_be_found        // Error: the object can't be found
};

enum class modification_t
{
    increment,
    decrement,
    set
};

/// A structure which represents a strategy to find a specific widget
struct find_strategy
{
    find_strategy_t type;
    QString class_name;
    QString string;
    int integer;
};

/// The base class for all interaction
struct macro_interaction
{
    macro_interaction(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers
    );

    intptr_t receiver_id;
    QVector<find_strategy> how_to_find_receiver;
    Qt::KeyboardModifiers modifiers;
};

/// An interaction before the preprocessing (that is, directly caught from Qt)
struct pre_interaction : public macro_interaction
{
    pre_interaction(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type
    );

    QEvent::Type type;
};

/// An interaction after the preprocessing (ready to be translated into C++)
struct post_interaction : public macro_interaction
{
    post_interaction(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type
    );

    interaction_t type;
};

/// A mouse interaction (mouse click, mouse drag...)
struct interaction_mouse
{
    interaction_mouse(QPoint _from, QPoint _to, Qt::MouseButton _button);

    QPoint from;
    QPoint to;
    Qt::MouseButton button;
};

/// A mouse wheel interaction
struct interaction_mouse_wheel
{
    interaction_mouse_wheel(QPoint _angle_delta, QPoint _pos);

    QPoint angle_delta;
    QPoint pos;
};

/// A keyboard interaction (key press, key sequence...)
struct interaction_keyboard
{
    interaction_keyboard(Qt::Key _key, QString _sequence);

    [[nodiscard]] bool is_printable() const;

    Qt::Key key;
    QString sequence;
};

/// An interaction when selecting an item on a model-based widget
struct interaction_model_view_select
{
    explicit interaction_model_view_select(QString _name);

    QString name;
};

struct number_input_modification
{
    explicit number_input_modification(modification_t _type, double _number);

    modification_t modif_type;
    double modif_number;
};

/// A mouse interaction before the preprocessing
struct pre_interaction_mouse : public pre_interaction,
                               public interaction_mouse
{
    pre_interaction_mouse(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        QPoint _from,
        QPoint _to,
        Qt::MouseButton _button
    );
};

/// A mouse wheel interaction before the preprocessing
struct pre_interaction_mouse_wheel : public pre_interaction,
                                     public interaction_mouse_wheel
{
    pre_interaction_mouse_wheel(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        QPoint _angle_delta,
        QPoint _pos
    );
};

/// A keyboard interaction before the preprocessing
struct pre_interaction_keyboard : public pre_interaction,
                                  public interaction_keyboard
{
    pre_interaction_keyboard(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        Qt::Key _key,
        const QString& _sequence
    );
};

/// A list widget click interaction before the preprocessing
struct pre_interaction_model_view_select : public pre_interaction,
                                           public interaction_model_view_select
{
    pre_interaction_model_view_select(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        const QString& _name
    );
};

struct pre_interaction_number_input_modification : public pre_interaction,
                                                   public number_input_modification
{
    pre_interaction_number_input_modification(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        modification_t _modif_type,
        double _modif_number
    );
};

/// A mouse interaction after the preprocessing
struct post_interaction_mouse : public post_interaction,
                                public interaction_mouse
{
    post_interaction_mouse(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        QPoint _from,
        QPoint _to,
        Qt::MouseButton _button
    );
};

/// A mouse wheel interaction after the preprocessing
struct post_interaction_mouse_wheel : public post_interaction,
                                      public interaction_mouse_wheel
{
    post_interaction_mouse_wheel(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        QPoint _angle_delta,
        QPoint _pos
    );
};

/// A keyboard interaction after the preprocessing
struct post_interaction_keyboard : public post_interaction,
                                   public interaction_keyboard
{
    post_interaction_keyboard(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        Qt::Key _key,
        const QString& _sequence
    );
};

/// A list widget click interaction after the preprocessing
struct post_interaction_model_view_select : public post_interaction,
                                            public interaction_model_view_select
{
    post_interaction_model_view_select(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        const QString& _name
    );
};

struct post_interaction_number_input_modification : public post_interaction,
                                                    public number_input_modification
{
    post_interaction_number_input_modification(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        modification_t _modif_type,
        double _modif_number
    );
};

struct interaction_helper_api : public post_interaction
{
    interaction_helper_api(
        intptr_t _receiver_id,
        const QVector<find_strategy>& _how_to_find_receiver,
        QString _method_name,
        std::optional<sight::ui::test::helper::selector> _select,
        QStringList _args = {});

    QString method_name;
    std::optional<sight::ui::test::helper::selector> select;
    QStringList args;
};

/// Class which captures interactions to save them as a GuiTester-compatible GUI test C++ source file
class MODULE_UI_TEST_CLASS_API macro_saver : public QObject
{
Q_OBJECT

public:

    // Constructor. Does nothing.
    MODULE_UI_TEST_API macro_saver() = default;

    /**
     * @brief Method used by Qt when an infected widget receives an event.
     * @details If it's the creation of a child widget, it infects it as well, if it's a user interaction, it saves it
     * in its interactions vector.
     *
     * @param _target The widget being targeted by the event
     * @param _e The event which was sent to the target
     * @returns Should the event handling stop?
     */
    bool eventFilter(QObject* _target, QEvent* _e) override;

    /**
     * @brief Infects a widget and its children recursively, so that the events they will receive will be filtered with
     * @ref MacroSaver
     *
     * @param _o The widget to infect
     */
    MODULE_UI_TEST_API void infect(QObject* _o);

    /// Saves the user interactions as GuiTester-compatible GUI test C++ source files named GuiTest.cpp and GuiTest.hpp
    MODULE_UI_TEST_API void save();

private:

    std::unique_ptr<pre_interaction> create_interaction(QObject* _target, QEvent* _e);
    QVector<find_strategy> find(QObject* _o);

    static QObject* findChild(
        QObject* _o,
        const QString& _type,
        const QString& _object_name   = "",
        Qt::FindChildOptions _options = Qt::FindChildrenRecursively
    );
    static QObjectList findChildren(
        QObject* _o,
        const QString& _type,
        const QString& _object_name   = "",
        Qt::FindChildOptions _options = Qt::FindChildrenRecursively
    );

    std::vector<std::unique_ptr<pre_interaction> > m_interactions;
    QWidget* m_main_window  = nullptr;
    bool m_drag_in_progress = false;
};
