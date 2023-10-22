/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <ui/testCore/helper/Select.hpp>

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
    MOUSE_CLICK,
    MOUSE_DOUBLE_CLICK,
    MOUSE_DRAG,
    MOUSE_WHEEL,
    KEYBOARD_CLICK,
    KEYBOARD_SEQUENCE,
    MODEL_VIEW_SELECT,
    NUMBER_INPUT_MODIFICATION,
    HELPER_API
};

/// An enumeration which represents how to find a widget, @see FindStrategy
enum class find_strategy_t
{
    ROOT,                // The object to be found is root
    ACTIVE_MODAL_WIDGET, // The object to be found is the active modal widget
    OBJECT_NAME,         // Find an object using its object name
    GLOBAL_TYPE,         // Find an object using its type from the root
    ACTION,              // Find an object via of its action, requires to find the action
    LOCAL_TYPE,          // Find an object using its type from the parent, requires to find the parent
    NTH_CHILD,           // Find an object using its index in its parent children list, requires to find the parent
    CANT_BE_FOUND        // Error: the object can't be found
};

enum class modification_t
{
    INCREMENT,
    DECREMENT,
    SET
};

/// A structure which represents a strategy to find a specific widget
struct FindStrategy
{
    find_strategy_t type;
    QString className;
    QString string;
    int integer;
};

/// The base class for all interaction
struct MacroInteraction
{
    MacroInteraction(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers
    );

    intptr_t receiverId;
    QVector<FindStrategy> howToFindReceiver;
    Qt::KeyboardModifiers modifiers;
};

/// An interaction before the preprocessing (that is, directly caught from Qt)
struct PreInteraction : public MacroInteraction
{
    PreInteraction(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type
    );

    QEvent::Type type;
};

/// An interaction after the preprocessing (ready to be translated into C++)
struct PostInteraction : public MacroInteraction
{
    PostInteraction(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type
    );

    interaction_t type;
};

/// A mouse interaction (mouse click, mouse drag...)
struct InteractionMouse
{
    InteractionMouse(QPoint _from, QPoint _to, Qt::MouseButton _button);

    QPoint from;
    QPoint to;
    Qt::MouseButton button;
};

/// A mouse wheel interaction
struct InteractionMouseWheel
{
    InteractionMouseWheel(QPoint _angle_delta, QPoint _pos);

    QPoint angleDelta;
    QPoint pos;
};

/// A keyboard interaction (key press, key sequence...)
struct InteractionKeyboard
{
    InteractionKeyboard(Qt::Key _key, QString _sequence);

    [[nodiscard]] bool isPrintable() const;

    Qt::Key key;
    QString sequence;
};

/// An interaction when selecting an item on a model-based widget
struct InteractionModelViewSelect
{
    explicit InteractionModelViewSelect(QString _name);

    QString name;
};

struct NumberInputModification
{
    explicit NumberInputModification(modification_t _type, double _number);

    modification_t modifType;
    double modifNumber;
};

/// A mouse interaction before the preprocessing
struct PreInteractionMouse : public PreInteraction,
                             public InteractionMouse
{
    PreInteractionMouse(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        QPoint _from,
        QPoint _to,
        Qt::MouseButton _button
    );
};

/// A mouse wheel interaction before the preprocessing
struct PreInteractionMouseWheel : public PreInteraction,
                                  public InteractionMouseWheel
{
    PreInteractionMouseWheel(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        QPoint _angle_delta,
        QPoint _pos
    );
};

/// A keyboard interaction before the preprocessing
struct PreInteractionKeyboard : public PreInteraction,
                                public InteractionKeyboard
{
    PreInteractionKeyboard(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        Qt::Key _key,
        const QString& _sequence
    );
};

/// A list widget click interaction before the preprocessing
struct PreInteractionModelViewSelect : public PreInteraction,
                                       public InteractionModelViewSelect
{
    PreInteractionModelViewSelect(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        const QString& _name
    );
};

struct PreInteractionNumberInputModification : public PreInteraction,
                                               public NumberInputModification
{
    PreInteractionNumberInputModification(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        QEvent::Type _type,
        modification_t _modif_type,
        double _modif_number
    );
};

/// A mouse interaction after the preprocessing
struct PostInteractionMouse : public PostInteraction,
                              public InteractionMouse
{
    PostInteractionMouse(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        QPoint _from,
        QPoint _to,
        Qt::MouseButton _button
    );
};

/// A mouse wheel interaction after the preprocessing
struct PostInteractionMouseWheel : public PostInteraction,
                                   public InteractionMouseWheel
{
    PostInteractionMouseWheel(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        QPoint _angle_delta,
        QPoint _pos
    );
};

/// A keyboard interaction after the preprocessing
struct PostInteractionKeyboard : public PostInteraction,
                                 public InteractionKeyboard
{
    PostInteractionKeyboard(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        Qt::Key _key,
        const QString& _sequence
    );
};

/// A list widget click interaction after the preprocessing
struct PostInteractionModelViewSelect : public PostInteraction,
                                        public InteractionModelViewSelect
{
    PostInteractionModelViewSelect(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        const QString& _name
    );
};

struct PostInteractionNumberInputModification : public PostInteraction,
                                                public NumberInputModification
{
    PostInteractionNumberInputModification(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        Qt::KeyboardModifiers _modifiers,
        interaction_t _type,
        modification_t _modif_type,
        double _modif_number
    );
};

struct InteractionHelperAPI : public PostInteraction
{
    InteractionHelperAPI(
        intptr_t _receiver_id,
        const QVector<FindStrategy>& _how_to_find_receiver,
        QString _method_name,
        std::optional<sight::ui::test_core::helper::Select> _select,
        QStringList _args = {});

    QString methodName;
    std::optional<sight::ui::test_core::helper::Select> select;
    QStringList args;
};

/// Class which captures interactions to save them as a GuiTester-compatible GUI test C++ source file
class MODULE_UI_TEST_CLASS_API MacroSaver : public QObject
{
Q_OBJECT

public:

    // Constructor. Does nothing.
    MODULE_UI_TEST_API MacroSaver() = default;

    /**
     * @brief Method used by Qt when an infected widget receives an event.
     * @details If it's the creation of a child widget, it infects it as well, if it's a user interaction, it saves it
     * in its interactions vector.
     *
     * @param target The widget being targeted by the event
     * @param e The event which was sent to the target
     * @returns Should the event handling stop?
     */
    bool eventFilter(QObject* _target, QEvent* _e) override;

    /**
     * @brief Infects a widget and its children recursively, so that the events they will receive will be filtered with
     * @ref MacroSaver
     *
     * @param o The widget to infect
     */
    MODULE_UI_TEST_API void infect(QObject* _o);

    /// Saves the user interactions as GuiTester-compatible GUI test C++ source files named GuiTest.cpp and GuiTest.hpp
    MODULE_UI_TEST_API void save();

private:

    std::unique_ptr<PreInteraction> createInteraction(QObject* _target, QEvent* _e);
    QVector<FindStrategy> find(QObject* _o);

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

    std::vector<std::unique_ptr<PreInteraction> > m_interactions;
    QWidget* m_mainWindow = nullptr;
    bool m_dragInProgress = false;
};
