/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "modules/ui/test/config.hpp"

#include <QEvent>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QVector>
#include <QWidget>

/// An enumeration which represents the type of a user interaction
enum InteractionType
{
    MOUSE_CLICK,
    MOUSE_DRAG,
    KEYBOARD_CLICK,
    KEYBOARD_SEQUENCE,
    LIST_WIDGET_CLICK
};

/// An enumeration which represents how to find a widget, @see FindStrategy
enum class FindStrategyType
{
    ROOT,                // The object to be found is root
    ACTIVE_MODAL_WIDGET, // The object to be found is the active modal widget
    OBJECT_NAME,         // Find an object using its object name (we assume object names are unique)
    GLOBAL_TYPE,         // Find an object using its type from the root
    ACTION,              // Find an object via of its action, requires to find the action
    LOCAL_TYPE,          // Find an object using its type from the parent, requires to find the parent
    NTH_CHILD,           // Find an object using its index in its parent children list, requires to find the parent
    CANT_BE_FOUND        // Error: the object can't be found
};

/// A structure which represents a strategy to find a specific widget
struct FindStrategy
{
    FindStrategyType type;
    QString className;
    QString string;
    int integer;
};

/// The base class for all interaction
struct MacroInteraction
{
    MacroInteraction(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers
    );

    const intptr_t receiverId;
    const QVector<FindStrategy> howToFindReceiver;
    const Qt::KeyboardModifiers modifiers;
};

/// An interaction before the preprocessing (that is, directly caught from Qt)
struct PreInteraction : public MacroInteraction
{
    PreInteraction(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        QEvent::Type type
    );

    const QEvent::Type type;
};

/// An interaction after the preprocessing (ready to be translated into C++)
struct PostInteraction : public MacroInteraction
{
    PostInteraction(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        InteractionType type
    );

    const InteractionType type;
};

/// A mouse interaction (mouse click, mouse drag...)
struct InteractionMouse
{
    InteractionMouse(QPoint from, QPoint to, Qt::MouseButton button);

    const QPoint from;
    const QPoint to;
    const Qt::MouseButton button;
};

/// A keyboard interaction (key press, key sequence...)
struct InteractionKeyboard
{
    InteractionKeyboard(Qt::Key key, QString sequence);

    bool isPrintable();

    const Qt::Key key;
    const QString sequence;
};

/// An interaction when clicking on a list widget (it's a special case because it's a model-based widget)
struct InteractionListWidgetClick
{
    InteractionListWidgetClick(QString name);

    const QString name;
};

/// A mouse interaction before the preprocessing
struct PreInteractionMouse : public PreInteraction,
                             public InteractionMouse
{
    PreInteractionMouse(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        QEvent::Type type,
        QPoint from,
        QPoint to,
        Qt::MouseButton button
    );
};

/// A keyboard interaction before the preprocessing
struct PreInteractionKeyboard : public PreInteraction,
                                public InteractionKeyboard
{
    PreInteractionKeyboard(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        QEvent::Type type,
        Qt::Key key,
        const QString& sequence
    );
};

/// A list widget click interaction before the preprocessing
struct PreInteractionListWidgetClick : public PreInteraction,
                                       public InteractionListWidgetClick
{
    PreInteractionListWidgetClick(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        QEvent::Type type,
        const QString& name
    );
};

/// A mouse interaction after the preprocessing
struct PostInteractionMouse : public PostInteraction,
                              public InteractionMouse
{
    PostInteractionMouse(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        InteractionType type,
        QPoint from,
        QPoint to,
        Qt::MouseButton button
    );
};

/// A keyboard interaction after the preprocessing
struct PostInteractionKeyboard : public PostInteraction,
                                 public InteractionKeyboard
{
    PostInteractionKeyboard(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        InteractionType type,
        Qt::Key key,
        const QString& sequence
    );
};

/// A list widget click interaction after the preprocessing
struct PostInteractionListWidgetClick : public PostInteraction,
                                        public InteractionListWidgetClick
{
    PostInteractionListWidgetClick(
        intptr_t receiverId,
        const QVector<FindStrategy>& howToFindReceiver,
        Qt::KeyboardModifiers modifiers,
        InteractionType type,
        const QString& name
    );
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
    bool eventFilter(QObject* target, QEvent* e) override;

    /**
     * @brief Infects a widget and its children recursively, so that the events they will receive will be filtered with
     * @ref MacroSaver
     *
     * @param o The widget to infect
     */
    MODULE_UI_TEST_API void infect(QObject* o);

    /// Saves the user interactions as GuiTester-compatible GUI test C++ source files named GuiTest.cpp and GuiTest.hpp
    MODULE_UI_TEST_API void save();

private:

    std::shared_ptr<PreInteraction> createInteraction(QObject* target, QEvent* e);
    QVector<FindStrategy> find(QObject* o);

    static QObject* findChild(
        QObject* o,
        const QString& type,
        const QString& objectName    = "",
        Qt::FindChildOptions options = Qt::FindChildrenRecursively
    );
    static QObjectList findChildren(
        QObject* o,
        const QString& type,
        const QString& objectName    = "",
        Qt::FindChildOptions options = Qt::FindChildrenRecursively
    );

    std::vector<std::shared_ptr<PreInteraction> > m_interactions;
    QWidget* m_mainWindow = nullptr;
};
