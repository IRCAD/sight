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
// cspell:ignore ilwc strat htfr modif imvs

#include "MacroSaver.hpp"

#include <core/runtime/profile/profile.hpp>

#include <QAction>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QFile>
#include <QLineEdit>
#include <QListWidget>
#include <QMetaEnum>
#include <QPushButton>
#include <QTest>

const QEvent::Type ModelViewSelect         = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type NumberInputModification = static_cast<QEvent::Type>(QEvent::registerEventType());

//------------------------------------------------------------------------------

QString modifiersToString(Qt::KeyboardModifiers modifiers)
{
    QString res;
    static const std::map<QString, Qt::KeyboardModifier> modifiersList {
        {"Shift", Qt::KeyboardModifier::ShiftModifier},
        {"Control", Qt::KeyboardModifier::ControlModifier},
        {"Alt", Qt::KeyboardModifier::AltModifier},
        {"Meta", Qt::KeyboardModifier::MetaModifier},
        {"Keypad", Qt::KeyboardModifier::KeypadModifier},
        {"GroupSwitch", Qt::KeyboardModifier::GroupSwitchModifier}
    };
    if(modifiers == Qt::KeyboardModifier::NoModifier)
    {
        res = "Qt::KeyboardModifier::NoModifier";
    }
    else
    {
        for(const auto& [name, value] : modifiersList)
        {
            if((modifiers & value) != 0U)
            {
                if(!res.isEmpty())
                {
                    res += " | ";
                }

                res += "Qt::KeyboardModifier::" + name + "Modifier";
            }
        }
    }

    return res;
}

//------------------------------------------------------------------------------

MacroInteraction::MacroInteraction(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers
) :
    receiverId(receiverId),
    howToFindReceiver(howToFindReceiver),
    modifiers(modifiers)
{
}

//------------------------------------------------------------------------------

PreInteraction::PreInteraction(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type
) :
    MacroInteraction(receiverId, howToFindReceiver, modifiers),
    type(type)
{
}

//------------------------------------------------------------------------------

PostInteraction::PostInteraction(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type
) :
    MacroInteraction(receiverId, howToFindReceiver, modifiers),
    type(type)
{
}

//------------------------------------------------------------------------------

InteractionMouse::InteractionMouse(QPoint from, QPoint to, Qt::MouseButton button) :
    from(from),
    to(to),
    button(button)
{
}

InteractionMouseWheel::InteractionMouseWheel(QPoint angleDelta, QPoint pos) :
    angleDelta(angleDelta),
    pos(pos)
{
}

//------------------------------------------------------------------------------

InteractionKeyboard::InteractionKeyboard(Qt::Key key, QString sequence) :
    key(key),
    sequence(std::move(sequence))
{
}

//------------------------------------------------------------------------------

bool InteractionKeyboard::isPrintable() const
{
    return !sequence.isEmpty() && sequence[0].isPrint();
}

//------------------------------------------------------------------------------

InteractionModelViewSelect::InteractionModelViewSelect(QString name) :
    name(std::move(name))
{
}

NumberInputModification::NumberInputModification(ModificationType type, double number) :
    modifType(type),
    modifNumber(number)
{
}

//------------------------------------------------------------------------------

PreInteractionMouse::PreInteractionMouse(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type,
    QPoint from,
    QPoint to,
    Qt::MouseButton button
) :
    PreInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionMouse(from, to, button)
{
}

PreInteractionMouseWheel::PreInteractionMouseWheel(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type,
    QPoint angleDelta,
    QPoint pos
) :
    PreInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionMouseWheel(angleDelta, pos)
{
}

//------------------------------------------------------------------------------

PreInteractionKeyboard::PreInteractionKeyboard(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type,
    Qt::Key key,
    const QString& sequence
) :
    PreInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionKeyboard(key, sequence)
{
}

//------------------------------------------------------------------------------

PreInteractionModelViewSelect::PreInteractionModelViewSelect(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type,
    const QString& name
) :
    PreInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionModelViewSelect(name)
{
}

PreInteractionNumberInputModification::PreInteractionNumberInputModification(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type,
    ModificationType modifType,
    double modifNumber
) :
    PreInteraction(receiverId, howToFindReceiver, modifiers, type),
    NumberInputModification(modifType, modifNumber)
{
}

//------------------------------------------------------------------------------

PostInteractionMouse::PostInteractionMouse(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type,
    QPoint from,
    QPoint to,
    Qt::MouseButton button
) :
    PostInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionMouse(from, to, button)
{
}

PostInteractionMouseWheel::PostInteractionMouseWheel(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type,
    QPoint angleDelta,
    QPoint pos
) :
    PostInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionMouseWheel(angleDelta, pos)
{
}

//------------------------------------------------------------------------------

PostInteractionKeyboard::PostInteractionKeyboard(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type,
    Qt::Key key,
    const QString& sequence
) :
    PostInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionKeyboard(key, sequence)
{
}

//------------------------------------------------------------------------------

PostInteractionModelViewSelect::PostInteractionModelViewSelect(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type,
    const QString& name
) :
    PostInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionModelViewSelect(name)
{
}

PostInteractionNumberInputModification::PostInteractionNumberInputModification(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type,
    ModificationType modifType,
    double modifNumber
) :
    PostInteraction(receiverId, howToFindReceiver, modifiers, type),
    NumberInputModification(modifType, modifNumber)
{
}

InteractionHelperAPI::InteractionHelperAPI(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    QString methodName,
    std::optional<sight::ui::testCore::helper::Select> select,
    QStringList args
) :
    PostInteraction(receiverId, howToFindReceiver, {}, HELPER_API), methodName(std::move(methodName)),
    select(std::move(select)), args(std::move(args))
{
}

//------------------------------------------------------------------------------

bool MacroSaver::eventFilter(QObject* target, QEvent* e)
{
    if(e->type() == QEvent::ChildAdded)
    {
        auto* ce = dynamic_cast<QChildEvent*>(e);
        infect(ce->child());
        if(m_mainWindow == nullptr)
        {
            m_mainWindow = qApp->activeWindow();
        }
    }
    else if(e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease
            || e->type() == QEvent::MouseMove || e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::Wheel
            || e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)
    {
        // Ignore the MouseMove event if no buttons are pressed, else we will get one thousand of events
        QWidget* w = qobject_cast<QWidget*>(target);
        if((e->type() != QEvent::MouseMove || dynamic_cast<QMouseEvent*>(e)->buttons() != Qt::MouseButton::NoButton)
           && !(w != nullptr && w->isWindow()) && target != m_mainWindow)
        {
            std::unique_ptr<PreInteraction> interaction = createInteraction(target, e);
            if(interaction != nullptr)
            {
                m_interactions.emplace_back(std::move(interaction));
            }
        }
    }

    return QObject::eventFilter(target, e);
}

//------------------------------------------------------------------------------

void MacroSaver::infect(QObject* o)
{
    o->installEventFilter(this);
    for(QObject* child : o->children())
    {
        infect(child);
    }
}

//------------------------------------------------------------------------------

