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
// cspell:ignore ilwc strat htfr modif imvs

#include "macro_saver.hpp"

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

//------------------------------------------------------------------------------

const QEvent::Type MODEL_VIEW_SELECT_TYPE         = static_cast<QEvent::Type>(QEvent::registerEventType());
const QEvent::Type NUMBER_INPUT_MODIFICATION_TYPE = static_cast<QEvent::Type>(QEvent::registerEventType());

//------------------------------------------------------------------------------

QString modifiers_to_string(Qt::KeyboardModifiers _modifiers)
{
    QString res;
    static const std::map<QString, Qt::KeyboardModifier> s_MODIFIERS_LIST {
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
        for(const auto& [name, value] : s_MODIFIERS_LIST)
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

macro_interaction::macro_interaction(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers
) :
    receiver_id(_receiver_id),
    how_to_find_receiver(_how_to_find_receiver),
    modifiers(_modifiers)
{
}

//------------------------------------------------------------------------------

pre_interaction::pre_interaction(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type
) :
    macro_interaction(_receiver_id, _how_to_find_receiver, _modifiers),
    type(_type)
{
}

//------------------------------------------------------------------------------

post_interaction::post_interaction(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type
) :
    macro_interaction(_receiver_id, _how_to_find_receiver, _modifiers),
    type(_type)
{
}

//------------------------------------------------------------------------------

interaction_mouse::interaction_mouse(QPoint _from, QPoint _to, Qt::MouseButton _button) :
    from(_from),
    to(_to),
    button(_button)
{
}

interaction_mouse_wheel::interaction_mouse_wheel(QPoint _angle_delta, QPoint _pos) :
    angle_delta(_angle_delta),
    pos(_pos)
{
}

//------------------------------------------------------------------------------

interaction_keyboard::interaction_keyboard(Qt::Key _key, QString _sequence) :
    key(_key),
    sequence(std::move(_sequence))
{
}

//------------------------------------------------------------------------------

bool interaction_keyboard::is_printable() const
{
    return !sequence.isEmpty() && sequence[0].isPrint();
}

//------------------------------------------------------------------------------

interaction_model_view_select::interaction_model_view_select(QString _name) :
    name(std::move(_name))
{
}

number_input_modification::number_input_modification(modification_t _type, double _number) :
    modif_type(_type),
    modif_number(_number)
{
}

//------------------------------------------------------------------------------

pre_interaction_mouse::pre_interaction_mouse(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    QPoint _from,
    QPoint _to,
    Qt::MouseButton _button
) :
    pre_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_mouse(_from, _to, _button)
{
}

pre_interaction_mouse_wheel::pre_interaction_mouse_wheel(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    QPoint _angle_delta,
    QPoint _pos
) :
    pre_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_mouse_wheel(_angle_delta, _pos)
{
}

//------------------------------------------------------------------------------

pre_interaction_keyboard::pre_interaction_keyboard(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    Qt::Key _key,
    const QString& _sequence
) :
    pre_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_keyboard(_key, _sequence)
{
}

//------------------------------------------------------------------------------

pre_interaction_model_view_select::pre_interaction_model_view_select(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    const QString& _name
) :
    pre_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_model_view_select(_name)
{
}

pre_interaction_number_input_modification::pre_interaction_number_input_modification(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    QEvent::Type _type,
    modification_t _modif_type,
    double _modif_number
) :
    pre_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    number_input_modification(_modif_type, _modif_number)
{
}

//------------------------------------------------------------------------------

post_interaction_mouse::post_interaction_mouse(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    QPoint _from,
    QPoint _to,
    Qt::MouseButton _button
) :
    post_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_mouse(_from, _to, _button)
{
}

post_interaction_mouse_wheel::post_interaction_mouse_wheel(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    QPoint _angle_delta,
    QPoint _pos
) :
    post_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_mouse_wheel(_angle_delta, _pos)
{
}

//------------------------------------------------------------------------------

post_interaction_keyboard::post_interaction_keyboard(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    Qt::Key _key,
    const QString& _sequence
) :
    post_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_keyboard(_key, _sequence)
{
}

//------------------------------------------------------------------------------

post_interaction_model_view_select::post_interaction_model_view_select(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    const QString& _name
) :
    post_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    interaction_model_view_select(_name)
{
}

post_interaction_number_input_modification::post_interaction_number_input_modification(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    Qt::KeyboardModifiers _modifiers,
    interaction_t _type,
    modification_t _modif_type,
    double _modif_number
) :
    post_interaction(_receiver_id, _how_to_find_receiver, _modifiers, _type),
    number_input_modification(_modif_type, _modif_number)
{
}

interaction_helper_api::interaction_helper_api(
    intptr_t _receiver_id,
    const QVector<find_strategy>& _how_to_find_receiver,
    QString _method_name,
    std::optional<sight::ui::test::helper::selector> _select,
    QStringList _args
) :
    post_interaction(_receiver_id, _how_to_find_receiver, {}, helper_api), method_name(std::move(_method_name)),
    select(std::move(_select)), args(std::move(_args))
{
}

//------------------------------------------------------------------------------

bool macro_saver::eventFilter(QObject* _target, QEvent* _e)
{
    if(_e->type() == QEvent::ChildAdded)
    {
        auto* ce = dynamic_cast<QChildEvent*>(_e);
        infect(ce->child());
        if(m_main_window == nullptr)
        {
            m_main_window = qApp->activeWindow();
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
           && !(w != nullptr && w->isWindow()) && _target != m_main_window)
        {
            std::unique_ptr<pre_interaction> interaction = create_interaction(_target, _e);
            if(interaction != nullptr)
            {
                m_interactions.emplace_back(std::move(interaction));
            }
        }
    }

    return QObject::eventFilter(_target, _e);
}

//------------------------------------------------------------------------------

void macro_saver::infect(QObject* _o)
{
    _o->installEventFilter(this);
    for(QObject* child : _o->children())
    {
        infect(child);
    }
}

//------------------------------------------------------------------------------

QObject* macro_saver::findChild(
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

QObjectList macro_saver::findChildren(
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

QVector<find_strategy> macro_saver::find(QObject* _o)
{
    assert(_o != nullptr);
    QWidget* w            = qobject_cast<QWidget*>(_o);
    QByteArray class_name = _o->metaObject()->className();
    if(class_name == "QWidgetWindow" || class_name == "QColorPicker") // Those are private classes of Qt, we can't use
                                                                      // them as is.
    {
        class_name = "QWidget";
    }

    if(_o == m_main_window)
    {
        return {{find_strategy_t::root, class_name, "", 0}};
    }

    if(_o == qApp->activeModalWidget())
    {
        return {{find_strategy_t::active_modal_widget, class_name,
            w->objectName().isEmpty() ? w->windowTitle() : w->objectName(), 0
        }
        };
    }

    if(_o->objectName() != "")
    {
        QVector<find_strategy> res {{find_strategy_t::object_name, class_name, _o->objectName().toLatin1(), 0}};
        if(QObjectList children;
           w != nullptr
           && (children = findChildren(w->window(), class_name, _o->objectName())).size() == 1 && children[0] == _o)
        {
            res.append(find(w->window()));
        }
        else if(QObjectList children2 = findChildren(m_main_window, class_name, _o->objectName());
                children2.size() == 1 && children2[0] == _o)
        {
            res.append(find(m_main_window));
        }
        else
        {
            QVector<find_strategy> best_ancestor_strat;
            for(QObject* current_ancestor = parent_of(_o) ;
                current_ancestor != nullptr ;
                current_ancestor = parent_of(current_ancestor))
            {
                if(QObjectList children3 =
                       findChildren(current_ancestor, class_name, _o->objectName());
                   children3.size() == 1 && children3[0] == _o)
                {
                    QVector<find_strategy> current_ancestor_strat = find(current_ancestor);
                    if(current_ancestor_strat.back().type != find_strategy_t::cant_be_found
                       && (best_ancestor_strat.empty() || current_ancestor_strat.size() < best_ancestor_strat.size()))
                    {
                        best_ancestor_strat = current_ancestor_strat;
                    }
                }
            }

            if(best_ancestor_strat.empty())
            {
                res.append({find_strategy_t::cant_be_found, class_name, "", 0});
            }
            else
            {
                res.append(best_ancestor_strat);
            }
        }

        return res;
    }

    if(findChildren(m_main_window, class_name).size() == 1)
    {
        return {{find_strategy_t::global_type, class_name, "", 0}};
    }

    if(w != nullptr && w->actions().size() == 1)
    {
        QVector<find_strategy> res {{find_strategy_t::action, class_name, "", 0}};
        res.append(find(w->actions()[0]));
        return res;
    }

    if(parent_of(_o) == nullptr)
    {
        return {{find_strategy_t::cant_be_found, class_name, "", 0}};
    }

    if(findChildren(parent_of(_o), class_name, "", Qt::FindDirectChildrenOnly).size() == 1)
    {
        QVector<find_strategy> res {{find_strategy_t::local_type, class_name, "", 0}};
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

    QVector<find_strategy> res {{find_strategy_t::nth_child, class_name, "", index}};
    res.append(find(parent_of(_o)));
    return res;
}

//------------------------------------------------------------------------------

static QString select_to_code(const sight::ui::test::helper::selector& _select)
{
    switch(_select.type())
    {
        case sight::ui::test::helper::selector::type::from_main:
            return QString("\"%1\"").arg(std::get<std::string>(_select.data()).c_str());

        case sight::ui::test::helper::selector::type::from_dialog:
            return QString("selector::fromDialog(\"%1\")").arg(std::get<std::string>(_select.data()).c_str());

        case sight::ui::test::helper::selector::type::from_parent:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(_select.data());
            return QString(R"(selector::fromParent("%1", "%2"))").arg(parentName.c_str()).arg(childName.c_str());
        }

        case sight::ui::test::helper::selector::type::from_current:
            return QString("selector::fromCurrent(\"%1\")").arg(std::get<std::string>(_select.data()).c_str());

        case sight::ui::test::helper::selector::type::current:
            return "Select::current()";

        case sight::ui::test::helper::selector::type::dialog:
            return "Select::dialog()";
    }

    SIGHT_ASSERT(
        "Invalid Select type",
        0
    );
    return "";
}

//------------------------------------------------------------------------------

[[nodiscard]] static std::pair<QVector<find_strategy>, sight::ui::test::helper::selector> compute_select(
    QVector<find_strategy> _how_to_find_receiver
)
{
    sight::ui::test::helper::selector select = sight::ui::test::helper::selector::current();
    if(_how_to_find_receiver.size() >= 2 && _how_to_find_receiver[0].type == find_strategy_t::object_name
       && _how_to_find_receiver[1].type == find_strategy_t::root)
    {
        select = sight::ui::test::helper::selector(_how_to_find_receiver[0].string.toStdString());
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
    }
    else if(_how_to_find_receiver.size() >= 2 && _how_to_find_receiver[0].type == find_strategy_t::object_name
            && _how_to_find_receiver[1].type == find_strategy_t::active_modal_widget)
    {
        select = sight::ui::test::helper::selector::from_dialog(_how_to_find_receiver[0].string.toStdString());
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
    }
    else if(_how_to_find_receiver.size() >= 2 && _how_to_find_receiver[0].type == find_strategy_t::object_name
            && _how_to_find_receiver[1].type == find_strategy_t::object_name
            && _how_to_find_receiver[2].type == find_strategy_t::root)
    {
        select = sight::ui::test::helper::selector::from_parent(
            _how_to_find_receiver[1].string.toStdString(),
            _how_to_find_receiver[0].string.toStdString()
        );
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
        _how_to_find_receiver.pop_front();
    }
    else if(!_how_to_find_receiver.empty() && _how_to_find_receiver[0].type == find_strategy_t::active_modal_widget)
    {
        select = sight::ui::test::helper::selector::dialog();
        _how_to_find_receiver.pop_front();
    }

    return {_how_to_find_receiver, select};
}

//------------------------------------------------------------------------------

void macro_saver::save()
{
    // Interactions preprocessing
    std::vector<std::unique_ptr<post_interaction> > pre_post_interactions;
    for(std::size_t i = 0 ; i < m_interactions.size() ; i++)
    {
        if(m_interactions[i]->type == QEvent::Type::MouseButtonPress)
        {
            const auto& im = static_cast<pre_interaction_mouse&>(*m_interactions[i]);
            if(i + 1 == m_interactions.size())
            {
                // This mouse button press event is the last event, it won't be ended with a release. Ignore it.
                continue;
            }

            if(m_interactions[i + 1]->type == QEvent::Type::MouseButtonRelease)
            {
                // A mouse button press immediately followed with a mouse button release is a simple mouse click.
                pre_post_interactions.emplace_back(
                    std::make_unique<post_interaction_mouse>(
                        im.receiver_id,
                        im.how_to_find_receiver,
                        im.modifiers,
                        mouse_click,
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
                    const auto& release = static_cast<pre_interaction_mouse&>(*m_interactions[i]);
                    pre_post_interactions.emplace_back(
                        std::make_unique<post_interaction_mouse>(
                            im.receiver_id,
                            im.how_to_find_receiver,
                            im.modifiers,
                            mouse_drag,
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
            if(pre_post_interactions.back()->type == mouse_double_click)
            {
                // This is a duplicated event, ignore it
                continue;
            }

            if(pre_post_interactions.back()->type == mouse_click)
            {
                // Ignore the click that generated this double click
                pre_post_interactions.pop_back();
            }

            const auto& im = static_cast<pre_interaction_mouse&>(*m_interactions[i]);
            pre_post_interactions.emplace_back(
                std::make_unique<post_interaction_mouse>(
                    im.receiver_id,
                    im.how_to_find_receiver,
                    im.modifiers,
                    mouse_double_click,
                    im.from,
                    im.to,
                    im.button
                )
            );
        }
        else if(m_interactions[i]->type == QEvent::Type::Wheel)
        {
            const auto& imw    = static_cast<pre_interaction_mouse_wheel&>(*m_interactions[i]);
            QPoint angle_delta = imw.angle_delta / 2; // Divide by two because events are duplicated
            // Compress all mouse wheel events into one
            while(i + 1 < m_interactions.size() && m_interactions[i + 1]->type == QEvent::Type::Wheel)
            {
                i++;
                angle_delta += static_cast<pre_interaction_mouse_wheel&>(*m_interactions[i]).angle_delta / 2;
            }

            pre_post_interactions.emplace_back(
                std::make_unique<post_interaction_mouse_wheel>(
                    imw.receiver_id,
                    imw.how_to_find_receiver,
                    imw.modifiers,
                    mouse_wheel,
                    angle_delta,
                    imw.pos
                )
            );
        }
        else if(m_interactions[i]->type == QEvent::Type::KeyPress)
        {
            const auto& ik = static_cast<pre_interaction_keyboard&>(*m_interactions[i].get());
            if(ik.is_printable())
            {
                // If the key is printable, we will attempt to "compress" it with the following key events if they are
                // printable as well, to make the resulting code more readable.
                QString sequence;
                const pre_interaction_keyboard* press = nullptr;
                while(i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::KeyPress
                      && (press = static_cast<pre_interaction_keyboard*>(m_interactions[i].get())) != nullptr
                      && press->is_printable())
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
                        std::make_unique<post_interaction_keyboard>(
                            ik.receiver_id,
                            ik.how_to_find_receiver,
                            ik.modifiers,
                            (sequence.size() > 1 ? keyboard_sequence : keyboard_click),
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
                        std::make_unique<post_interaction_keyboard>(
                            ik.receiver_id,
                            ik.how_to_find_receiver,
                            ik.modifiers,
                            keyboard_click,
                            ik.key,
                            ik.sequence
                        )
                    );
                }
            }
        }
        else if(m_interactions[i]->type == MODEL_VIEW_SELECT_TYPE)
        {
            const auto& ilwc = static_cast<pre_interaction_model_view_select&>(*m_interactions[i]);
            pre_post_interactions.emplace_back(
                std::make_unique<post_interaction_model_view_select>(
                    ilwc.receiver_id,
                    ilwc.how_to_find_receiver,
                    ilwc.modifiers,
                    model_view_select,
                    ilwc.name
                )
            );
        }
        else if(m_interactions[i]->type == NUMBER_INPUT_MODIFICATION_TYPE)
        {
            const auto& nim = static_cast<pre_interaction_number_input_modification&>(*m_interactions[i]);
            pre_post_interactions.emplace_back(
                std::make_unique<post_interaction_number_input_modification>(
                    nim.receiver_id,
                    nim.how_to_find_receiver,
                    nim.modifiers,
                    number_input_modification,
                    nim.modif_type,
                    nim.modif_number
                )
            );
        }
    }

    std::vector<std::unique_ptr<post_interaction> > post_interactions;
    for(std::size_t i = 0 ; i < pre_post_interactions.size() ; i++)
    {
        if(pre_post_interactions[i]->type == mouse_click
           && (pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QPushButton"
               || pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QToolButton"))
        {
            if(pre_post_interactions[i]->how_to_find_receiver.back().string == "preferences_configuration")
            {
                // Special case: preferences Configuration pop up close button
                if(interaction_helper_api* iha =
                       nullptr;
                   post_interactions.back()->type == helper_api
                   && (iha = static_cast<interaction_helper_api*>(post_interactions.back().get()))->method_name
                   == "PreferencesConfiguration::fill")
                {
                    iha->args[0] += "}";
                }
                else
                {
                    post_interactions.emplace_back(
                        std::make_unique<interaction_helper_api>(
                            pre_post_interactions[i]->
                            receiver_id,
                            QVector<find_strategy> {},
                            "PreferencesConfiguration",
                            std::nullopt,
                            QStringList {"{}"
                            })
                    );
                }
            }
            else
            {
                QVector<find_strategy> how_to_find_receiver = pre_post_interactions[i]->how_to_find_receiver;
                sight::ui::test::helper::selector select    = sight::ui::test::helper::selector::current();
                if(how_to_find_receiver.front().type == find_strategy_t::action)
                {
                    // helper::Button::push already handles the "action" case.
                    how_to_find_receiver.pop_front();
                }

                std::tie(how_to_find_receiver, select) = compute_select(how_to_find_receiver);
                post_interactions.emplace_back(
                    std::make_unique<interaction_helper_api>(
                        pre_post_interactions[i]->receiver_id,
                        how_to_find_receiver,
                        "Button::push",
                        select
                    )
                );
            }
        }
        else if(pre_post_interactions[i]->type == mouse_click
                && pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QCheckBox")
        {
            auto [howToFindReceiver, select] = compute_select(pre_post_interactions[i]->how_to_find_receiver);
            post_interactions.emplace_back(
                std::make_unique<interaction_helper_api>(
                    pre_post_interactions[i]->receiver_id,
                    howToFindReceiver,
                    "CheckBox::toggle",
                    select
                )
            );
        }
        else if(pre_post_interactions[i]->type == model_view_select
                && (pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QCheckBox"
                    || pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QComboBox")
                && pre_post_interactions[i]->how_to_find_receiver.back().string == "preferences_configuration")
        {
            const auto& imvs = static_cast<const post_interaction_model_view_select&>(*pre_post_interactions[i]);
            QString arg      = QString(R"({"%1", "%2"})").arg(imvs.how_to_find_receiver[0].string).arg(imvs.name);
            if(interaction_helper_api* iha =
                   nullptr;
               post_interactions.back()->type == helper_api
               && (iha = static_cast<interaction_helper_api*>(post_interactions.back().get()))->method_name
               == "PreferencesConfiguration::fill")
            {
                if(!iha->args[0].contains(
                       QString("\"%1\"").arg(
                           pre_post_interactions[i]->how_to_find_receiver[0].string
                       )
                ))
                {
                    iha->args[0] += QString(", " + arg);
                }
            }
            else
            {
                post_interactions.emplace_back(
                    std::make_unique<interaction_helper_api>(
                        pre_post_interactions[i]->receiver_id,
                        QVector<find_strategy> {},
                        "PreferencesConfiguration::fill",
                        std::nullopt,
                        QStringList {"{" + arg
                        })
                );
            }
        }
        else if(pre_post_interactions[i]->type == model_view_select
                && pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QComboBox")
        {
            if(!post_interactions.empty() && post_interactions.back()->type == mouse_drag)
            {
                // It seems mouse drags are always duplicated with comboboxes
                post_interactions.pop_back();
            }
            else if(post_interactions.size() >= 2 && post_interactions.back()->type == mouse_click
                    && post_interactions[post_interactions.size() - 2]->type == mouse_click)
            {
                // It seems mouse clicks are always duplicated with comboboxes
                post_interactions.pop_back();
                post_interactions.pop_back();
            }
            else if(!post_interactions.empty() && post_interactions.back()->type == helper_api)
            {
                // It seems mouse clicks are always duplicated with comboboxes
                continue;
            }

            const auto& imvs = static_cast<post_interaction_model_view_select&>(*pre_post_interactions[i]);
            auto [howToFindReceiver, select] = compute_select(imvs.how_to_find_receiver);
            post_interactions.emplace_back(
                std::make_unique<interaction_helper_api>(
                    imvs.receiver_id,
                    howToFindReceiver,
                    "ComboBox::select",
                    select,
                    QStringList {QString("\"%1\"").arg(imvs.name)
                    })
            );
        }
        else if((pre_post_interactions[i]->type == keyboard_click
                 || pre_post_interactions[i]->type == keyboard_sequence)
                && pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QLineEdit")
        {
            const auto& ik = static_cast<post_interaction_keyboard&>(*pre_post_interactions[i]);
            if(pre_post_interactions[i]->how_to_find_receiver.back().type == find_strategy_t::active_modal_widget)
            {
                // Compress all keyboard sequence/click in one
                QString sequence = (ik.is_printable() ? ik.sequence : "");
                for( ;
                     i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->receiver_id == ik.receiver_id
                     && (pre_post_interactions[i + 1]->type == keyboard_click
                         || pre_post_interactions[i + 1]->type == keyboard_sequence) ;
                     i++)
                {
                    const auto& ik2 = static_cast<post_interaction_keyboard&>(*pre_post_interactions[i + 1]);
                    if(ik2.key == Qt::Key::Key_Backspace)
                    {
                        sequence.resize(sequence.size() - 1);
                    }
                    else if(ik2.key == Qt::Key::Key_Return)
                    {
                        break;
                    }
                    else if(ik2.is_printable())
                    {
                        sequence += ik2.sequence;
                    }

                    // Ignore other keys
                }

                if(pre_post_interactions[i]->how_to_find_receiver.back().class_name == "QFileDialog")
                {
                    post_interactions.emplace_back(
                        std::make_unique<interaction_helper_api>(
                            ik.receiver_id,
                            QVector<find_strategy> {},
                            "FileDialog::fill",
                            std::nullopt,
                            QStringList {QString("\"%1\"").arg(sequence)
                            })
                    );
                    if(i + 1 < pre_post_interactions.size()
                       && pre_post_interactions[i + 1]->receiver_id == ik.receiver_id
                       && pre_post_interactions[i + 1]->type == keyboard_click
                       && static_cast<post_interaction_keyboard&>(*pre_post_interactions[i + 1]).key
                       == Qt::Key::Key_Return)
                    {
                        // Ignore the Enter key press to confirm the file dialog, as it is already handled by
                        // helper::FieldDialog::fill
                        i++;
                    }
                }
                else if(pre_post_interactions[i]->how_to_find_receiver.back().string == "preferences_configuration")
                {
                    QString arg =
                        QString(R"({"%1", "%2"})").arg(pre_post_interactions[i]->how_to_find_receiver[0].string).arg(
                            sequence
                        );
                    if(interaction_helper_api* iha =
                           nullptr;
                       post_interactions.back()->type == helper_api
                       && (iha = static_cast<interaction_helper_api*>(post_interactions.back().get()))->method_name
                       == "PreferencesConfiguration::fill")
                    {
                        if(!iha->args[0].contains(
                               QString("\"%1\"").arg(
                                   pre_post_interactions[i]->how_to_find_receiver[0].
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
                            std::make_unique<interaction_helper_api>(
                                pre_post_interactions[i]->
                                receiver_id,
                                QVector<find_strategy> {},
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
                auto [howToFindReceiver, select] = compute_select(ik.how_to_find_receiver);
                post_interactions.emplace_back(
                    std::make_unique<interaction_helper_api>(
                        ik.receiver_id,
                        howToFindReceiver,
                        "field::fill",
                        select,
                        QStringList {QString("\"%1\"").arg(ik.sequence)
                        })
                );
            }
        }
        else if(pre_post_interactions[i]->type == model_view_select
                && pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QListWidget")
        {
            const auto& ilwc = static_cast<post_interaction_model_view_select&>(*pre_post_interactions[i]);
            if(pre_post_interactions[i]->how_to_find_receiver.back().type == find_strategy_t::active_modal_widget
               && pre_post_interactions[i]->how_to_find_receiver.back().string == "selector")
            {
                if(post_interactions.back()->type == helper_api
                   && static_cast<interaction_helper_api&>(*post_interactions.back()).method_name
                   == "selector::select"
                   && post_interactions.back()->receiver_id == pre_post_interactions[i]->receiver_id)
                {
                    // The user selected an item then selected another one; take only the last choice
                    post_interactions.pop_back();
                }

                post_interactions.emplace_back(
                    std::make_unique<interaction_helper_api>(
                        ilwc.receiver_id,
                        QVector<find_strategy> {},
                        "selector::select",
                        std::nullopt,
                        QStringList {QString("\"%1\"").arg(ilwc.name)
                        })
                );
                if(i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->type == mouse_click
                   && pre_post_interactions[i + 1]->how_to_find_receiver[0].class_name == "QPushButton"
                   && pre_post_interactions[i + 1]->how_to_find_receiver[0].string == "Ok"
                   && pre_post_interactions[i + 1]->how_to_find_receiver[1].type
                   == find_strategy_t::active_modal_widget)
                {
                    // Ignore the eventual "Ok" button click as it is handled by helper::selector::select
                    i++;
                }

                if(i + 1 < pre_post_interactions.size() && pre_post_interactions[i + 1]->type == mouse_double_click
                   && pre_post_interactions[i + 1]->how_to_find_receiver[0].string == "SelectorDialogWindow")
                {
                    // It seems there might be a duplicated double click event
                    i++;
                }
            }
            else
            {
                auto [howToFindReceiver, select] = compute_select(ilwc.how_to_find_receiver);
                post_interactions.emplace_back(
                    std::make_unique<interaction_helper_api>(
                        ilwc.receiver_id,
                        howToFindReceiver,
                        "ListWidget::setCurrentText",
                        select,
                        QStringList {QString("\"%1\"").arg(ilwc.name)
                        })
                );
            }
        }
        else if(pre_post_interactions[i]->type == number_input_modification
                && pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QSlider")
        {
            if(!post_interactions.empty()
               && (post_interactions.back()->type == mouse_drag || post_interactions.back()->type == mouse_click
                   || post_interactions.back()->type == mouse_double_click))
            {
                // It seems mouse drags are always duplicated with sliders
                post_interactions.pop_back();
            }
            else if(!post_interactions.empty() && post_interactions[post_interactions.size() - 1]->type == helper_api)
            {
                // It seems mouse drags are always duplicated with sliders
                continue;
            }

            const auto& nim =
                static_cast<post_interaction_number_input_modification&>(*pre_post_interactions[i]);
            QVector<find_strategy> how_to_find_receiver = nim.how_to_find_receiver;
            sight::ui::test::helper::selector select    = sight::ui::test::helper::selector::current();
            if(how_to_find_receiver.front().type == find_strategy_t::local_type
               && how_to_find_receiver.front().class_name == "QSlider")
            {
                // Finding the actual QSlider is already handled in helper::Slider::set
                how_to_find_receiver.pop_front();
            }

            std::tie(how_to_find_receiver, select) = compute_select(how_to_find_receiver);
            if(nim.modif_type == modification_t::set)
            {
                post_interactions.emplace_back(
                    std::make_unique<interaction_helper_api>(
                        nim.receiver_id,
                        how_to_find_receiver,
                        "Slider::set",
                        select,
                        QStringList {QString::number(nim.modif_number)
                        })
                );
            }
            else
            {
                QString position;
                if(nim.modif_type == modification_t::decrement)
                {
                    position = "helper::Slider::Position::LEFT";
                }
                else if(nim.modif_type == modification_t::increment)
                {
                    position = "helper::Slider::Position::RIGHT";
                }

                if(interaction_helper_api* previous_interaction =
                       nullptr;
                   !post_interactions.empty() && post_interactions.back()->type == helper_api
                   && post_interactions.back()->receiver_id == nim.receiver_id
                   && (previous_interaction =
                           static_cast<interaction_helper_api*>(post_interactions.back().get()))->method_name
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
                        std::make_unique<interaction_helper_api>(
                            nim.receiver_id,
                            how_to_find_receiver,
                            "Slider::move",
                            select,
                            QStringList {position
                            })
                    );
                }
            }
        }
        else if(pre_post_interactions[i]->type == number_input_modification
                && (pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QSpinBox"
                    || pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QDoubleSpinBox"))
        {
            if(!post_interactions.empty() && post_interactions.back()->type == mouse_click)
            {
                // It seems mouse clicks are always duplicated with spinboxes
                post_interactions.pop_back();
            }
            else if(!post_interactions.empty() && post_interactions.back()->type == helper_api)
            {
                // It seems mouse clicks are always duplicated with spinboxes
                continue;
            }

            const auto& nim = static_cast<post_interaction_number_input_modification&>(*pre_post_interactions[i]);
            auto [howToFindReceiver, select] = compute_select(nim.how_to_find_receiver);
            if(nim.modif_type == modification_t::set)
            {
                assert(0);
            }
            else
            {
                QString method_name;
                if(nim.modif_type == modification_t::decrement)
                {
                    method_name = "SpinBox::decrement";
                }
                else if(nim.modif_type == modification_t::increment)
                {
                    method_name = "SpinBox::increment";
                }

                if(interaction_helper_api* previous_interaction =
                       nullptr;
                   !post_interactions.empty() && post_interactions.back()->type == helper_api
                   && post_interactions.back()->receiver_id == nim.receiver_id
                   && (previous_interaction =
                           static_cast<interaction_helper_api*>(post_interactions.back().get()))->method_name
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
                        std::make_unique<interaction_helper_api>(
                            nim.receiver_id,
                            howToFindReceiver,
                            method_name,
                            select
                        )
                    );
                }
            }
        }
        else if(pre_post_interactions[i]->type == model_view_select
                && pre_post_interactions[i]->how_to_find_receiver[0].class_name == "QColorDialog")
        {
            // Ignore all interactions inside the color dialog
            while(std::ranges::any_of(
                      post_interactions.back()->how_to_find_receiver,
                      [](const find_strategy& _s)
            {
                return _s.class_name == "QColorDialog";
            }))
            {
                post_interactions.pop_back();
            }

            if((post_interactions.back()->type == mouse_click || post_interactions.back()->type == helper_api)
               && !post_interactions.back()->how_to_find_receiver.empty()
               && post_interactions.back()->how_to_find_receiver[0].class_name == "QPushButton")
            {
                // Get rid of duplicate button click
                post_interactions.pop_back();
            }

            QVector<find_strategy> how_to_find_receiver;
            std::optional<sight::ui::test::helper::selector> select;
            if(interaction_helper_api* iha =
                   nullptr;
               post_interactions.back()->type == helper_api
               && (iha = static_cast<interaction_helper_api*>(post_interactions.back().get()))->method_name
               == "Button::push")
            {
                // helper::ColorParameter::select already clicks on the Color button
                how_to_find_receiver = iha->how_to_find_receiver;
                select               = iha->select;
                post_interactions.pop_back();
            }
            else
            {
                // It's a duplicate, ignore it
                continue;
            }

            post_interactions.emplace_back(
                std::make_unique<interaction_helper_api>(
                    pre_post_interactions[i]->receiver_id,
                    how_to_find_receiver,
                    "ColorParameter::select",
                    select,
                    QStringList {QString("\"%1\"").arg(
                                     static_cast<post_interaction_model_view_select&>(*pre_post_interactions
                                                                                      [i]).name
                    )
                    })
            );
        }
        else if(pre_post_interactions[i]->how_to_find_receiver.back().string == "preferences_configuration"
                || std::ranges::any_of(
                    pre_post_interactions[i]->how_to_find_receiver,
                    [](const find_strategy& _f)
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
    for(const std::unique_ptr<post_interaction>& interaction : post_interactions)
    {
        for(const find_strategy& strategy : interaction->how_to_find_receiver)
        {
            if(strategy.type == find_strategy_t::global_type || strategy.type == find_strategy_t::local_type)
            {
                dependencies.append(strategy.class_name);
            }
            else if(strategy.type == find_strategy_t::action)
            {
                dependencies.append("QAction");
            }
            else if(strategy.type == find_strategy_t::active_modal_widget)
            {
                dependencies.append("ui/test/helper/dialog.hpp");
                dependencies.append(strategy.class_name);
            }
        }

        if(interaction->type == mouse_click || interaction->type == mouse_drag)
        {
            dependencies.append("QPoint");
        }
        else if(interaction->type == model_view_select)
        {
            dependencies.append("QListWidget");
        }

        if(interaction->type == helper_api)
        {
            use_helpers = true;
            const auto& iha = static_cast<interaction_helper_api&>(*interaction);
            if(iha.select && iha.select.value().type() != sight::ui::test::helper::selector::type::from_main)
            {
                use_select_constructor = true;
            }

            dependencies.append(QString("ui/test/helper/%1.hpp").arg(iha.method_name.split("::")[0]));
        }
    }

    dependencies.sort();
    dependencies.removeDuplicates();

    static constexpr auto s_WRITE = [](QFile& _f, int _indentation, const QString& _line)
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

    QFile cpp("gui_test.cpp");
    cpp.open(QIODevice::WriteOnly);

    s_WRITE(cpp, 0, "#include \"gui_test.hpp\"");
    s_WRITE(cpp, 0, "");
    s_WRITE(cpp, 0, "#include <core/runtime/path.hpp>");
    s_WRITE(cpp, 0, "#include <ui/test/tester.hpp>");
    s_WRITE(cpp, 0, "");
    s_WRITE(cpp, 0, "#include <boost/dll.hpp>");
    s_WRITE(cpp, 0, "");
    for(const QString& dependency : dependencies)
    {
        s_WRITE(cpp, 0, QString("#include <%1>").arg(dependency));
    }

    s_WRITE(cpp, 0, "");
    s_WRITE(cpp, 0, "CPPUNIT_TEST_SUITE_REGISTRATION(gui_test);");
    s_WRITE(cpp, 0, "");
    s_WRITE(cpp, 0, "//------------------------------------------------------------------------------");
    s_WRITE(cpp, 0, "");
    s_WRITE(cpp, 0, "std::filesystem::path gui_test::getProfilePath()");
    s_WRITE(cpp, 0, "{");
    s_WRITE(cpp, 4, "const std::filesystem::path cwd = std::filesystem::path(");
    s_WRITE(cpp, 8, "boost::dll::this_line_location().parent_path().parent_path().string()");
    s_WRITE(cpp, 4, ");");
    std::filesystem::path absolute_profile_path(sight::core::runtime::get_current_profile()->get_file_path());
    QString profile_path(QString::fromStdString(std::filesystem::relative(absolute_profile_path).string()));
    s_WRITE(cpp, 4, QString("return cwd / \"%1\";").arg(profile_path).toLatin1());
    s_WRITE(cpp, 0, "}");
    s_WRITE(cpp, 0, "");

    s_WRITE(cpp, 0, "void gui_test::test()");
    s_WRITE(cpp, 0, "{");
    s_WRITE(cpp, 4, "start(\"gui_test\",");
    s_WRITE(cpp, 8, "[](sight::ui::testCore::Tester& tester)");
    s_WRITE(cpp, 4, "{");

    if(use_helpers)
    {
        s_WRITE(cpp, 8, "namespace helper = sight::ui::testCore::helper;");
    }

    if(use_select_constructor)
    {
        s_WRITE(cpp, 8, "using Select     = helper::Select;");
    }

    s_WRITE(cpp, 0, "");

    intptr_t current_item_id = 0;
    for(const std::unique_ptr<post_interaction>& interaction : post_interactions)
    {
        if(interaction->receiver_id != current_item_id)
        {
            // The receiver of this interaction isn't the current item: We must take it.
            if(interaction->how_to_find_receiver.size() >= 2
               && interaction->how_to_find_receiver.back().type == find_strategy_t::root
               && interaction->how_to_find_receiver[interaction->how_to_find_receiver.size() - 2].type
               == find_strategy_t::object_name)
            {
                s_WRITE(
                    cpp,
                    8,
                    QString(R"(tester.take("%1", "%1");)").arg(
                        interaction->how_to_find_receiver[interaction->
                                                          how_to_find_receiver
                                                          .size() - 2].string
                    )
                );
                interaction->how_to_find_receiver.pop_back();
                interaction->how_to_find_receiver.pop_back();
            }

            for(int i = interaction->how_to_find_receiver.size() - 1 ; i >= 0 ; i--)
            {
                find_strategy strat = interaction->how_to_find_receiver[i];
                if(strat.type == find_strategy_t::root)
                {
                    s_WRITE(cpp, 8, "tester.take(\"main window\", tester.getMainWindow());");
                }
                else if(strat.type == find_strategy_t::active_modal_widget)
                {
                    s_WRITE(
                        cpp,
                        8,
                        QString("helper::Dialog::take<%1*>(tester, \"%2\");").arg(strat.class_name).arg(strat.string)
                    );
                }
                else if(strat.type == find_strategy_t::object_name)
                {
                    s_WRITE(cpp, 8, "tester.yields(");
                    s_WRITE(cpp, 12, QString("\"%1\",").arg(strat.string));
                    s_WRITE(
                        cpp,
                        12,
                        QString("[](QObject* old) -> QObject* {return old->findChild<QObject*>(\"%1\");}")
                        .arg(strat.string)
                    );
                    s_WRITE(cpp, 8, ");");
                }
                else if(strat.type == find_strategy_t::global_type)
                {
                    s_WRITE(cpp, 8, QString("tester.take%1(").arg(strat.string.isEmpty() ? "" : "<QWidget*>"));
                    s_WRITE(cpp, 12, QString("\"%1\",").arg(strat.class_name));
                    s_WRITE(cpp, 12, "[&tester]() -> QObject*");
                    s_WRITE(cpp, 8, "{");
                    s_WRITE(
                        cpp,
                        12,
                        QString("return tester.getMainWindow()->findChild<%1*>();")
                        .arg(strat.class_name)
                    );
                    s_WRITE(cpp, 8, "});");
                }
                else if(strat.type == find_strategy_t::action)
                {
                    s_WRITE(cpp, 8, "tester.yields(");
                    s_WRITE(cpp, 12, QString("\"%1\",").arg(strat.class_name));
                    s_WRITE(cpp, 12, "[](QObject* old) -> QObject*");
                    s_WRITE(cpp, 8, "{");
                    s_WRITE(
                        cpp,
                        12,
                        "return sight::ui::testCore::Tester::getWidgetFromAction(qobject_cast<QAction*>(old));"
                    );
                    s_WRITE(cpp, 8, "});");
                }
                else if(strat.type == find_strategy_t::local_type)
                {
                    s_WRITE(cpp, 8, "tester.yields(");
                    s_WRITE(cpp, 12, QString("\"%1\",").arg(strat.class_name));
                    s_WRITE(cpp, 12, "[](QObject* old) -> QObject*");
                    s_WRITE(cpp, 8, "{");
                    s_WRITE(cpp, 12, QString("return old->findChild<%1*>();").arg(strat.class_name));
                    s_WRITE(cpp, 8, "});");
                }
                else if(strat.type == find_strategy_t::nth_child)
                {
                    s_WRITE(cpp, 8, "tester.yields(");
                    s_WRITE(cpp, 12, QString("\"%1\",").arg(strat.class_name));
                    s_WRITE(cpp, 12, "[](QObject* old) -> QObject*");
                    s_WRITE(cpp, 8, "{");
                    s_WRITE(cpp, 12, QString("return old->children()[%1];").arg(strat.integer));
                    s_WRITE(cpp, 8, "});");
                }
                else if(strat.type == find_strategy_t::cant_be_found)
                {
                    s_WRITE(
                        cpp,
                        8,
                        QString("// TODO: Find the %1. MacroSaver wasn't able to find it automatically.")
                        .arg(strat.class_name)
                    );
                }
            }

            current_item_id = interaction->receiver_id;
        }

        const static QMetaEnum s_MOUSE_BUTTON = QMetaEnum::fromType<Qt::MouseButtons>();
        const static QMetaEnum s_KEY          = QMetaEnum::fromType<Qt::Key>();
        if(interaction->type == mouse_click)
        {
            const auto& im = static_cast<post_interaction_mouse&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseClick>(Qt::MouseButton::%1, %2, %3));"
                )
                .arg(s_MOUSE_BUTTON.valueToKey(static_cast<int>(im.button))).arg(modifiers_to_string(im.modifiers))
                .arg(QTest::toString(im.from))
            );
        }
        else if(interaction->type == mouse_double_click)
        {
            const auto& im = static_cast<post_interaction_mouse&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseDoubleClick>(Qt::MouseButton::%1, %2, %3));"
                )
                .arg(s_MOUSE_BUTTON.valueToKey(static_cast<int>(im.button))).arg(modifiers_to_string(im.modifiers))
                .arg(QTest::toString(im.from))
            );
        }
        else if(interaction->type == mouse_drag)
        {
            const auto& im = static_cast<post_interaction_mouse&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseDrag>(%1, %2, Qt::MouseButton::%3, %4));"
                )
                .arg(QTest::toString(im.from)).arg(QTest::toString(im.to))
                .arg(s_MOUSE_BUTTON.valueToKey(static_cast<int>(im.button))).arg(modifiers_to_string(im.modifiers))
            );
        }
        else if(interaction->type == mouse_wheel)
        {
            const auto& im = static_cast<post_interaction_mouse_wheel&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::testCore::MouseWheel>(%1, %2, %3));"
                )
                .arg(QTest::toString(im.angle_delta)).arg(modifiers_to_string(im.modifiers)).arg(
                    QTest::toString(
                        im.pos
                    )
                )
            );
        }
        else if(interaction->type == keyboard_click)
        {
            const auto& ik = static_cast<post_interaction_keyboard&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::testCore::KeyboardClick>(Qt::Key::%1, %2));")
                .arg(s_KEY.valueToKey(static_cast<int>(ik.key))).arg(modifiers_to_string(ik.modifiers))
            );
        }
        else if(interaction->type == keyboard_sequence)
        {
            const auto& ik = static_cast<post_interaction_keyboard&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::testCore::KeyboardSequence>(\"%1\", %2));")
                .arg(ik.sequence).arg(modifiers_to_string(ik.modifiers))
            );
        }
        else if(interaction->type == model_view_select)
        {
            const auto& ilwc = static_cast<post_interaction_model_view_select&>(*interaction);
            s_WRITE(cpp, 8, "tester.do_something_asynchronously<QListWidget*>(");
            s_WRITE(cpp, 12, "[](QListWidget* obj)");
            s_WRITE(cpp, 8, "{");
            s_WRITE(
                cpp,
                12,
                QString("obj->setCurrentItem(obj->findItems(\"%1\", Qt::MatchFlag::MatchExactly)[0]);")
                .arg(ilwc.name)
            );
            s_WRITE(cpp, 8, "});");
        }
        else if(interaction->type == number_input_modification)
        {
            assert(0);
        }
        else if(interaction->type == helper_api)
        {
            const auto& ha = static_cast<interaction_helper_api&>(*interaction);
            s_WRITE(
                cpp,
                8,
                QString("helper::%1(tester%2%3);").arg(ha.method_name).arg(
                    ha.select ? ", "
                    + select_to_code(*ha.select) : ""
                ).arg(
                    ha.args.empty()
                    ? "" : ", " + ha.args.join(", ")
                )
            );
        }
    }

    s_WRITE(cpp, 4, "});");
    s_WRITE(cpp, 0, "}");

    QFile hpp("gui_test.hpp");
    hpp.open(QIODevice::WriteOnly);
    s_WRITE(hpp, 0, "#pragma once");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 0, "#include <ui/test/test.hpp>");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 0, "#include <core/runtime/profile/profile.hpp>");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 0, "#include <cppunit/extensions/HelperMacros.h>");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 0, "class gui_test : public sight::ui::testCore::test");
    s_WRITE(hpp, 0, "{");
    s_WRITE(hpp, 0, "CPPUNIT_TEST_SUITE(gui_test);");
    s_WRITE(hpp, 0, "CPPUNIT_TEST(test);");
    s_WRITE(hpp, 0, "CPPUNIT_TEST_SUITE_END();");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 0, "public:");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 4, "std::filesystem::path getProfilePath() override;");
    s_WRITE(hpp, 0, "");
    s_WRITE(hpp, 4, "void test();");
    s_WRITE(hpp, 0, "};");
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

std::unique_ptr<pre_interaction> macro_saver::create_interaction(QObject* _target, QEvent* _e)
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
                    return std::make_unique<pre_interaction_model_view_select>(
                        reinterpret_cast<intptr_t>(_target),
                        find(lw),
                        Qt::KeyboardModifier::NoModifier,
                        MODEL_VIEW_SELECT_TYPE,
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
                    return std::make_unique<pre_interaction_model_view_select>(
                        reinterpret_cast<intptr_t>(cb),
                        find(cb),
                        Qt::KeyboardModifier::NoModifier,
                        MODEL_VIEW_SELECT_TYPE,
                        cb->model()->data(lv->indexAt(me->pos())).toString()
                    );
                }

                return nullptr;
            }

            if(auto* slider = qobject_cast<QSlider*>(_target))
            {
                if(_e->type() == QEvent::MouseButtonRelease)
                {
                    bool drag_in_progress = m_drag_in_progress;
                    m_drag_in_progress = false;
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
                            return std::make_unique<pre_interaction_number_input_modification>(
                                reinterpret_cast<intptr_t>(
                                    _target),
                                find(slider),
                                me->modifiers(),
                                NUMBER_INPUT_MODIFICATION_TYPE,
                                modification_t::decrement,
                                1
                            );
                        }

                        if(increment_zone.contains(me->pos()))
                        {
                            return std::make_unique<pre_interaction_number_input_modification>(
                                reinterpret_cast<intptr_t>(
                                    _target),
                                find(slider),
                                me->modifiers(),
                                NUMBER_INPUT_MODIFICATION_TYPE,
                                modification_t::increment,
                                1
                            );
                        }
                    }
                    else
                    {
                        return std::make_unique<pre_interaction_number_input_modification>(
                            reinterpret_cast<intptr_t>(_target),
                            find(slider),
                            me->modifiers(),
                            NUMBER_INPUT_MODIFICATION_TYPE,
                            modification_t::set,
                            slider->value()
                        );
                    }
                }
                else if(_e->type() == QEvent::MouseMove)
                {
                    m_drag_in_progress = true;
                }
                else if(_e->type() == QEvent::MouseButtonPress)
                {
                    m_drag_in_progress = false;
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
                        return std::make_unique<pre_interaction_number_input_modification>(
                            reinterpret_cast<intptr_t>(_target),
                            find(spinbox),
                            me->modifiers(),
                            NUMBER_INPUT_MODIFICATION_TYPE,
                            modification_t::decrement,
                            1
                        );
                    }

                    if(increment_zone.contains(me->pos()))
                    {
                        return std::make_unique<pre_interaction_number_input_modification>(
                            reinterpret_cast<intptr_t>(_target),
                            find(spinbox),
                            me->modifiers(),
                            NUMBER_INPUT_MODIFICATION_TYPE,
                            modification_t::increment,
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
                return std::make_unique<pre_interaction_model_view_select>(
                    reinterpret_cast<intptr_t>(button->window()),
                    find(button->window()),
                    me->modifiers(),
                    MODEL_VIEW_SELECT_TYPE,
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
                    return std::make_unique<pre_interaction_model_view_select>(
                        reinterpret_cast<intptr_t>(checkbox),
                        find(checkbox),
                        me->modifiers(),
                        MODEL_VIEW_SELECT_TYPE,
                        checkbox->isChecked() ? "true" : "false"
                    );
                }

                return nullptr;
            }

            if(_e->type() == QEvent::MouseButtonDblClick
               && _target->objectName() == m_main_window->windowTitle() + "Window")
            {
                return nullptr;
            }

            return std::make_unique<pre_interaction_mouse>(
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
               && static_cast<pre_interaction_keyboard&>(*m_interactions.back()).key == ke->key()
               && !ke->isAutoRepeat())
            {
                // Ignore duplicates (if the same key is pressed multiple times in a row without any releases, it must
                // be an auto repeat, else it means the event was duplicated).
                return nullptr;
            }

            return std::make_unique<pre_interaction_keyboard>(
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
            return std::make_unique<pre_interaction_mouse_wheel>(
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
