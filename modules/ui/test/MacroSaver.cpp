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

QString modifiers_to_string(Qt::KeyboardModifiers _modifiers)
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
    if(_modifiers == Qt::KeyboardModifier::NoModifier)
    {
        res = "Qt::KeyboardModifier::NoModifier";
    }
    else
    {
        for(const auto& [name, value] : modifiersList)
        {
            if((_modifiers & value) != 0U)
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
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers
) :
    receiverId(_receiver_id),
    howToFindReceiver(_how_to_find_receiver),
    modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

PreInteraction::PreInteraction(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type
) :
    MacroInteraction(_receiver_id, _how_to_find_receiver, _modifiers),
    type(_type)
{
}

//------------------------------------------------------------------------------

PostInteraction::PostInteraction(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type
) :
    MacroInteraction(_receiver_id, _how_to_find_receiver, _modifiers),
    type(_type)
{
}

//------------------------------------------------------------------------------

InteractionMouse::InteractionMouse(QPoint _from, QPoint _to, Qt::MouseButton _button) :
    from(_from),
    to(_to),
    button(_button)
{
}

InteractionMouseWheel::InteractionMouseWheel(QPoint _angle_delta, QPoint _pos) :
    angleDelta(_angle_delta),
    pos(_pos)
{
}

//------------------------------------------------------------------------------

InteractionKeyboard::InteractionKeyboard(Qt::Key _key, QString _sequence) :
    key(_key),
    sequence(std::move(_sequence))
{
}

//------------------------------------------------------------------------------

bool InteractionKeyboard::isPrintable() const
{
    return !sequence.isEmpty() && sequence[0].isPrint();
}

//------------------------------------------------------------------------------

InteractionModelViewSelect::InteractionModelViewSelect(QString _name) :
    name(std::move(_name))
{
}

NumberInputModification::NumberInputModification(modification_t _type, double _number) :
    modifType(_type),
    modifNumber(_number)
{
}

//------------------------------------------------------------------------------

PreInteractionMouse::PreInteractionMouse(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    QPoint _from,
    QPoint _to,
    Qt::MouseButton _button
) :
    PreInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionMouse(_from, _to, _button)
{
}

PreInteractionMouseWheel::PreInteractionMouseWheel(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    QPoint _angle_delta,
    QPoint _pos
) :
    PreInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionMouseWheel(_angle_delta, _pos)
{
}

//------------------------------------------------------------------------------

PreInteractionKeyboard::PreInteractionKeyboard(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    Qt::Key _key,
    const QString& _sequence
) :
    PreInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionKeyboard(_key, _sequence)
{
}

//------------------------------------------------------------------------------

PreInteractionModelViewSelect::PreInteractionModelViewSelect(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    const QString& _name
) :
    PreInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionModelViewSelect(_name)
{
}

PreInteractionNumberInputModification::PreInteractionNumberInputModification(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    modification_t _modif_type,
    double _modif_number
) :
    PreInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    NumberInputModification(_modif_type, _modif_number)
{
}

//------------------------------------------------------------------------------

PostInteractionMouse::PostInteractionMouse(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    QPoint _from,
    QPoint _to,
    Qt::MouseButton _button
) :
    PostInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionMouse(_from, _to, _button)
{
}

PostInteractionMouseWheel::PostInteractionMouseWheel(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    QPoint _angle_delta,
    QPoint _pos
) :
    PostInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionMouseWheel(_angle_delta, _pos)
{
}

//------------------------------------------------------------------------------

PostInteractionKeyboard::PostInteractionKeyboard(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    Qt::Key _key,
    const QString& _sequence
) :
    PostInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionKeyboard(_key, _sequence)
{
}

//------------------------------------------------------------------------------

PostInteractionModelViewSelect::PostInteractionModelViewSelect(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    const QString& _name
) :
    PostInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    InteractionModelViewSelect(_name)
{
}

PostInteractionNumberInputModification::PostInteractionNumberInputModification(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    modification_t _modif_type,
    double _modif_number
) :
    PostInteraction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    NumberInputModification(_modif_type, _modif_number)
{
}

InteractionHelperAPI::InteractionHelperAPI(
    intptr_t _receiver_id,
    const QVector<FindStrategy>& _how_to_find_receiver,
    QString _method_name,
    std::optional<sight::ui::test_core::helper::Select> _select,
    QStringList _args
) :
    PostInteraction(_receiver_id, _how_to_find_receiver, {}, HELPER_API), methodName(std::move(_method_name)),
    select(std::move(_select)), args(std::move(_args))
{
}

//------------------------------------------------------------------------------

bool MacroSaver::eventFilter(QObject* _target, QEvent* _e)
{
    if(_e->type() == QEvent::ChildAdded)
    {
        auto* ce = dynamic_cast<QChildEvent*>(_e);
        infect(ce->child());
        if(m_mainWindow == nullptr)
        {
            m_mainWindow = qApp->activeWindow();
        }
    }
    else if(_e->type() == QEvent::MouseButtonPress || _e->type() == QEvent::MouseButtonRelease
            || _e->type() == QEvent::MouseMove || _e->type() == QEvent::MouseButtonDblClick
            || _e->type() == QEvent::Wheel
            || _e->type() == QEvent::KeyPress || _e->type() == QEvent::KeyRelease)
    {
        // Ignore the MouseMove event if no buttons are pressed, else we will get one thousand of events
        QWidget* w = qobject_cast<QWidget*>(_target);
        if((_e->type() != QEvent::MouseMove || dynamic_cast<QMouseEvent*>(_e)->buttons() != Qt::MouseButton::NoButton)
           && !(w != nullptr && w->isWindow()) && _target != m_mainWindow)
        {
            std::unique_ptr<PreInteraction> interaction = createInteraction(_target, _e);
            if(interaction != nullptr)
            {
                m_interactions.emplace_back(std::move(interaction));
            }
        }
    }

    return QObject::eventFilter(_target, _e);
}

//------------------------------------------------------------------------------

void MacroSaver::infect(QObject* _o)
{
    _o->installEventFilter(this);
    for(QObject* child : _o->children())
    {
        infect(child);
    }
}

//------------------------------------------------------------------------------

QObject* MacroSaver::findChild(
    QObject* _o,
    const QString& _type,
    const QString& _object_name,
    Qt::FindChildOptions _options
)
{
    for(QObject* child : _o->children())
    {
        if(child->inherits(_type.toLatin1()) && (_object_name == "" || child->objectName() == _object_name))
        {
            return child;
        }

        if(_options == Qt::FindChildrenRecursively)
        {
            QObject* rec = findChild(child, _type, _object_name, _options);
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
    QObject* _o,
    const QString& _type,
    const QString& _object_name,
    Qt::FindChildOptions _options
)
{
    QObjectList res;
    for(QObject* child : _o->children())
    {
        if(child->inherits(_type.toLatin1()) && (_object_name == "" || child->objectName() == _object_name))
        {
            res.append(child);
        }

        if(_options == Qt::FindChildrenRecursively)
        {
            res.append(findChildren(child, _type, _object_name, _options));
        }
    }

    return res;
}

//------------------------------------------------------------------------------

static QObject* parent_of(const QObject* _obj)
{
    QObject* parent = _obj->parent();
    if(parent != nullptr && parent->objectName() == "qt_scrollarea_viewport")
    {
        parent = parent->parent();
    }

    return parent;
}

//------------------------------------------------------------------------------

QVector<FindStrategy> MacroSaver::find(QObject* _o)
{
    assert(_o != nullptr);
    QWidget* w            = qobject_cast<QWidget*>(_o);
    QByteArray class_name = _o->metaObject()->className();
    if(class_name == "QWidgetWindow" || class_name == "QColorPicker") // Those are private classes of Qt, we can't use
                                                                      // them as is.
    {
        class_name = "QWidget";
    }

    if(_o == m_mainWindow)
    {
        return {{find_strategy_t::ROOT, class_name, "", 0}};
    }

    if(_o == qApp->activeModalWidget())
    {
        return {{find_strategy_t::ACTIVE_MODAL_WIDGET, class_name,
            w->objectName().isEmpty() ? w->windowTitle() : w->objectName(), 0
        }
        };
    }

    if(_o->objectName() != "")
    {
        QVector<FindStrategy> res {{find_strategy_t::OBJECT_NAME, class_name, _o->objectName().toLatin1(), 0}};
        if(QObjectList children;
           w != nullptr
           && (children = findChildren(w->window(), class_name, _o->objectName())).size() == 1 && children[0] == _o)
        {
            res.append(find(w->window()));
        }
        else if(QObjectList children2 = findChildren(m_mainWindow, class_name, _o->objectName());
                children2.size() == 1 && children2[0] == _o)
        {
            res.append(find(m_mainWindow));
        }
        else
        {
            QVector<FindStrategy> best_ancestor_strat;
            for(QObject* current_ancestor = parent_of(_o) ;
                current_ancestor != nullptr ;
                current_ancestor = parent_of(current_ancestor))
            {
                if(QObjectList children3 =
                       findChildren(current_ancestor, class_name, _o->objectName());
                   children3.size() == 1 && children3[0] == _o)
                {
                    QVector<FindStrategy> current_ancestor_strat = find(current_ancestor);
                    if(current_ancestor_strat.back().type != find_strategy_t::CANT_BE_FOUND
                       && (best_ancestor_strat.empty() || current_ancestor_strat.size() < best_ancestor_strat.size()))
                    {
                        best_ancestor_strat = current_ancestor_strat;
                    }
                }
            }

            if(best_ancestor_strat.empty())
            {
                res.append({find_strategy_t::CANT_BE_FOUND, class_name, "", 0});
            }
            else
            {
                res.append(best_ancestor_strat);
            }
        }

        return res;
    }

    if(findChildren(m_mainWindow, class_name).size() == 1)
    {
        return {{find_strategy_t::GLOBAL_TYPE, class_name, "", 0}};
    }

    if(w != nullptr && w->actions().size() == 1)
    {
        QVector<FindStrategy> res {{find_strategy_t::ACTION, class_name, "", 0}};
        res.append(find(w->actions()[0]));
        return res;
    }

    if(parent_of(_o) == nullptr)
    {
        return {{find_strategy_t::CANT_BE_FOUND, class_name, "", 0}};
    }

    if(findChildren(parent_of(_o), class_name, "", Qt::FindDirectChildrenOnly).size() == 1)
    {
        QVector<FindStrategy> res {{find_strategy_t::LOCAL_TYPE, class_name, "", 0}};
        res.append(find(parent_of(_o)));
        return res;
    } // else

    int index = 0;
    for( ; index < parent_of(_o)->children().size() ; index++)
    {
        if(parent_of(_o)->children()[index] == _o)
        {
            break;
        }
    }

    QVector<FindStrategy> res {{find_strategy_t::NTH_CHILD, class_name, "", index}};
    res.append(find(parent_of(_o)));
    return res;
}

//------------------------------------------------------------------------------

static QString select_to_code(const sight::ui::test_core::helper::Select& _select)
{
    switch(_select.type())
    {
        case sight::ui::test_core::helper::Select::Type::FROM_MAIN:
            return QString("\"%1\"").arg(std::get<std::string>(_select.data()).c_str());

        case sight::ui::test_core::helper::Select::Type::FROM_DIALOG:
            return QString("Select::fromDialog(\"%1\")").arg(std::get<std::string>(_select.data()).c_str());

        case sight::ui::test_core::helper::Select::Type::FROM_PARENT:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(_select.data());
            return QString(R"(Select::fromParent("%1", "%2"))").arg(parentName.c_str()).arg(childName.c_str());
        }

        case sight::ui::test_core::helper::Select::Type::FROM_CURRENT:
            return QString("Select::fromCurrent(\"%1\")").arg(std::get<std::string>(_select.data()).c_str());

        case sight::ui::test_core::helper::Select::Type::CURRENT:
            return "Select::current()";

        case sight::ui::test_core::helper::Select::Type::DIALOG:
            return "Select::dialog()";
    }

    SIGHT_ASSERT(
        "Invalid Select type",
        0
    );
    return "";
}

//------------------------------------------------------------------------------

[[nodiscard]] static std::pair<QVector<FindStrategy>, sight::ui::test_core::helper::Select> compute_select(
    QVector<FindStrategy> _how_to_find_receiver
)
{
    sight::ui::test_core::helper::Select select = sight::ui::test_core::helper::Select::current();
    if(_how_to_find_receiver.size() >= 2 && _how_to_find_receiver[0].type == find_strategy_t::OBJECT_NAME
       && _how_to_find_receiver[1].type == find_strategy_t::ROOT)
    {
        select = sight::ui::test_core::helper::Select(_how_to_find_receiver[0].string.toStdString());
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
    }
    else if(_how_to_find_receiver.size() >= 2 && _how_to_find_receiver[0].type == find_strategy_t::OBJECT_NAME
            && _how_to_find_receiver[1].type == find_strategy_t::ACTIVE_MODAL_WIDGET)
    {
        select = sight::ui::test_core::helper::Select::fromDialog(_how_to_find_receiver[0].string.toStdString());
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
    }
    else if(_how_to_find_receiver.size() >= 2 && _how_to_find_receiver[0].type == find_strategy_t::OBJECT_NAME
            && _how_to_find_receiver[1].type == find_strategy_t::OBJECT_NAME
            && _how_to_find_receiver[2].type == find_strategy_t::ROOT)
    {
        select = sight::ui::test_core::helper::Select::fromParent(
            _how_to_find_receiver[1].string.toStdString(),
            _how_to_find_receiver[0].string.toStdString()
        );
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
    }
    else if(!_how_to_find_receiver.empty() && _how_to_find_receiver[0].type == find_strategy_t::ACTIVE_MODAL_WIDGET)
    {
        select = sight::ui::test_core::helper::Select::dialog();
        _how_to_find_receiver.pop_front();
    }

    return {_how_to_find_receiver, select};
}

//------------------------------------------------------------------------------

void MacroSaver::save()
{
    // Interactions preprocessing
    std::vector<std::unique_ptr<PostInteraction> > pre_post_interactions;
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
                pre_post_interactions.emplace_back(
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
                    pre_post_interactions.emplace_back(
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
            if(pre_post_interactions.back()->type == MOUSE_DOUBLE_CLICK)
            {
                // This is a duplicated event, ignore it
                continue;
            }

            if(pre_post_interactions.back()->type == MOUSE_CLICK)
            {
                // Ignore the click that generated this double click
                pre_post_interactions.pop_back();
            }

            const auto& im = static_cast<PreInteractionMouse&>(*m_interactions[i]);
            pre_post_interactions.emplace_back(
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
            const auto& imw    = static_cast<PreInteractionMouseWheel&>(*m_interactions[i]);
            QPoint angle_delta = imw.angleDelta / 2; // Divide by two because events are duplicated
            // Compress all mouse wheel events into one
            while(i + 1 < m_interactions.size() && m_interactions[i + 1]->type == QEvent::Type::Wheel)
            {
                i++;
                angle_delta += static_cast<PreInteractionMouseWheel&>(*m_interactions[i]).angleDelta / 2;
            }

            pre_post_interactions.emplace_back(
                std::make_unique<PostInteractionMouseWheel>(
                    imw.receiverId,
                    imw.howToFindReceiver,
                    imw.modifiers,
                    MOUSE_WHEEL,
                    angle_delta,
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
                    pre_post_interactions.emplace_back(
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
                    pre_post_interactions.emplace_back(
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
            pre_post_interactions.emplace_back(
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
            pre_post_interactions.emplace_back(
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

    std::vector<std::unique_ptr<PostInteraction> > post_interactions;
    for(std::size_t i = 0 ; i < pre_post_interactions.size() ; i++)
    {
        if(pre_post_interactions[i]->type == MOUSE_CLICK
           && (pre_post_interactions[i]->howToFindReceiver[0].className == "QPushButton"
               || pre_post_interactions[i]->howToFindReceiver[0].className == "QToolButton"))
        {
            if(pre_post_interactions[i]->howToFindReceiver.back().string == "preferences_configuration")
            {
                // Special case: preferences Configuration pop up close button
                if(InteractionHelperAPI* iha =
                       nullptr;
                   post_interactions.back()->type == HELPER_API
                   && (iha = static_cast<InteractionHelperAPI*>(post_interactions.back().get()))->methodName
                   == "PreferencesConfiguration::fill")
                {
                    iha->args[0] += "}";
                }
                else
                {
                    post_interactions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            pre_post_interactions[i]->
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
                QVector<FindStrategy> how_to_find_receiver  = pre_post_interactions[i]->howToFindReceiver;
                sight::ui::test_core::helper::Select select = sight::ui::test_core::helper::Select::current();
                if(how_to_find_receiver.front().type == find_strategy_t::ACTION)
                {
                    // helper::Button::push already handles the "action" case.
                    how_to_find_receiver.pop_front();
                }

                std::tie(how_to_find_receiver, select) = compute_select(how_to_find_receiver);
                post_interactions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        pre_post_interactions[i]->receiverId,
                        how_to_find_receiver,
                        "Button::push",
                        select
                    )
                );
            }
        }
        else if(pre_post_interactions[i]->type == MOUSE_CLICK
                && pre_post_interactions[i]->howToFindReceiver[0].className == "QCheckBox")
        {
            auto [howToFindReceiver, select] = compute_select(pre_post_interactions[i]->howToFindReceiver);
            post_interactions.emplace_back(
                std::make_unique<InteractionHelperAPI>(
                    pre_post_interactions[i]->receiverId,
                    howToFindReceiver,
                    "CheckBox::toggle",
                    select
                )
            );
        }
        else if(pre_post_interactions[i]->type == MODEL_VIEW_SELECT
                && (pre_post_interactions[i]->howToFindReceiver[0].className == "QCheckBox"
                    || pre_post_interactions[i]->howToFindReceiver[0].className == "QComboBox")
                && pre_post_interactions[i]->howToFindReceiver.back().string == "preferences_configuration")
        {
            const auto& imvs = static_cast<const PostInteractionModelViewSelect&>(*pre_post_interactions[i]);
            QString arg      = QString(R"({"%1", "%2"})").arg(imvs.howToFindReceiver[0].string).arg(imvs.name);
            if(InteractionHelperAPI* iha =
                   nullptr;
               post_interactions.back()->type == HELPER_API
               && (iha = static_cast<InteractionHelperAPI*>(post_interactions.back().get()))->methodName
               == "PreferencesConfiguration::fill")
            {
                if(!iha->args[0].contains(QString("\"%1\"").arg(pre_post_interactions[i]->howToFindReceiver[0].string)))
                {
                    iha->args[0] += QString(", " + arg);
                }
            }
            else
            {
                post_interactions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        pre_post_interactions[i]->receiverId,
                        QVector<FindStrategy> {},
                        "PreferencesConfiguration::fill",
                        std::nullopt,
                        QStringList {"{" + arg
                        })
                );
            }
        }
        else if(pre_post_interactions[i]->type == MODEL_VIEW_SELECT
                && pre_post_interactions[i]->howToFindReceiver[0].className == "QComboBox")
        {
            if(!post_interactions.empty() && post_interactions.back()->type == MOUSE_DRAG)
            {
                // It seems mouse drags are always duplicated with comboboxes
                post_interactions.pop_back();
            }
            else if(post_interactions.size() >= 2 && post_interactions.back()->type == MOUSE_CLICK
                    && post_interactions[post_interactions.size() - 2]->type == MOUSE_CLICK)
            {
                // It seems mouse clicks are always duplicated with comboboxes
                post_interactions.pop_back();
                post_interactions.pop_back();
            }
            else if(!post_interactions.empty() && post_interactions.back()->type == HELPER_API)
            {
                // It seems mouse clicks are always duplicated with comboboxes
                continue;
            }

            const auto& imvs = static_cast<PostInteractionModelViewSelect&>(*pre_post_interactions[i]);
            auto [howToFindReceiver, select] = compute_select(imvs.howToFindReceiver);
            post_interactions.emplace_back(
                std::make_unique<InteractionHelperAPI>(
                    imvs.receiverId,
                    howToFindReceiver,
                    "ComboBox::select",
                    select,
                    QStringList {QString("\"%1\"").arg(imvs.name)
                    })
            );
        }
        else if((pre_post_interactions[i]->type == KEYBOARD_CLICK
                 || pre_post_interactions[i]->type == KEYBOARD_SEQUENCE)
                && pre_post_interactions[i]->howToFindReceiver[0].className == "QLineEdit")
        {
            const auto& ik = static_cast<PostInteractionKeyboard&>(*pre_post_interactions[i]);
            if(pre_post_interactions[i]->howToFindReceiver.back().type == find_strategy_t::ACTIVE_MODAL_WIDGET)
            {
                // Compress all keyboard sequence/click in one
                QString sequence = (ik.isPrintable() ? ik.sequence : "");
                for( ;
                     i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->receiverId == ik.receiverId
                     && (pre_post_interactions[i + 1]->type == KEYBOARD_CLICK
                         || pre_post_interactions[i + 1]->type == KEYBOARD_SEQUENCE) ;
                     i++)
                {
                    const auto& ik2 = static_cast<PostInteractionKeyboard&>(*pre_post_interactions[i + 1]);
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

                if(pre_post_interactions[i]->howToFindReceiver.back().className == "QFileDialog")
                {
                    post_interactions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            ik.receiverId,
                            QVector<FindStrategy> {},
                            "FileDialog::fill",
                            std::nullopt,
                            QStringList {QString("\"%1\"").arg(sequence)
                            })
                    );
                    if(i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->receiverId == ik.receiverId
                       && pre_post_interactions[i + 1]->type == KEYBOARD_CLICK
                       && static_cast<PostInteractionKeyboard&>(*pre_post_interactions[i + 1]).key
                       == Qt::Key::Key_Return)
                    {
                        // Ignore the Enter key press to confirm the file dialog, as it is already handled by
                        // helper::FieldDialog::fill
                        i++;
                    }
                }
                else if(pre_post_interactions[i]->howToFindReceiver.back().string == "preferences_configuration")
                {
                    QString arg =
                        QString(R"({"%1", "%2"})").arg(pre_post_interactions[i]->howToFindReceiver[0].string).arg(
                            sequence
                        );
                    if(InteractionHelperAPI* iha =
                           nullptr;
                       post_interactions.back()->type == HELPER_API
                       && (iha = static_cast<InteractionHelperAPI*>(post_interactions.back().get()))->methodName
                       == "PreferencesConfiguration::fill")
                    {
                        if(!iha->args[0].contains(
                               QString("\"%1\"").arg(
                                   pre_post_interactions[i]->howToFindReceiver[0].
                                   string
                               )
                        ))
                        {
                            iha->args[0] += QString(", " + arg);
                        }
                    }
                    else
                    {
                        post_interactions.emplace_back(
                            std::make_unique<InteractionHelperAPI>(
                                pre_post_interactions[i]->
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
                auto [howToFindReceiver, select] = compute_select(ik.howToFindReceiver);
                post_interactions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        ik.receiverId,
                        howToFindReceiver,
                        "field::fill",
                        select,
                        QStringList {QString("\"%1\"").arg(ik.sequence)
                        })
                );
            }
        }
        else if(pre_post_interactions[i]->type == MODEL_VIEW_SELECT
                && pre_post_interactions[i]->howToFindReceiver[0].className == "QListWidget")
        {
            const auto& ilwc = static_cast<PostInteractionModelViewSelect&>(*pre_post_interactions[i]);
            if(pre_post_interactions[i]->howToFindReceiver.back().type == find_strategy_t::ACTIVE_MODAL_WIDGET
               && pre_post_interactions[i]->howToFindReceiver.back().string == "selector")
            {
                if(post_interactions.back()->type == HELPER_API
                   && static_cast<InteractionHelperAPI&>(*post_interactions.back()).methodName
                   == "selector::select"
                   && post_interactions.back()->receiverId == pre_post_interactions[i]->receiverId)
                {
                    // The user selected an item then selected another one; take only the last choice
                    post_interactions.pop_back();
                }

                post_interactions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        ilwc.receiverId,
                        QVector<FindStrategy> {},
                        "selector::select",
                        std::nullopt,
                        QStringList {QString("\"%1\"").arg(ilwc.name)
                        })
                );
                if(i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->type == MOUSE_CLICK
                   && pre_post_interactions[i + 1]->howToFindReceiver[0].className == "QPushButton"
                   && pre_post_interactions[i + 1]->howToFindReceiver[0].string == "Ok"
                   && pre_post_interactions[i + 1]->howToFindReceiver[1].type == find_strategy_t::ACTIVE_MODAL_WIDGET)
                {
                    // Ignore the eventual "Ok" button click as it is handled by helper::selector::select
                    i++;
                }

                if(i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->type == MOUSE_DOUBLE_CLICK
                   && pre_post_interactions[i + 1]->howToFindReceiver[0].string == "SelectorDialogWindow")
                {
                    // It seems there might be a duplicated double click event
                    i++;
                }
            }
            else
            {
                auto [howToFindReceiver, select] = compute_select(ilwc.howToFindReceiver);
                post_interactions.emplace_back(
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
        else if(pre_post_interactions[i]->type == NUMBER_INPUT_MODIFICATION
                && pre_post_interactions[i]->howToFindReceiver[0].className == "QSlider")
        {
            if(!post_interactions.empty()
               && (post_interactions.back()->type == MOUSE_DRAG || post_interactions.back()->type == MOUSE_CLICK
                   || post_interactions.back()->type == MOUSE_DOUBLE_CLICK))
            {
                // It seems mouse drags are always duplicated with sliders
                post_interactions.pop_back();
            }
            else if(!post_interactions.empty() && post_interactions[post_interactions.size() - 1]->type == HELPER_API)
            {
                // It seems mouse drags are always duplicated with sliders
                continue;
            }

            const auto& nim =
                static_cast<PostInteractionNumberInputModification&>(*pre_post_interactions[i]);
            QVector<FindStrategy> how_to_find_receiver  = nim.howToFindReceiver;
            sight::ui::test_core::helper::Select select = sight::ui::test_core::helper::Select::current();
            if(how_to_find_receiver.front().type == find_strategy_t::LOCAL_TYPE
               && how_to_find_receiver.front().className == "QSlider")
            {
                // Finding the actual QSlider is already handled in helper::Slider::set
                how_to_find_receiver.pop_front();
            }

            std::tie(how_to_find_receiver, select) = compute_select(how_to_find_receiver);
            if(nim.modifType == modification_t::SET)
            {
                post_interactions.emplace_back(
                    std::make_unique<InteractionHelperAPI>(
                        nim.receiverId,
                        how_to_find_receiver,
                        "Slider::set",
                        select,
                        QStringList {QString::number(nim.modifNumber)
                        })
                );
            }
            else
            {
                QString position;
                if(nim.modifType == modification_t::DECREMENT)
                {
                    position = "helper::Slider::Position::LEFT";
                }
                else if(nim.modifType == modification_t::INCREMENT)
                {
                    position = "helper::Slider::Position::RIGHT";
                }

                if(InteractionHelperAPI* previous_interaction =
                       nullptr;
                   !post_interactions.empty() && post_interactions.back()->type == HELPER_API
                   && post_interactions.back()->receiverId == nim.receiverId
                   && (previous_interaction =
                           static_cast<InteractionHelperAPI*>(post_interactions.back().get()))->methodName
                   == "Slider::move"
                   && previous_interaction->args[0] == position)
                {
                    // If the previous interaction is the same, we compress it with the current one
                    if(previous_interaction->args.size() == 1)
                    {
                        previous_interaction->args.push_back("2");
                    }
                    else
                    {
                        previous_interaction->args[1] = QString::number(previous_interaction->args[1].toFloat() + 1);
                    }
                }
                else
                {
                    post_interactions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            nim.receiverId,
                            how_to_find_receiver,
                            "Slider::move",
                            select,
                            QStringList {position
                            })
                    );
                }
            }
        }
        else if(pre_post_interactions[i]->type == NUMBER_INPUT_MODIFICATION
                && (pre_post_interactions[i]->howToFindReceiver[0].className == "QSpinBox"
                    || pre_post_interactions[i]->howToFindReceiver[0].className == "QDoubleSpinBox"))
        {
            if(!post_interactions.empty() && post_interactions.back()->type == MOUSE_CLICK)
            {
                // It seems mouse clicks are always duplicated with spinboxes
                post_interactions.pop_back();
            }
            else if(!post_interactions.empty() && post_interactions.back()->type == HELPER_API)
            {
                // It seems mouse clicks are always duplicated with spinboxes
                continue;
            }

            const auto& nim = static_cast<PostInteractionNumberInputModification&>(*pre_post_interactions[i]);
            auto [howToFindReceiver, select] = compute_select(nim.howToFindReceiver);
            if(nim.modifType == modification_t::SET)
            {
                assert(0);
            }
            else
            {
                QString method_name;
                if(nim.modifType == modification_t::DECREMENT)
                {
                    method_name = "SpinBox::decrement";
                }
                else if(nim.modifType == modification_t::INCREMENT)
                {
                    method_name = "SpinBox::increment";
                }

                if(InteractionHelperAPI* previous_interaction =
                       nullptr;
                   !post_interactions.empty() && post_interactions.back()->type == HELPER_API
                   && post_interactions.back()->receiverId == nim.receiverId
                   && (previous_interaction =
                           static_cast<InteractionHelperAPI*>(post_interactions.back().get()))->methodName
                   == method_name)
                {
                    if(previous_interaction->args.empty())
                    {
                        previous_interaction->args.push_back("2");
                    }
                    else
                    {
                        previous_interaction->args[0] = QString::number(previous_interaction->args[0].toFloat() + 1);
                    }
                }
                else
                {
                    post_interactions.emplace_back(
                        std::make_unique<InteractionHelperAPI>(
                            nim.receiverId,
                            howToFindReceiver,
                            method_name,
                            select
                        )
                    );
                }
            }
        }
        else if(pre_post_interactions[i]->type == MODEL_VIEW_SELECT
                && pre_post_interactions[i]->howToFindReceiver[0].className == "QColorDialog")
        {
            // Ignore all interactions inside the color dialog
            while(std::ranges::any_of(
                      post_interactions.back()->howToFindReceiver,
                      [](const FindStrategy& _s)
            {
                return _s.className == "QColorDialog";
            }))
            {
                post_interactions.pop_back();
            }

            if((post_interactions.back()->type == MOUSE_CLICK || post_interactions.back()->type == HELPER_API)
               && !post_interactions.back()->howToFindReceiver.empty()
               && post_interactions.back()->howToFindReceiver[0].className == "QPushButton")
            {
                // Get rid of duplicate button click
                post_interactions.pop_back();
            }

            QVector<FindStrategy> how_to_find_receiver;
            std::optional<sight::ui::test_core::helper::Select> select;
            if(InteractionHelperAPI* iha =
                   nullptr;
               post_interactions.back()->type == HELPER_API
               && (iha = static_cast<InteractionHelperAPI*>(post_interactions.back().get()))->methodName
               == "Button::push")
            {
                // helper::ColorParameter::select already clicks on the Color button
                how_to_find_receiver = iha->howToFindReceiver;
                select               = iha->select;
                post_interactions.pop_back();
            }
            else
            {
                // It's a duplicate, ignore it
                continue;
            }

            post_interactions.emplace_back(
                std::make_unique<InteractionHelperAPI>(
                    pre_post_interactions[i]->receiverId,
                    how_to_find_receiver,
                    "ColorParameter::select",
                    select,
                    QStringList {QString("\"%1\"").arg(
                                     static_cast<PostInteractionModelViewSelect&>(*pre_post_interactions
                                                                                  [i]).name
                    )
                    })
            );
        }
        else if(pre_post_interactions[i]->howToFindReceiver.back().string == "preferences_configuration"
                || std::ranges::any_of(
                    pre_post_interactions[i]->howToFindReceiver,
                    [](const FindStrategy& _f)
        {
            return _f.string == "preferences_configurationWindow";
        }))
        {
            // Ignore other interactions in PreferencesConfiguration
        }
        else
        {
            post_interactions.emplace_back(std::move(pre_post_interactions[i]));
        }
    }

    QStringList dependencies;
    bool use_helpers            = false;
    bool use_select_constructor = false;
    for(const std::unique_ptr<PostInteraction>& interaction : post_interactions)
    {
        for(const FindStrategy& strategy : interaction->howToFindReceiver)
        {
            if(strategy.type == find_strategy_t::GLOBAL_TYPE || strategy.type == find_strategy_t::LOCAL_TYPE)
            {
                dependencies.append(strategy.className);
            }
            else if(strategy.type == find_strategy_t::ACTION)
            {
                dependencies.append("QAction");
            }
            else if(strategy.type == find_strategy_t::ACTIVE_MODAL_WIDGET)
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
            use_helpers = true;
            const auto& iha = static_cast<InteractionHelperAPI&>(*interaction);
            if(iha.select && iha.select.value().type() != sight::ui::test_core::helper::Select::Type::FROM_MAIN)
            {
                use_select_constructor = true;
            }

            dependencies.append(QString("ui/testCore/helper/%1.hpp").arg(iha.methodName.split("::")[0]));
        }
    }

    dependencies.sort();
    dependencies.removeDuplicates();

    static constexpr auto write = [](QFile& _f, int _indentation, const QString& _line)
                                  {
                                      if(_f.write(
                                             QByteArray(
                                                 _indentation,
                                                 ' '
                                             ) + _line.toLatin1() + '\n'
                                         ) < _line.size())
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
    std::filesystem::path absolute_profile_path(sight::core::runtime::get_current_profile()->get_file_path());
    QString profile_path(QString::fromStdString(std::filesystem::relative(absolute_profile_path).string()));
    write(cpp, 4, QString("return cwd / \"%1\";").arg(profile_path).toLatin1());
    write(cpp, 0, "}");
    write(cpp, 0, "");

    write(cpp, 0, "void GuiTest::test()");
    write(cpp, 0, "{");
    write(cpp, 4, "start(\"GuiTest\",");
    write(cpp, 8, "[](sight::ui::testCore::Tester& tester)");
    write(cpp, 4, "{");

    if(use_helpers)
    {
        write(cpp, 8, "namespace helper = sight::ui::testCore::helper;");
    }

    if(use_select_constructor)
    {
        write(cpp, 8, "using Select     = helper::Select;");
    }

    write(cpp, 0, "");

    intptr_t current_item_id = 0;
    for(const std::unique_ptr<PostInteraction>& interaction : post_interactions)
    {
        if(interaction->receiverId != current_item_id)
        {
            // The receiver of this interaction isn't the current item: We must take it.
            if(interaction->howToFindReceiver.size() >= 2
               && interaction->howToFindReceiver.back().type == find_strategy_t::ROOT
               && interaction->howToFindReceiver[interaction->howToFindReceiver.size() - 2].type
               == find_strategy_t::OBJECT_NAME)
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
                if(strat.type == find_strategy_t::ROOT)
                {
                    write(cpp, 8, "tester.take(\"main window\", tester.getMainWindow());");
                }
                else if(strat.type == find_strategy_t::ACTIVE_MODAL_WIDGET)
                {
                    write(
                        cpp,
                        8,
                        QString("helper::Dialog::take<%1*>(tester, \"%2\");").arg(strat.className).arg(strat.string)
                    );
                }
                else if(strat.type == find_strategy_t::OBJECT_NAME)
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
                else if(strat.type == find_strategy_t::GLOBAL_TYPE)
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
                else if(strat.type == find_strategy_t::ACTION)
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
                else if(strat.type == find_strategy_t::LOCAL_TYPE)
                {
                    write(cpp, 8, "tester.yields(");
                    write(cpp, 12, QString("\"%1\",").arg(strat.className));
                    write(cpp, 12, "[](QObject* old) -> QObject*");
                    write(cpp, 8, "{");
                    write(cpp, 12, QString("return old->findChild<%1*>();").arg(strat.className));
                    write(cpp, 8, "});");
                }
                else if(strat.type == find_strategy_t::NTH_CHILD)
                {
                    write(cpp, 8, "tester.yields(");
                    write(cpp, 12, QString("\"%1\",").arg(strat.className));
                    write(cpp, 12, "[](QObject* old) -> QObject*");
                    write(cpp, 8, "{");
                    write(cpp, 12, QString("return old->children()[%1];").arg(strat.integer));
                    write(cpp, 8, "});");
                }
                else if(strat.type == find_strategy_t::CANT_BE_FOUND)
                {
                    write(
                        cpp,
                        8,
                        QString("// TODO: Find the %1. MacroSaver wasn't able to find it automatically.")
                        .arg(strat.className)
                    );
                }
            }

            current_item_id = interaction->receiverId;
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
                .arg(mouseButton.valueToKey(static_cast<int>(im.button))).arg(modifiers_to_string(im.modifiers))
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
                .arg(mouseButton.valueToKey(static_cast<int>(im.button))).arg(modifiers_to_string(im.modifiers))
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
                .arg(mouseButton.valueToKey(static_cast<int>(im.button))).arg(modifiers_to_string(im.modifiers))
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
                .arg(QTest::toString(im.angleDelta)).arg(modifiers_to_string(im.modifiers)).arg(QTest::toString(im.pos))
            );
        }
        else if(interaction->type == KEYBOARD_CLICK)
        {
            const auto& ik = static_cast<PostInteractionKeyboard&>(*interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::testCore::KeyboardClick>(Qt::Key::%1, %2));")
                .arg(key.valueToKey(static_cast<int>(ik.key))).arg(modifiers_to_string(ik.modifiers))
            );
        }
        else if(interaction->type == KEYBOARD_SEQUENCE)
        {
            const auto& ik = static_cast<PostInteractionKeyboard&>(*interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::testCore::KeyboardSequence>(\"%1\", %2));")
                .arg(ik.sequence).arg(modifiers_to_string(ik.modifiers))
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
                    + select_to_code(*ha.select) : ""
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
static T* find_ancestor(QObject* _child)
{
    for(QObject* current_ancestor = _child ;
        current_ancestor != nullptr ;
        current_ancestor = current_ancestor->parent())
    {
        if(auto* ancestor = qobject_cast<T*>(current_ancestor))
        {
            return ancestor;
        }
    }

    return nullptr;
}

//------------------------------------------------------------------------------

std::unique_ptr<PreInteraction> MacroSaver::createInteraction(QObject* _target, QEvent* _e)
{
    switch(_e->type())
    {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        {
            auto* me = static_cast<QMouseEvent*>(_e);

            if(auto* lw = qobject_cast<QListWidget*>(_target->parent()))
            {
                if(_e->type() == QEvent::MouseButtonPress)
                {
                    return std::make_unique<PreInteractionModelViewSelect>(
                        reinterpret_cast<intptr_t>(_target),
                        find(lw),
                        Qt::KeyboardModifier::NoModifier,
                        ModelViewSelect,
                        lw->itemAt(me->pos())->text()
                    );
                }

                return nullptr;
            }

            auto* lv = qobject_cast<QListView*>(_target->parent());
            if(auto* cb = find_ancestor<QComboBox>(_target); cb != nullptr && lv != nullptr)
            {
                if(_e->type() == QEvent::MouseButtonRelease)
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

            if(auto* slider = qobject_cast<QSlider*>(_target))
            {
                if(_e->type() == QEvent::MouseButtonRelease)
                {
                    bool drag_in_progress = m_dragInProgress;
                    m_dragInProgress = false;
                    QRect decrement_zone = slider->rect();
                    QRect increment_zone = slider->rect();
                    if(slider->orientation() == Qt::Horizontal)
                    {
                        decrement_zone.setWidth(static_cast<int>(decrement_zone.width() * 0.25));
                        increment_zone.setX(static_cast<int>(increment_zone.width() * 0.75));
                    }
                    else if(slider->orientation() == Qt::Vertical)
                    {
                        decrement_zone.setHeight(static_cast<int>(decrement_zone.height() * 0.25));
                        increment_zone.setY(static_cast<int>(decrement_zone.height() * 0.75));
                    }

                    if(!drag_in_progress && (decrement_zone.contains(me->pos()) || increment_zone.contains(me->pos())))
                    {
                        if(decrement_zone.contains(me->pos()))
                        {
                            return std::make_unique<PreInteractionNumberInputModification>(
                                reinterpret_cast<intptr_t>(
                                    _target),
                                find(slider),
                                me->modifiers(),
                                NumberInputModification,
                                modification_t::DECREMENT,
                                1
                            );
                        }

                        if(increment_zone.contains(me->pos()))
                        {
                            return std::make_unique<PreInteractionNumberInputModification>(
                                reinterpret_cast<intptr_t>(
                                    _target),
                                find(slider),
                                me->modifiers(),
                                NumberInputModification,
                                modification_t::INCREMENT,
                                1
                            );
                        }
                    }
                    else
                    {
                        return std::make_unique<PreInteractionNumberInputModification>(
                            reinterpret_cast<intptr_t>(_target),
                            find(slider),
                            me->modifiers(),
                            NumberInputModification,
                            modification_t::SET,
                            slider->value()
                        );
                    }
                }
                else if(_e->type() == QEvent::MouseMove)
                {
                    m_dragInProgress = true;
                }
                else if(_e->type() == QEvent::MouseButtonPress)
                {
                    m_dragInProgress = false;
                }

                return nullptr;
            }

            if(auto* spinbox = qobject_cast<QAbstractSpinBox*>(_target))
            {
                if(_e->type() == QEvent::MouseButtonRelease)
                {
                    QRect decrement_zone = spinbox->rect();
                    QRect increment_zone = spinbox->rect();
                    decrement_zone.setWidth(static_cast<int>(decrement_zone.width() * 0.25));
                    increment_zone.setX(static_cast<int>(increment_zone.width() * 0.75));
                    if(decrement_zone.contains(me->pos()))
                    {
                        return std::make_unique<PreInteractionNumberInputModification>(
                            reinterpret_cast<intptr_t>(_target),
                            find(spinbox),
                            me->modifiers(),
                            NumberInputModification,
                            modification_t::DECREMENT,
                            1
                        );
                    }

                    if(increment_zone.contains(me->pos()))
                    {
                        return std::make_unique<PreInteractionNumberInputModification>(
                            reinterpret_cast<intptr_t>(_target),
                            find(spinbox),
                            me->modifiers(),
                            NumberInputModification,
                            modification_t::INCREMENT,
                            1
                        );
                    }
                }

                return nullptr;
            }

            if(auto* button = qobject_cast<QPushButton*>(_target);
               _e->type() == QEvent::MouseButtonRelease && button != nullptr && qobject_cast<QColorDialog*>(
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
                   _target
            ); checkbox != nullptr && checkbox->window()->objectName() == "preferences_configuration")
            {
                // Exception for preferences Configuration, in order to add it in helper::PreferencesConfiguration::fill
                // map
                if(_e->type() == QEvent::MouseButtonRelease)
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

            if(_e->type() == QEvent::MouseButtonDblClick
               && _target->objectName() == m_mainWindow->windowTitle() + "Window")
            {
                return nullptr;
            }

            return std::make_unique<PreInteractionMouse>(
                reinterpret_cast<intptr_t>(_target),
                find(_target),
                me->modifiers(),
                _e->type(),
                me->pos(),
                me->pos(),
                me->button()
            );
        }

        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            auto* ke          = static_cast<QKeyEvent*>(_e);
            QObject* receiver = nullptr;
            if(qApp->focusWidget() == nullptr)
            {
                receiver = _target;
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

            if(_e->type() == QEvent::KeyPress && !m_interactions.empty()
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
                _e->type(),
                static_cast<Qt::Key>(ke->key()),
                ke->text()
            );
        }

        case QEvent::Wheel:
        {
            auto* we = static_cast<QWheelEvent*>(_e);
            return std::make_unique<PreInteractionMouseWheel>(
                reinterpret_cast<intptr_t>(_target),
                find(_target),
                we->modifiers(),
                _e->type(),
                we->angleDelta(),
                we->position().toPoint()
            );
        }

        default:
            return nullptr;
    }
}