QObject* MacroSaver::findChild(QObject* o, const QString& type, const QString& objectName, Qt::FindChildOptions options)
{
    for(QObject* child : o->children())
    {
        if(child->inherits(type.toLatin1()) && (objectName == "" || child->objectName() == objectName))
        {
            return child;
        }

        if(options == Qt::FindChildrenRecursively)
        {
            QObject* rec = findChild(child, type, objectName, options);
            if(rec != nullptr)
            {
                return rec;
            }
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

QObjectList MacroSaver::findChildren(
    QObject* o,
    const QString& type,
    const QString& objectName,
    Qt::FindChildOptions options
)
{
    QObjectList res;
    for(QObject* child : o->children())
    {
        if(child->inherits(type.toLatin1()) && (objectName == "" || child->objectName() == objectName))
        {
            res.append(child);
        }

        if(options == Qt::FindChildrenRecursively)
        {
            res.append(findChildren(child, type, objectName, options));
        }
    }

    return res;
}

//------------------------------------------------------------------------------

static QObject* parentOf(const QObject* obj)
{
    QObject* parent = obj->parent();
    if(parent != nullptr && parent->objectName() == "qt_scrollarea_viewport")
    {
        parent = parent->parent();
    }

    return parent;
}

//------------------------------------------------------------------------------

QVector<FindStrategy> MacroSaver::find(QObject* o)
{
    assert(o != nullptr);
    QWidget* w           = qobject_cast<QWidget*>(o);
    QByteArray className = o->metaObject()->className();
    if(className == "QWidgetWindow" || className == "QColorPicker") // Those are private classes of Qt, we can't use
                                                                    // them as is.
    {
        className = "QWidget";
    }

    if(o == m_mainWindow)
    {
        return {{FindStrategyType::ROOT, className, "", 0}};
    }

    if(o == qApp->activeModalWidget())
    {
        return {{FindStrategyType::ACTIVE_MODAL_WIDGET, className,
            w->objectName().isEmpty() ? w->windowTitle() : w->objectName(), 0
        }
        };
    }

    if(o->objectName() != "")
    {
        QVector<FindStrategy> res {{FindStrategyType::OBJECT_NAME, className, o->objectName().toLatin1(), 0}};
        if(QObjectList children;
           w != nullptr
           && (children = findChildren(w->window(), className, o->objectName())).size() == 1 && children[0] == o)
        {
            res.append(find(w->window()));
        }
        else if(QObjectList children2 = findChildren(m_mainWindow, className, o->objectName());
                children2.size() == 1 && children2[0] == o)
        {
            res.append(find(m_mainWindow));
        }
        else
        {
            QVector<FindStrategy> bestAncestorStrat;
            for(QObject* currentAncestor = parentOf(o) ;
                currentAncestor != nullptr ;
                currentAncestor = parentOf(currentAncestor))
            {
                if(QObjectList children3 =
                       findChildren(currentAncestor, className, o->objectName());
                   children3.size() == 1 && children3[0] == o)
                {
                    QVector<FindStrategy> currentAncestorStrat = find(currentAncestor);
                    if(currentAncestorStrat.back().type != FindStrategyType::CANT_BE_FOUND
                       && (bestAncestorStrat.empty() || currentAncestorStrat.size() < bestAncestorStrat.size()))
                    {
                        bestAncestorStrat = currentAncestorStrat;
                    }
                }
            }

            if(bestAncestorStrat.empty())
            {
                res.append({FindStrategyType::CANT_BE_FOUND, className, "", 0});
            }
            else
            {
                res.append(bestAncestorStrat);
            }
        }

        return res;
    }

    if(findChildren(m_mainWindow, className).size() == 1)
    {
        return {{FindStrategyType::GLOBAL_TYPE, className, "", 0}};
    }

    if(w != nullptr && w->actions().size() == 1)
    {
        QVector<FindStrategy> res {{FindStrategyType::ACTION, className, "", 0}};
        res.append(find(w->actions()[0]));
        return res;
    }

    if(parentOf(o) == nullptr)
    {
        return {{FindStrategyType::CANT_BE_FOUND, className, "", 0}};
    }

    if(findChildren(parentOf(o), className, "", Qt::FindDirectChildrenOnly).size() == 1)
    {
        QVector<FindStrategy> res {{FindStrategyType::LOCAL_TYPE, className, "", 0}};
        res.append(find(parentOf(o)));
        return res;
    } // else

    int index = 0;
    for( ; index < parentOf(o)->children().size() ; index++)
    {
        if(parentOf(o)->children()[index] == o)
        {
            break;
        }
    }

    QVector<FindStrategy> res {{FindStrategyType::NTH_CHILD, className, "", index}};
    res.append(find(parentOf(o)));
    return res;
}

//------------------------------------------------------------------------------

static QString selectToCode(const sight::ui::testCore::helper::Select& select)
{
    switch(select.type())
    {
        case sight::ui::testCore::helper::Select::Type::FROM_MAIN:
            return QString("\"%1\"").arg(std::get<std::string>(select.data()).c_str());

        case sight::ui::testCore::helper::Select::Type::FROM_DIALOG:
            return QString("Select::fromDialog(\"%1\")").arg(std::get<std::string>(select.data()).c_str());

        case sight::ui::testCore::helper::Select::Type::FROM_PARENT:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(select.data());
            return QString(R"(Select::fromParent("%1", "%2"))").arg(parentName.c_str()).arg(childName.c_str());
        }

        case sight::ui::testCore::helper::Select::Type::FROM_CURRENT:
            return QString("Select::fromCurrent(\"%1\")").arg(std::get<std::string>(select.data()).c_str());

        case sight::ui::testCore::helper::Select::Type::CURRENT:
            return "Select::current()";

        case sight::ui::testCore::helper::Select::Type::DIALOG:
            return "Select::dialog()";
    }

    SIGHT_ASSERT(
        "Invalid Select type",
        0
    );
    return "";
}

//------------------------------------------------------------------------------

[[nodiscard]] static std::pair<QVector<FindStrategy>, sight::ui::testCore::helper::Select> computeSelect(
    QVector<FindStrategy> howToFindReceiver
)
{
    sight::ui::testCore::helper::Select select = sight::ui::testCore::helper::Select::current();
    if(howToFindReceiver.size() >= 2 && howToFindReceiver[0].type == FindStrategyType::OBJECT_NAME
       && howToFindReceiver[1].type == FindStrategyType::ROOT)
    {
        select = sight::ui::testCore::helper::Select(howToFindReceiver[0].string.toStdString());
        howToFindReceiver.pop_front();
        howToFindReceiver.pop_front();
    }
    else if(howToFindReceiver.size() >= 2 && howToFindReceiver[0].type == FindStrategyType::OBJECT_NAME
            && howToFindReceiver[1].type == FindStrategyType::ACTIVE_MODAL_WIDGET)
    {
        select = sight::ui::testCore::helper::Select::fromDialog(howToFindReceiver[0].string.toStdString());
        howToFindReceiver.pop_front();
        howToFindReceiver.pop_front();
    }
    else if(howToFindReceiver.size() >= 2 && howToFindReceiver[0].type == FindStrategyType::OBJECT_NAME
            && howToFindReceiver[1].type == FindStrategyType::OBJECT_NAME
            && howToFindReceiver[2].type == FindStrategyType::ROOT)
    {
        select = sight::ui::testCore::helper::Select::fromParent(
            howToFindReceiver[1].string.toStdString(),
            howToFindReceiver[0].string.toStdString()
        );
        howToFindReceiver.pop_front();
        howToFindReceiver.pop_front();
        howToFindReceiver.pop_front();
    }
    else if(!howToFindReceiver.empty() && howToFindReceiver[0].type == FindStrategyType::ACTIVE_MODAL_WIDGET)
    {
        select = sight::ui::testCore::helper::Select::dialog();
        howToFindReceiver.pop_front();
    }

    return {howToFindReceiver, select};
}

//------------------------------------------------------------------------------

void MacroSaver::save()
{
    // Interactions preprocessing
    std::vector<std::unique_ptr<PostInteraction> > prePostInteractions;
    for(std::size_t i = 0 ; i < m_interactions.size() ; i++)
    {
        if(m_interactions[i]->type == QEvent::Type::MouseButtonPress)
        {
            const auto& im = static_cast<PreInteractionMouse&>(*m_interactions[i]);
            if(i + 1 == m_interactions.size())
            {
                // This mouse button press event is the last event, it won't be ended with a release. Ignore it.
                continue;
            }

            if(m_interactions[i + 1]->type == QEvent::Type::MouseButtonRelease)
            {
                // A mouse button press immediately followed with a mouse button release is a simple mouse click.
                prePostInteractions.emplace_back(
                    std::make_unique<PostInteractionMouse>(
                        im.receiverId,
                        im.howToFindReceiver,
                        im.modifiers,
                        MOUSE_CLICK,
                        im.from,
                        im.to,
                        im.button
                    )
                );
                i++;
            }
            else if(m_interactions[i + 1]->type == QEvent::Type::MouseMove)
            {
                // A mouse button press followed with a mouse move and ended with a release is a mouse drag.
                i++;
                // Canonize the mouse drag
                for( ; i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::MouseMove ; i++)
                {
                }

                if(i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::MouseButtonRelease)
                {
                    const auto& release = static_cast<PreInteractionMouse&>(*m_interactions[i]);
                    prePostInteractions.emplace_back(
                        std::make_unique<PostInteractionMouse>(
                            im.receiverId,
                            im.howToFindReceiver,
                            im.modifiers,
                            MOUSE_DRAG,
                            im.from,
                            release.to,
                            im.button
                        )
                    );
                }
            }
        }
        else if(m_interactions[i]->type == QEvent::Type::MouseButtonDblClick)
        {
            if(prePostInteractions.back()->type == MOUSE_DOUBLE_CLICK)
            {
                // This is a duplicated event, ignore it
                continue;
            }

            if(prePostInteractions.back()->type == MOUSE_CLICK)
            {
                // Ignore the click that generated this double click
                prePostInteractions.pop_back();
            }

            const auto& im = static_cast<PreInteractionMouse&>(*m_interactions[i]);
            prePostInteractions.emplace_back(
                std::make_unique<PostInteractionMouse>(
                    im.receiverId,
                    im.howToFindReceiver,
                    im.modifiers,
                    MOUSE_DOUBLE_CLICK,
                    im.from,
                    im.to,
                    im.button
                )
            );
        }
        else if(m_interactions[i]->type == QEvent::Type::Wheel)
        {
            const auto& imw   = static_cast<PreInteractionMouseWheel&>(*m_interactions[i]);
            QPoint angleDelta = imw.angleDelta / 2; // Divide by two because events are duplicated
            // Compress all mouse wheel events into one
            while(i + 1 < m_interactions.size() && m_interactions[i + 1]->type == QEvent::Type::Wheel)
            {
                i++;
                angleDelta += static_cast<PreInteractionMouseWheel&>(*m_interactions[i]).angleDelta / 2;
            }

            prePostInteractions.emplace_back(
                std::make_unique<PostInteractionMouseWheel>(
                    imw.receiverId,
                    imw.howToFindReceiver,
                    imw.modifiers,
                    MOUSE_WHEEL,
                    angleDelta,
                    imw.pos
                )
            );
        }
        else if(m_interactions[i]->type == QEvent::Type::KeyPress)
        {
            const auto& ik = static_cast<PreInteractionKeyboard&>(*m_interactions[i].get());
            if(ik.isPrintable())
            {
                // If the key is printable, we will attempt to "compress" it with the following key events if they are
                // printable as well, to make the resulting code more readable.
                QString sequence;
                const PreInteractionKeyboard* press = nullptr;
                while(i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::KeyPress
                      && (press = static_cast<PreInteractionKeyboard*>(m_interactions[i].get())) != nullptr
                      && press->isPrintable())
                {
                    sequence += press->sequence;
                    i++;

                    for( ; i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::KeyRelease ; i++)
                    {
                    }
                }

                i--;
                if(sequence.size() > 0)
                {
                    prePostInteractions.emplace_back(
                        std::make_unique<PostInteractionKeyboard>(
                            ik.receiverId,
                            ik.howToFindReceiver,
                            ik.modifiers,
                            (sequence.size() > 1 ? KEYBOARD_SEQUENCE : KEYBOARD_CLICK),
                            ik.key,
                            sequence
                        )
                    );
                }
            }
            else
            {
                // If the key isn't printable, then it's a control key such as BackSpace. We can't compress it with
                // other key events.

                // Avoid duplicated events
                for( ; i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::KeyPress ; i++)
                {
                }

                if(i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::KeyRelease)
                {
                    prePostInteractions.emplace_back(
                        std::make_unique<PostInteractionKeyboard>(
                            ik.receiverId,
                            ik.howToFindReceiver,
                            ik.modifiers,
                            KEYBOARD_CLICK,
                            ik.key,
                            ik.sequence
                        )
                    );
                }
            }
        }
        else if(m_interactions[i]->type == ModelViewSelect)
        {
            const auto& ilwc = static_cast<PreInteractionModelViewSelect&>(*m_interactions[i]);
            prePostInteractions.emplace_back(
                std::make_unique<PostInteractionModelViewSelect>(
                    ilwc.receiverId,
                    ilwc.howToFindReceiver,
                    ilwc.modifiers,
                    MODEL_VIEW_SELECT,
                    ilwc.name
                )
            );
        }
        else if(m_interactions[i]->type == NumberInputModification)
        {
            const auto& nim = static_cast<PreInteractionNumberInputModification&>(*m_interactions[i]);
            prePostInteractions.emplace_back(
                std::make_unique<PostInteractionNumberInputModification>(
                    nim.receiverId,
                    nim.howToFindReceiver,
                    nim.modifiers,
                    NUMBER_INPUT_MODIFICATION,
                    nim.modifType,
                    nim.modifNumber
                )
            );
        }
    }

    std::vector<std::unique_ptr<PostInteraction> > postInteractions;
    for(std::size_t i = 0 ; i < prePostInteractions.size() ; i++)
    {
        if(prePostInteractions[i]->type == MOUSE_CLICK
           && (prePostInteractions[i]->howToFindReceiver[0].className == "QPushButton"
               || prePostInteractions[i]->howToFindReceiver[0].className == "QToolButton"))
        {
            if(prePostInteractions[i]->howToFindReceiver.back().string == "preferences_configuration")
            {
                // Special case: Preferences Configuration pop up close button
                if(InteractionHelperAPI* iha =
                       nullptr;
                   postInteractions.back()->type == HELPER_API
                   && (iha = static_cast<InteractionHelperAPI*>(postInteractions.back().get()))->methodName
                   == "PreferencesConfiguration::fill")
                {
                    iha->args[0] += "}";
                }
                else
                {
                    postInteractions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            prePostInteractions[i]->
                            receiverId,
                            QVector<FindStrategy> {},
                            "PreferencesConfiguration",
                            std::nullopt,
                            QStringList {"{}"
                            })
                    );
                }
            }
            else
            {
                QVector<FindStrategy> howToFindReceiver    = prePostInteractions[i]->howToFindReceiver;
                sight::ui::testCore::helper::Select select = sight::ui::testCore::helper::Select::current();
                if(howToFindReceiver.front().type == FindStrategyType::ACTION)
                {
                    // helper::Button::push already handles the "Action" case.
                    howToFindReceiver.pop_front();
                }

                std::tie(howToFindReceiver, select) = computeSelect(howToFindReceiver);
                postInteractions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        prePostInteractions[i]->receiverId,
                        howToFindReceiver,
                        "Button::push",
                        select
                    )
                );
            }
        }
        else if(prePostInteractions[i]->type == MOUSE_CLICK
                && prePostInteractions[i]->howToFindReceiver[0].className == "QCheckBox")
        {
            auto [howToFindReceiver, select] = computeSelect(prePostInteractions[i]->howToFindReceiver);
            postInteractions.emplace_back(
                std::make_unique<InteractionHelperAPI>(
                    prePostInteractions[i]->receiverId,
                    howToFindReceiver,
                    "CheckBox::toggle",
                    select
                )
            );
        }
        else if(prePostInteractions[i]->type == MODEL_VIEW_SELECT
                && (prePostInteractions[i]->howToFindReceiver[0].className == "QCheckBox"
                    || prePostInteractions[i]->howToFindReceiver[0].className == "QComboBox")
                && prePostInteractions[i]->howToFindReceiver.back().string == "preferences_configuration")
        {
            const auto& imvs = static_cast<const PostInteractionModelViewSelect&>(*prePostInteractions[i]);
            QString arg      = QString(R"({"%1", "%2"})").arg(imvs.howToFindReceiver[0].string).arg(imvs.name);
            if(InteractionHelperAPI* iha =
                   nullptr;
               postInteractions.back()->type == HELPER_API
               && (iha = static_cast<InteractionHelperAPI*>(postInteractions.back().get()))->methodName
               == "PreferencesConfiguration::fill")
            {
                if(!iha->args[0].contains(QString("\"%1\"").arg(prePostInteractions[i]->howToFindReceiver[0].string)))
                {
                    iha->args[0] += QString(", " + arg);
                }
            }
            else
            {
                postInteractions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        prePostInteractions[i]->receiverId,
                        QVector<FindStrategy> {},
                        "PreferencesConfiguration::fill",
                        std::nullopt,
                        QStringList {"{" + arg
                        })
                );
            }
        }
        else if(prePostInteractions[i]->type == MODEL_VIEW_SELECT
                && prePostInteractions[i]->howToFindReceiver[0].className == "QComboBox")
        {
            if(!postInteractions.empty() && postInteractions.back()->type == MOUSE_DRAG)
            {
                // It seems mouse drags are always duplicated with comboboxes
                postInteractions.pop_back();
            }
            else if(postInteractions.size() >= 2 && postInteractions.back()->type == MOUSE_CLICK
                    && postInteractions[postInteractions.size() - 2]->type == MOUSE_CLICK)
            {
                // It seems mouse clicks are always duplicated with comboboxes
                postInteractions.pop_back();
                postInteractions.pop_back();
            }
            else if(!postInteractions.empty() && postInteractions.back()->type == HELPER_API)
            {
                // It seems mouse clicks are always duplicated with comboboxes
                continue;
            }

            const auto& imvs = static_cast<PostInteractionModelViewSelect&>(*prePostInteractions[i]);
            auto [howToFindReceiver, select] = computeSelect(imvs.howToFindReceiver);
            postInteractions.emplace_back(
                std::make_unique<InteractionHelperAPI>(
                    imvs.receiverId,
                    howToFindReceiver,
                    "ComboBox::select",
                    select,
                    QStringList {QString("\"%1\"").arg(imvs.name)
                    })
            );
        }
        else if((prePostInteractions[i]->type == KEYBOARD_CLICK || prePostInteractions[i]->type == KEYBOARD_SEQUENCE)
                && prePostInteractions[i]->howToFindReceiver[0].className == "QLineEdit")
        {
            const auto& ik = static_cast<PostInteractionKeyboard&>(*prePostInteractions[i]);
            if(prePostInteractions[i]->howToFindReceiver.back().type == FindStrategyType::ACTIVE_MODAL_WIDGET)
            {
                // Compress all keyboard sequence/click in one
                QString sequence = (ik.isPrintable() ? ik.sequence : "");
                for( ;
                     i + 1 < prePostInteractions.size() && prePostInteractions[i + 1]->receiverId == ik.receiverId
                     && (prePostInteractions[i + 1]->type == KEYBOARD_CLICK
                         || prePostInteractions[i + 1]->type == KEYBOARD_SEQUENCE) ;
                     i++)
                {
                    const auto& ik2 = static_cast<PostInteractionKeyboard&>(*prePostInteractions[i + 1]);
                    if(ik2.key == Qt::Key::Key_Backspace)
                    {
                        sequence.resize(sequence.size() - 1);
                    }
                    else if(ik2.key == Qt::Key::Key_Return)
                    {
                        break;
                    }
                    else if(ik2.isPrintable())
                    {
                        sequence += ik2.sequence;
                    }

                    // Ignore other keys
                }

                if(prePostInteractions[i]->howToFindReceiver.back().className == "QFileDialog")
                {
                    postInteractions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            ik.receiverId,
                            QVector<FindStrategy> {},
                            "FileDialog::fill",
                            std::nullopt,
                            QStringList {QString("\"%1\"").arg(sequence)
                            })
                    );
                    if(i + 1 < prePostInteractions.size() && prePostInteractions[i + 1]->receiverId == ik.receiverId
                       && prePostInteractions[i + 1]->type == KEYBOARD_CLICK
                       && static_cast<PostInteractionKeyboard&>(*prePostInteractions[i + 1]).key == Qt::Key::Key_Return)
                    {
                        // Ignore the Enter key press to confirm the file dialog, as it is already handled by
                        // helper::FieldDialog::fill
                        i++;
                    }
                }
                else if(prePostInteractions[i]->howToFindReceiver.back().string == "preferences_configuration")
                {
                    QString arg =
                        QString(R"({"%1", "%2"})").arg(prePostInteractions[i]->howToFindReceiver[0].string).arg(sequence);
                    if(InteractionHelperAPI* iha =
                           nullptr;
                       postInteractions.back()->type == HELPER_API
                       && (iha = static_cast<InteractionHelperAPI*>(postInteractions.back().get()))->methodName
                       == "PreferencesConfiguration::fill")
                    {
                        if(!iha->args[0].contains(
                               QString("\"%1\"").arg(
                                   prePostInteractions[i]->howToFindReceiver[0].
                                   string
                               )
                        ))
                        {
                            iha->args[0] += QString(", " + arg);
                        }
                    }
                    else
                    {
                        postInteractions.emplace_back(
                            std::make_unique<InteractionHelperAPI>(
                                prePostInteractions[i]->
                                receiverId,
                                QVector<FindStrategy> {},
                                "PreferencesConfiguration::fill",
                                std::nullopt,
                                QStringList {"{" + arg
                                })
                        );
                    }
                }
            }
            else
            {
                auto [howToFindReceiver, select] = computeSelect(ik.howToFindReceiver);
                postInteractions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        ik.receiverId,
                        howToFindReceiver,
                        "Field::fill",
                        select,
                        QStringList {QString("\"%1\"").arg(ik.sequence)
                        })
                );
            }
        }
        else if(prePostInteractions[i]->type == MODEL_VIEW_SELECT
                && prePostInteractions[i]->howToFindReceiver[0].className == "QListWidget")
        {
            const auto& ilwc = static_cast<PostInteractionModelViewSelect&>(*prePostInteractions[i]);
            if(prePostInteractions[i]->howToFindReceiver.back().type == FindStrategyType::ACTIVE_MODAL_WIDGET
               && prePostInteractions[i]->howToFindReceiver.back().string == "selector")
            {
                if(postInteractions.back()->type == HELPER_API
                   && static_cast<InteractionHelperAPI&>(*postInteractions.back()).methodName
                   == "selector::select"
                   && postInteractions.back()->receiverId == prePostInteractions[i]->receiverId)
                {
                    // The user selected an item then selected another one; take only the last choice
                    postInteractions.pop_back();
                }

                postInteractions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        ilwc.receiverId,
                        QVector<FindStrategy> {},
                        "selector::select",
                        std::nullopt,
                        QStringList {QString("\"%1\"").arg(ilwc.name)
                        })
                );
                if(i + 1 < prePostInteractions.size() && prePostInteractions[i + 1]->type == MOUSE_CLICK
                   && prePostInteractions[i + 1]->howToFindReceiver[0].className == "QPushButton"
                   && prePostInteractions[i + 1]->howToFindReceiver[0].string == "Ok"
                   && prePostInteractions[i + 1]->howToFindReceiver[1].type == FindStrategyType::ACTIVE_MODAL_WIDGET)
                {
                    // Ignore the eventual "Ok" button click as it is handled by helper::selector::select
                    i++;
                }

                if(i + 1 < prePostInteractions.size() && prePostInteractions[i + 1]->type == MOUSE_DOUBLE_CLICK
                   && prePostInteractions[i + 1]->howToFindReceiver[0].string == "SelectorDialogWindow")
                {
                    // It seems there might be a duplicated double click event
                    i++;
                }
            }
            else
            {
                auto [howToFindReceiver, select] = computeSelect(ilwc.howToFindReceiver);
                postInteractions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        ilwc.receiverId,
                        howToFindReceiver,
                        "ListWidget::setCurrentText",
                        select,
                        QStringList {QString("\"%1\"").arg(ilwc.name)
                        })
                );
            }
        }
        else if(prePostInteractions[i]->type == NUMBER_INPUT_MODIFICATION
                && prePostInteractions[i]->howToFindReceiver[0].className == "QSlider")
        {
            if(!postInteractions.empty()
               && (postInteractions.back()->type == MOUSE_DRAG || postInteractions.back()->type == MOUSE_CLICK
                   || postInteractions.back()->type == MOUSE_DOUBLE_CLICK))
            {
                // It seems mouse drags are always duplicated with sliders
                postInteractions.pop_back();
            }
            else if(!postInteractions.empty() && postInteractions[postInteractions.size() - 1]->type == HELPER_API)
            {
                // It seems mouse drags are always duplicated with sliders
                continue;
            }

            const auto& nim =
                static_cast<PostInteractionNumberInputModification&>(*prePostInteractions[i]);
            QVector<FindStrategy> howToFindReceiver    = nim.howToFindReceiver;
            sight::ui::testCore::helper::Select select = sight::ui::testCore::helper::Select::current();
            if(howToFindReceiver.front().type == FindStrategyType::LOCAL_TYPE
               && howToFindReceiver.front().className == "QSlider")
            {
                // Finding the actual QSlider is already handled in helper::Slider::set
                howToFindReceiver.pop_front();
            }

            std::tie(howToFindReceiver, select) = computeSelect(howToFindReceiver);
            if(nim.modifType == ModificationType::SET)
            {
                postInteractions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        nim.receiverId,
                        howToFindReceiver,
                        "Slider::set",
                        select,
                        QStringList {QString::number(nim.modifNumber)
                        })
                );
            }
            else
            {
                QString position;
                if(nim.modifType == ModificationType::DECREMENT)
                {
                    position = "helper::Slider::Position::LEFT";
                }
                else if(nim.modifType == ModificationType::INCREMENT)
                {
                    position = "helper::Slider::Position::RIGHT";
                }

                if(InteractionHelperAPI* previousInteraction =
                       nullptr;
                   !postInteractions.empty() && postInteractions.back()->type == HELPER_API
                   && postInteractions.back()->receiverId == nim.receiverId
                   && (previousInteraction =
                           static_cast<InteractionHelperAPI*>(postInteractions.back().get()))->methodName
                   == "Slider::move"
                   && previousInteraction->args[0] == position)
                {
                    // If the previous interaction is the same, we compress it with the current one
                    if(previousInteraction->args.size() == 1)
                    {
                        previousInteraction->args.push_back("2");
                    }
                    else
                    {
                        previousInteraction->args[1] = QString::number(previousInteraction->args[1].toFloat() + 1);
                    }
                }
                else
                {
                    postInteractions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            nim.receiverId,
                            howToFindReceiver,
                            "Slider::move",
                            select,
                            QStringList {position
                            })
                    );
                }
            }
        }
        else if(prePostInteractions[i]->type == NUMBER_INPUT_MODIFICATION
                && (prePostInteractions[i]->howToFindReceiver[0].className == "QSpinBox"
                    || prePostInteractions[i]->howToFindReceiver[0].className == "QDoubleSpinBox"))
        {
            if(!postInteractions.empty() && postInteractions.back()->type == MOUSE_CLICK)
            {
                // It seems mouse clicks are always duplicated with spinboxes
                postInteractions.pop_back();
            }
            else if(!postInteractions.empty() && postInteractions.back()->type == HELPER_API)
            {
                // It seems mouse clicks are always duplicated with spinboxes
                continue;
            }

            const auto& nim = static_cast<PostInteractionNumberInputModification&>(*prePostInteractions[i]);
            auto [howToFindReceiver, select] = computeSelect(nim.howToFindReceiver);
            if(nim.modifType == ModificationType::SET)
            {
                assert(0);
            }
            else
            {
                QString methodName;
                if(nim.modifType == ModificationType::DECREMENT)
                {
                    methodName = "SpinBox::decrement";
                }
                else if(nim.modifType == ModificationType::INCREMENT)
                {
                    methodName = "SpinBox::increment";
                }

                if(InteractionHelperAPI* previousInteraction =
                       nullptr;
                   !postInteractions.empty() && postInteractions.back()->type == HELPER_API
                   && postInteractions.back()->receiverId == nim.receiverId
                   && (previousInteraction =
                           static_cast<InteractionHelperAPI*>(postInteractions.back().get()))->methodName == methodName)
                {
                    if(previousInteraction->args.empty())
                    {
                        previousInteraction->args.push_back("2");
                    }
                    else
                    {
                        previousInteraction->args[0] = QString::number(previousInteraction->args[0].toFloat() + 1);
                    }
                }
                else
                {
                    postInteractions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            nim.receiverId,
                            howToFindReceiver,
                            methodName,
                            select
                        )
                    );
                }
            }
        }
        else if(prePostInteractions[i]->type == MODEL_VIEW_SELECT
                && prePostInteractions[i]->howToFindReceiver[0].className == "QColorDialog")
        {
            // Ignore all interactions inside the color dialog
            while(std::ranges::any_of(
                      postInteractions.back()->howToFindReceiver,
                      [](const FindStrategy& s)
            {
                return s.className == "QColorDialog";
            }))
            {
                postInteractions.pop_back();
            }

            if((postInteractions.back()->type == MOUSE_CLICK || postInteractions.back()->type == HELPER_API)
               && !postInteractions.back()->howToFindReceiver.empty()
               && postInteractions.back()->howToFindReceiver[0].className == "QPushButton")
            {
                // Get rid of duplicate button click
                postInteractions.pop_back();
            }

            QVector<FindStrategy> howToFindReceiver;
            std::optional<sight::ui::testCore::helper::Select> select;
            if(InteractionHelperAPI* iha =
                   nullptr;
               postInteractions.back()->type == HELPER_API
               && (iha = static_cast<InteractionHelperAPI*>(postInteractions.back().get()))->methodName
               == "Button::push")
            {
                // helper::ColorParameter::select already clicks on the Color button
                howToFindReceiver = iha->howToFindReceiver;
                select            = iha->select;
                postInteractions.pop_back();
            }
            else
            {
                // It's a duplicate, ignore it
                continue;
            }

            postInteractions.emplace_back(
                std::make_unique<InteractionHelperAPI>(
                    prePostInteractions[i]->receiverId,
                    howToFindReceiver,
                    "ColorParameter::select",
                    select,
                    QStringList {QString("\"%1\"").arg(
                                     static_cast<PostInteractionModelViewSelect&>(*prePostInteractions
                                                                                  [i]).name
                    )
                    })
            );
        }
        else if(prePostInteractions[i]->howToFindReceiver.back().string == "preferences_configuration"
                || std::ranges::any_of(
                    prePostInteractions[i]->howToFindReceiver,
                    [](const FindStrategy& f)
        {
            return f.string == "preferences_configurationWindow";
        }))
        {
            // Ignore other interactions in PreferencesConfiguration
        }
        else
        {
            postInteractions.emplace_back(std::move(prePostInteractions[i]));
        }
    }

    QStringList dependencies;
    bool useHelpers           = false;
    bool useSelectConstructor = false;
    for(const std::unique_ptr<PostInteraction>& interaction : postInteractions)
    {
        for(const FindStrategy& strategy : interaction->howToFindReceiver)
        {
            if(strategy.type == FindStrategyType::GLOBAL_TYPE || strategy.type == FindStrategyType::LOCAL_TYPE)
            {
                dependencies.append(strategy.className);
            }
            else if(strategy.type == FindStrategyType::ACTION)
            {
                dependencies.append("QAction");
            }
            else if(strategy.type == FindStrategyType::ACTIVE_MODAL_WIDGET)
            {
                dependencies.append("ui/testCore/helper/Dialog.hpp");
                dependencies.append(strategy.className);
            }
        }

        if(interaction->type == MOUSE_CLICK || interaction->type == MOUSE_DRAG)
        {
            dependencies.append("QPoint");
        }
        else if(interaction->type == MODEL_VIEW_SELECT)
        {
            dependencies.append("QListWidget");
        }

        if(interaction->type == HELPER_API)
        {
            useHelpers = true;
            const auto& iha = static_cast<InteractionHelperAPI&>(*interaction);
            if(iha.select && iha.select.value().type() != sight::ui::testCore::helper::Select::Type::FROM_MAIN)
            {
                useSelectConstructor = true;
            }

            dependencies.append(QString("ui/testCore/helper/%1.hpp").arg(iha.methodName.split("::")[0]));
        }
    }

    dependencies.sort();
    dependencies.removeDuplicates();

    static constexpr auto write = [](QFile& f, int indentation, const QString& line)
                                  {
                                      if(f.write(QByteArray(indentation, ' ') + line.toLatin1() + '\n') < line.size())
                                      {
                                          throw std::runtime_error("Couldn't write data in the file.");
                                      }
                                  };

    QFile cpp("GuiTest.cpp");
    cpp.open(QIODevice::WriteOnly);

    write(cpp, 0, "#include \"GuiTest.hpp\"");
    write(cpp, 0, "");
    write(cpp, 0, "#include <core/runtime/path.hpp>");
    write(cpp, 0, "#include <ui/testCore/Tester.hpp>");
    write(cpp, 0, "");
    write(cpp, 0, "#include <boost/dll.hpp>");
    write(cpp, 0, "");
    for(const QString& dependency : dependencies)
    {
        write(cpp, 0, QString("#include <%1>").arg(dependency));
    }

    write(cpp, 0, "");
    write(cpp, 0, "CPPUNIT_TEST_SUITE_REGISTRATION(GuiTest);");
    write(cpp, 0, "");
    write(cpp, 0, "//------------------------------------------------------------------------------");
    write(cpp, 0, "");
    write(cpp, 0, "std::filesystem::path GuiTest::getProfilePath()");
    write(cpp, 0, "{");
    write(cpp, 4, "const std::filesystem::path cwd = std::filesystem::path(");
    write(cpp, 8, "boost::dll::this_line_location().parent_path().parent_path().string()");
    write(cpp, 4, ");");
    std::filesystem::path absoluteProfilePath(sight::core::runtime::get_current_profile()->get_file_path());
    QString profilePath(QString::fromStdString(std::filesystem::relative(absoluteProfilePath).string()));
    write(cpp, 4, QString("return cwd / \"%1\";").arg(profilePath).toLatin1());
    write(cpp, 0, "}");
    write(cpp, 0, "");

    write(cpp, 0, "void GuiTest::test()");
    write(cpp, 0, "{");
    write(cpp, 4, "start(\"GuiTest\",");
    write(cpp, 8, "[](sight::ui::testCore::Tester& tester)");
    write(cpp, 4, "{");

    if(useHelpers)
    {
        write(cpp, 8, "namespace helper = sight::ui::testCore::helper;");
    }

    if(useSelectConstructor)
    {
        write(cpp, 8, "using Select     = helper::Select;");
    }

    write(cpp, 0, "");

    intptr_t currentItemId = 0;
    for(const std::unique_ptr<PostInteraction>& interaction : postInteractions)
    {
        if(interaction->receiverId != currentItemId)
        {
            // The receiver of this interaction isn't the current item: We must take it.
            if(interaction->howToFindReceiver.size() >= 2
               && interaction->howToFindReceiver.back().type == FindStrategyType::ROOT
               && interaction->howToFindReceiver[interaction->howToFindReceiver.size() - 2].type
               == FindStrategyType::OBJECT_NAME)
            {
                write(
                    cpp,
                    8,
                    QString(R"(tester.take("%1", "%1");)").arg(
                        interaction->howToFindReceiver[interaction->
                                                       howToFindReceiver
                                                       .size() - 2].string
                    )
                );
                interaction->howToFindReceiver.pop_back();
                interaction->howToFindReceiver.pop_back();
            }

            for(int i = interaction->howToFindReceiver.size() - 1 ; i >= 0 ; i--)
            {
                FindStrategy strat = interaction->howToFindReceiver[i];
                if(strat.type == FindStrategyType::ROOT)
                {
                    write(cpp, 8, "tester.take(\"main window\", tester.getMainWindow());");
                }
                else if(strat.type == FindStrategyType::ACTIVE_MODAL_WIDGET)
                {
                    write(
                        cpp,
                        8,
                        QString("helper::Dialog::take<%1*>(tester, \"%2\");").arg(strat.className).arg(strat.string)
                    );
                }
                else if(strat.type == FindStrategyType::OBJECT_NAME)
                {
                    write(cpp, 8, "tester.yields(");
                    write(cpp, 12, QString("\"%1\",").arg(strat.string));
                    write(
                        cpp,
                        12,
                        QString("[](QObject* old) -> QObject* {return old->findChild<QObject*>(\"%1\");}")
                        .arg(strat.string)
                    );
                    write(cpp, 8, ");");
                }
                else if(strat.type == FindStrategyType::GLOBAL_TYPE)
                {
                    write(cpp, 8, QString("tester.take%1(").arg(strat.string.isEmpty() ? "" : "<QWidget*>"));
                    write(cpp, 12, QString("\"%1\",").arg(strat.className));
                    write(cpp, 12, "[&tester]() -> QObject*");
                    write(cpp, 8, "{");
                    write(
                        cpp,
                        12,
                        QString("return tester.getMainWindow()->findChild<%1*>();")
                        .arg(strat.className)
                    );
                    write(cpp, 8, "});");
                }
                else if(strat.type == FindStrategyType::ACTION)
                {
                    write(cpp, 8, "tester.yields(");
                    write(cpp, 12, QString("\"%1\",").arg(strat.className));
                    write(cpp, 12, "[](QObject* old) -> QObject*");
                    write(cpp, 8, "{");
                    write(
                        cpp,
                        12,
                        "return sight::ui::testCore::Tester::getWidgetFromAction(qobject_cast<QAction*>(old));"
                    );
                    write(cpp, 8, "});");
                }
                else if(strat.type == FindStrategyType::LOCAL_TYPE)
                {
                    write(cpp, 8, "tester.yields(");
                    write(cpp, 12, QString("\"%1\",").arg(strat.className));
                    write(cpp, 12, "[](QObject* old) -> QObject*");
                    write(cpp, 8, "{");
                    write(cpp, 12, QString("return old->findChild<%1*>();").arg(strat.className));
                    write(cpp, 8, "});");
                }
                else if(strat.type == FindStrategyType::NTH_CHILD)
                {
                    write(cpp, 8, "tester.yields(");
                    write(cpp, 12, QString("\"%1\",").arg(strat.className));
                    write(cpp, 12, "[](QObject* old) -> QObject*");
                    write(cpp, 8, "{");
                    write(cpp, 12, QString("return old->children()[%1];").arg(strat.integer));
                    write(cpp, 8, "});");
                }
                else if(strat.type == FindStrategyType::CANT_BE_FOUND)
                {
                    write(
                        cpp,
                        8,
                        QString("// TODO: Find the %1. MacroSaver wasn't able to find it automatically.")
                        .arg(strat.className)
                    );
                }
            }

            currentItemId = interaction->receiverId;
        }

        const static QMetaEnum mouseButton = QMetaEnum::fromType<Qt::MouseButtons>();
        const static QMetaEnum key         = QMetaEnum::fromType<Qt::Key>();
        if(interaction->type == MOUSE_CLICK)
        {
            const auto& im = static_cast<PostInteractionMouse&>(*interaction);
            write(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseClick>(Qt::MouseButton::%1, %2, %3));"
                )
                .arg(mouseButton.valueToKey(static_cast<int>(im.button))).arg(modifiersToString(im.modifiers))
                .arg(QTest::toString(im.from))
            );
        }
        else if(interaction->type == MOUSE_DOUBLE_CLICK)
        {
            const auto& im = static_cast<PostInteractionMouse&>(*interaction);
            write(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseDoubleClick>(Qt::MouseButton::%1, %2, %3));"
                )
                .arg(mouseButton.valueToKey(static_cast<int>(im.button))).arg(modifiersToString(im.modifiers))
                .arg(QTest::toString(im.from))
            );
        }
        else if(interaction->type == MOUSE_DRAG)
        {
            const auto& im = static_cast<PostInteractionMouse&>(*interaction);
            write(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseDrag>(%1, %2, Qt::MouseButton::%3, %4));"
                )
                .arg(QTest::toString(im.from)).arg(QTest::toString(im.to))
                .arg(mouseButton.valueToKey(static_cast<int>(im.button))).arg(modifiersToString(im.modifiers))
            );
        }
        else if(interaction->type == MOUSE_WHEEL)
        {
            const auto& im = static_cast<PostInteractionMouseWheel&>(*interaction);
            write(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseWheel>(%1, %2, %3));"
                )
                .arg(QTest::toString(im.angleDelta)).arg(modifiersToString(im.modifiers)).arg(QTest::toString(im.pos))
            );
        }
        else if(interaction->type == KEYBOARD_CLICK)
        {
            const auto& ik = static_cast<PostInteractionKeyboard&>(*interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::testCore::KeyboardClick>(Qt::Key::%1, %2));")
                .arg(key.valueToKey(static_cast<int>(ik.key))).arg(modifiersToString(ik.modifiers))
            );
        }
        else if(interaction->type == KEYBOARD_SEQUENCE)
        {
            const auto& ik = static_cast<PostInteractionKeyboard&>(*interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::testCore::KeyboardSequence>(\"%1\", %2));")
                .arg(ik.sequence).arg(modifiersToString(ik.modifiers))
            );
        }
        else if(interaction->type == MODEL_VIEW_SELECT)
        {
            const auto& ilwc = static_cast<PostInteractionModelViewSelect&>(*interaction);
            write(cpp, 8, "tester.doSomethingAsynchronously<QListWidget*>(");
            write(cpp, 12, "[](QListWidget* obj)");
            write(cpp, 8, "{");
            write(
                cpp,
                12,
                QString("obj->setCurrentItem(obj->findItems(\"%1\", Qt::MatchFlag::MatchExactly)[0]);")
                .arg(ilwc.name)
            );
            write(cpp, 8, "});");
        }
        else if(interaction->type == NUMBER_INPUT_MODIFICATION)
        {
            assert(0);
        }
        else if(interaction->type == HELPER_API)
        {
            const auto& ha = static_cast<InteractionHelperAPI&>(*interaction);
            write(
                cpp,
                8,
                QString("helper::%1(tester%2%3);").arg(ha.methodName).arg(
                    ha.select ? ", "
                    + selectToCode(*ha.select) : ""
                ).arg(
                    ha.args.empty()
                    ? "" : ", " + ha.args.join(", ")
                )
            );
        }
    }

    write(cpp, 4, "});");
    write(cpp, 0, "}");

    QFile hpp("GuiTest.hpp");
    hpp.open(QIODevice::WriteOnly);
    write(hpp, 0, "#pragma once");
    write(hpp, 0, "");
    write(hpp, 0, "#include <ui/testCore/test.hpp>");
    write(hpp, 0, "");
    write(hpp, 0, "#include <core/runtime/profile/profile.hpp>");
    write(hpp, 0, "");
    write(hpp, 0, "#include <cppunit/extensions/HelperMacros.h>");
    write(hpp, 0, "");
    write(hpp, 0, "class GuiTest : public sight::ui::testCore::test");
    write(hpp, 0, "{");
    write(hpp, 0, "CPPUNIT_TEST_SUITE(GuiTest);");
    write(hpp, 0, "CPPUNIT_TEST(test);");
    write(hpp, 0, "CPPUNIT_TEST_SUITE_END();");
    write(hpp, 0, "");
    write(hpp, 0, "public:");
    write(hpp, 0, "");
    write(hpp, 4, "std::filesystem::path getProfilePath() override;");
    write(hpp, 0, "");
    write(hpp, 4, "void test();");
    write(hpp, 0, "};");
}

//------------------------------------------------------------------------------

template<typename T>
static T* findAncestor(QObject* child)
{
    for(QObject* currentAncestor = child ; currentAncestor != nullptr ; currentAncestor = currentAncestor->parent())
    {
        if(auto* ancestor = qobject_cast<T*>(currentAncestor))
        {
            return ancestor;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

std::unique_ptr<PreInteraction> MacroSaver::createInteraction(QObject* target, QEvent* e)
{
    switch(e->type())
    {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        {
            auto* me = static_cast<QMouseEvent*>(e);

            if(auto* lw = qobject_cast<QListWidget*>(target->parent()))
            {
                if(e->type() == QEvent::MouseButtonPress)
                {
                    return std::make_unique<PreInteractionModelViewSelect>(
                        reinterpret_cast<intptr_t>(target),
                        find(lw),
                        Qt::KeyboardModifier::NoModifier,
                        ModelViewSelect,
                        lw->itemAt(me->pos())->text()
                    );
                }

                return nullptr;
            }

            auto* lv = qobject_cast<QListView*>(target->parent());
            if(auto* cb = findAncestor<QComboBox>(target); cb != nullptr && lv != nullptr)
            {
                if(e->type() == QEvent::MouseButtonRelease)
                {
                    return std::make_unique<PreInteractionModelViewSelect>(
                        reinterpret_cast<intptr_t>(cb),
                        find(cb),
                        Qt::KeyboardModifier::NoModifier,
                        ModelViewSelect,
                        cb->model()->data(lv->indexAt(me->pos())).toString()
                    );
                }

                return nullptr;
            }

            if(auto* slider = qobject_cast<QSlider*>(target))
            {
                if(e->type() == QEvent::MouseButtonRelease)
                {
                    bool dragInProgress = m_dragInProgress;
                    m_dragInProgress = false;
                    QRect decrementZone = slider->rect();
                    QRect incrementZone = slider->rect();
                    if(slider->orientation() == Qt::Horizontal)
                    {
                        decrementZone.setWidth(static_cast<int>(decrementZone.width() * 0.25));
                        incrementZone.setX(static_cast<int>(incrementZone.width() * 0.75));
                    }
                    else if(slider->orientation() == Qt::Vertical)
                    {
                        decrementZone.setHeight(static_cast<int>(decrementZone.height() * 0.25));
                        incrementZone.setY(static_cast<int>(decrementZone.height() * 0.75));
                    }

                    if(!dragInProgress && (decrementZone.contains(me->pos()) || incrementZone.contains(me->pos())))
                    {
                        if(decrementZone.contains(me->pos()))
                        {
                            return std::make_unique<PreInteractionNumberInputModification>(
                                reinterpret_cast<intptr_t>(
                                    target),
                                find(slider),
                                me->modifiers(),
                                NumberInputModification,
                                ModificationType::DECREMENT,
                                1
                            );
                        }

                        if(incrementZone.contains(me->pos()))
                        {
                            return std::make_unique<PreInteractionNumberInputModification>(
                                reinterpret_cast<intptr_t>(
                                    target),
                                find(slider),
                                me->modifiers(),
                                NumberInputModification,
                                ModificationType::INCREMENT,
                                1
                            );
                        }
                    }
                    else
                    {
                        return std::make_unique<PreInteractionNumberInputModification>(
                            reinterpret_cast<intptr_t>(target),
                            find(slider),
                            me->modifiers(),
                            NumberInputModification,
                            ModificationType::SET,
                            slider->value()
                        );
                    }
                }
                else if(e->type() == QEvent::MouseMove)
                {
                    m_dragInProgress = true;
                }
                else if(e->type() == QEvent::MouseButtonPress)
                {
                    m_dragInProgress = false;
                }

                return nullptr;
            }

            if(auto* spinbox = qobject_cast<QAbstractSpinBox*>(target))
            {
                if(e->type() == QEvent::MouseButtonRelease)
                {
                    QRect decrementZone = spinbox->rect();
                    QRect incrementZone = spinbox->rect();
                    decrementZone.setWidth(static_cast<int>(decrementZone.width() * 0.25));
                    incrementZone.setX(static_cast<int>(incrementZone.width() * 0.75));
                    if(decrementZone.contains(me->pos()))
                    {
                        return std::make_unique<PreInteractionNumberInputModification>(
                            reinterpret_cast<intptr_t>(target),
                            find(spinbox),
                            me->modifiers(),
                            NumberInputModification,
                            ModificationType::DECREMENT,
                            1
                        );
                    }

                    if(incrementZone.contains(me->pos()))
                    {
                        return std::make_unique<PreInteractionNumberInputModification>(
                            reinterpret_cast<intptr_t>(target),
                            find(spinbox),
                            me->modifiers(),
                            NumberInputModification,
                            ModificationType::INCREMENT,
                            1
                        );
                    }
                }

                return nullptr;
            }

            if(auto* button = qobject_cast<QPushButton*>(target);
               e->type() == QEvent::MouseButtonRelease && button != nullptr && qobject_cast<QColorDialog*>(
                   button->window()
               ) != nullptr)
            {
                return std::make_unique<PreInteractionModelViewSelect>(
                    reinterpret_cast<intptr_t>(button->window()),
                    find(button->window()),
                    me->modifiers(),
                    ModelViewSelect,
                    button->window()->findChild<QLineEdit*>()->text()
                );
            }

            if(auto* checkbox = qobject_cast<QCheckBox*>(
                   target
            ); checkbox != nullptr && checkbox->window()->objectName() == "preferences_configuration")
            {
                // Exception for Preferences Configuration, in order to add it in helper::PreferencesConfiguration::fill
                // map
                if(e->type() == QEvent::MouseButtonRelease)
                {
                    return std::make_unique<PreInteractionModelViewSelect>(
                        reinterpret_cast<intptr_t>(checkbox),
                        find(checkbox),
                        me->modifiers(),
                        ModelViewSelect,
                        checkbox->isChecked() ? "true" : "false"
                    );
                }

                return nullptr;
            }

            if(e->type() == QEvent::MouseButtonDblClick
               && target->objectName() == m_mainWindow->windowTitle() + "Window")
            {
                return nullptr;
            }

            return std::make_unique<PreInteractionMouse>(
                reinterpret_cast<intptr_t>(target),
                find(target),
                me->modifiers(),
                e->type(),
                me->pos(),
                me->pos(),
                me->button()
            );
        }

        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            auto* ke          = static_cast<QKeyEvent*>(e);
            QObject* receiver = nullptr;
            if(qApp->focusWidget() == nullptr)
            {
                receiver = target;
            }
            else
            {
                receiver = qApp->focusWidget();
            }

            if(ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_Control || ke->key() == Qt::Key_Meta
               || ke->key() == Qt::Key_Alt || ke->key() == Qt::Key_AltGr)
            {
                // Ignore modifier keys
                return nullptr;
            }

            if(e->type() == QEvent::KeyPress && !m_interactions.empty()
               && m_interactions.back()->type == QEvent::KeyPress
               && static_cast<PreInteractionKeyboard&>(*m_interactions.back()).key == ke->key()
               && !ke->isAutoRepeat())
            {
                // Ignore duplicates (if the same key is pressed multiple times in a row without any releases, it must
                // be an auto repeat, else it means the event was duplicated).
                return nullptr;
            }

            return std::make_unique<PreInteractionKeyboard>(
                reinterpret_cast<intptr_t>(receiver),
                find(receiver),
                ke->modifiers(),
                e->type(),
                static_cast<Qt::Key>(ke->key()),
                ke->text()
            );
        }

        case QEvent::Wheel:
        {
            auto* we = static_cast<QWheelEvent*>(e);
            return std::make_unique<PreInteractionMouseWheel>(
                reinterpret_cast<intptr_t>(target),
                find(target),
                we->modifiers(),
                e->type(),
                we->angleDelta(),
                we->position().toPoint()
            );
        }

        default:
            return nullptr;
    }
}
