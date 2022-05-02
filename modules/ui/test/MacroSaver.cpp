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
// cspell:ignore ilwc strat

#include "MacroSaver.hpp"

#include <core/runtime/operations.hpp>
#include <core/runtime/profile/Profile.hpp>

#include <QAction>
#include <QFile>
#include <QListWidget>
#include <QMetaEnum>
#include <QTest>

const QEvent::Type ListWidgetClick = static_cast<QEvent::Type>(QEvent::registerEventType());

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
            if(modifiers & value)
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

//------------------------------------------------------------------------------

InteractionKeyboard::InteractionKeyboard(Qt::Key key, const QString& sequence) :
    key(key),
    sequence(sequence)
{
}

//------------------------------------------------------------------------------

bool InteractionKeyboard::isPrintable()
{
    return !sequence.isEmpty() && sequence[0].isPrint();
}

//------------------------------------------------------------------------------

InteractionListWidgetClick::InteractionListWidgetClick(const QString& name) :
    name(name)
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

PreInteractionListWidgetClick::PreInteractionListWidgetClick(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    QEvent::Type type,
    const QString& name
) :
    PreInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionListWidgetClick(name)
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

PostInteractionListWidgetClick::PostInteractionListWidgetClick(
    intptr_t receiverId,
    const QVector<FindStrategy>& howToFindReceiver,
    Qt::KeyboardModifiers modifiers,
    InteractionType type,
    const QString& name
) :
    PostInteraction(receiverId, howToFindReceiver, modifiers, type),
    InteractionListWidgetClick(name)
{
}

//------------------------------------------------------------------------------

bool MacroSaver::eventFilter(QObject* target, QEvent* e)
{
    if(e->type() == QEvent::ChildAdded)
    {
        QChildEvent* ce = dynamic_cast<QChildEvent*>(e);
        infect(ce->child());
        if(m_mainWindow == nullptr)
        {
            m_mainWindow = qApp->activeWindow();
        }
    }
    else if(e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonRelease
            || e->type() == QEvent::MouseMove || e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease)
    {
        // Ignore the MouseMove event if no buttons are pressed, else we will get one thousand of events
        QWidget* w = qobject_cast<QWidget*>(target);
        if((e->type() != QEvent::MouseMove || dynamic_cast<QMouseEvent*>(e)->buttons() != Qt::MouseButton::NoButton)
           && !(w != nullptr && w->isWindow()) && target != m_mainWindow)
        {
            std::shared_ptr<PreInteraction> interaction = createInteraction(target, e);
            if(interaction != nullptr)
            {
                m_interactions.emplace_back(interaction);
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

QVector<FindStrategy> MacroSaver::find(QObject* o)
{
    assert(o != nullptr);
    QWidget* w           = qobject_cast<QWidget*>(o);
    QByteArray className = o->metaObject()->className();
    if(className == "QWidgetWindow") // QWidgetWindow is a private class of Qt, we can't use it as is.
    {
        className = "QWidget";
    }

    if(o == m_mainWindow)
    {
        return {{FindStrategyType::ROOT, className, "", 0}};
    }
    else if(o == qApp->activeModalWidget())
    {
        return {{FindStrategyType::ACTIVE_MODAL_WIDGET, className, w->windowTitle(), 0}};
    }
    else if(o->objectName() != "")
    {
        QVector<FindStrategy> res {{FindStrategyType::OBJECT_NAME, className, o->objectName().toLatin1(), 0}};
        if(findChild(m_mainWindow, className, o->objectName()) == o)
        {
            res.append(find(m_mainWindow));
        }
        else if(w != nullptr && findChild(w->window(), className, o->objectName()) == o)
        {
            res.append(find(w->window()));
        }
        else
        {
            res.append({FindStrategyType::CANT_BE_FOUND, className, "", 0});
        }

        return res;
    }
    else if(findChildren(m_mainWindow, className).size() == 1)
    {
        return {{FindStrategyType::GLOBAL_TYPE, className, "", 0}};
    }
    else if(w != nullptr && w->actions().size() == 1)
    {
        QVector<FindStrategy> res {{FindStrategyType::ACTION, className, "", 0}};
        res.append(find(w->actions()[0]));
        return res;
    }
    else if(o->parent() == nullptr)
    {
        return {{FindStrategyType::CANT_BE_FOUND, className, "", 0}};
    }
    else if(findChildren(o->parent(), className, "", Qt::FindDirectChildrenOnly).size() == 1)
    {
        QVector<FindStrategy> res {{FindStrategyType::LOCAL_TYPE, className, "", 0}};
        res.append(find(o->parent()));
        return res;
    } // else

    int index = 0;
    for( ; index < o->parent()->children().size() ; index++)
    {
        if(o->parent()->children()[index] == o)
        {
            break;
        }
    }

    QVector<FindStrategy> res {{FindStrategyType::NTH_CHILD, className, "", index}};
    res.append(find(o->parent()));
    return res;
}

//------------------------------------------------------------------------------

void MacroSaver::save()
{
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
    write(cpp, 0, "#include <core/runtime/operations.hpp>");
    write(cpp, 0, "#include <ui/test/Tester.hpp>");
    write(cpp, 0, "");
    QStringList dependencies {"QObject"};
    for(const std::shared_ptr<PreInteraction>& interaction : m_interactions)
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
        }

        if(interaction->type == QEvent::Type::MouseButtonPress || interaction->type == QEvent::Type::MouseMove
           || interaction->type == QEvent::Type::MouseButtonRelease)
        {
            dependencies.append("QPoint");
        }
        else if(interaction->type == ListWidgetClick)
        {
            dependencies.append("QListWidget");
        }
    }

    dependencies.removeDuplicates();
    dependencies.sort();
    for(const QString& dependency : dependencies)
    {
        write(cpp, 0, QString("#include <%1>").arg(dependency));
    }

    write(cpp, 0, "");
    write(cpp, 0, "CPPUNIT_TEST_SUITE_REGISTRATION(GuiTest);");
    write(cpp, 0, "");
    write(cpp, 0, "//------------------------------------------------------------------------------");
    write(cpp, 0, "");
    write(cpp, 0, "const char* GuiTest::getProfilePath()");
    write(cpp, 0, "{");
    std::filesystem::path absoluteProfilePath(sight::core::runtime::getCurrentProfile()->getFilePath());
    QString profilePath(QString::fromStdString(std::filesystem::relative(absoluteProfilePath).string()));
    write(cpp, 4, QString("return \"%1\";").arg(profilePath).toLatin1());
    write(cpp, 0, "}");

    // Interactions preprocessing
    std::vector<std::shared_ptr<PostInteraction> > postInteractions;
    for(size_t i = 0 ; i < m_interactions.size() ; i++)
    {
        if(m_interactions[i]->type == QEvent::Type::MouseButtonPress)
        {
            std::shared_ptr<PreInteractionMouse> im = std::static_pointer_cast<PreInteractionMouse>(m_interactions[i]);
            if(i + 1 == m_interactions.size())
            {
                // This mouse button press event is the last event, it won't be ended with a release. Ignore it.
                continue;
            }
            else if(m_interactions[i + 1]->type == QEvent::Type::MouseButtonRelease)
            {
                // A mouse button press immediately followed with a mouse button release is a simple mouse click.
                postInteractions.emplace_back(
                    std::make_shared<PostInteractionMouse>(
                        im->receiverId,
                        im->howToFindReceiver,
                        im->modifiers,
                        MOUSE_CLICK,
                        im->from,
                        im->to,
                        im->button
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
                    std::shared_ptr<PreInteractionMouse> release = std::static_pointer_cast<PreInteractionMouse>(
                        m_interactions[i]
                    );
                    postInteractions.emplace_back(
                        std::make_shared<PostInteractionMouse>(
                            im->receiverId,
                            im->howToFindReceiver,
                            im->modifiers,
                            MOUSE_DRAG,
                            im->from,
                            release->to,
                            im->button
                        )
                    );
                    i++;
                }
            }
        }
        else if(m_interactions[i]->type == QEvent::Type::KeyPress)
        {
            const std::shared_ptr<PreInteractionKeyboard>& ik = std::static_pointer_cast<PreInteractionKeyboard>(
                m_interactions[i]
            );
            if(ik->isPrintable())
            {
                // If the key is printable, we will attempt to "compress" it with the following key events if they are
                // printable as well, to make the resulting code more readable.
                QString sequence;
                std::shared_ptr<PreInteractionKeyboard> press;
                while(i < m_interactions.size() && m_interactions[i]->type == QEvent::Type::KeyPress
                      && (press = std::static_pointer_cast<PreInteractionKeyboard>(m_interactions[i]))
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
                    postInteractions.emplace_back(
                        std::make_shared<PostInteractionKeyboard>(
                            ik->receiverId,
                            ik->howToFindReceiver,
                            ik->modifiers,
                            (sequence.size() > 1 ? KEYBOARD_SEQUENCE : KEYBOARD_CLICK),
                            ik->key,
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
                    postInteractions.emplace_back(
                        std::make_shared<PostInteractionKeyboard>(
                            ik->receiverId,
                            ik->howToFindReceiver,
                            ik->modifiers,
                            KEYBOARD_CLICK,
                            ik->key,
                            ik->sequence
                        )
                    );
                }
            }
        }
        else if(m_interactions[i]->type == ListWidgetClick)
        {
            std::shared_ptr<PreInteractionListWidgetClick> ilwc =
                std::static_pointer_cast<PreInteractionListWidgetClick>(m_interactions[i]);
            postInteractions.emplace_back(
                std::make_shared<PostInteractionListWidgetClick>(
                    ilwc->receiverId,
                    ilwc->howToFindReceiver,
                    ilwc->modifiers,
                    LIST_WIDGET_CLICK,
                    ilwc->name
                )
            );
        }
    }

    write(cpp, 0, "void GuiTest::test()");
    write(cpp, 0, "{");
    write(cpp, 4, "start(\"GuiTest\",");
    write(cpp, 8, "[](sight::ui::test::Tester& tester)");
    write(cpp, 4, "{");

    intptr_t currentItemId = 0;
    for(const std::shared_ptr<PostInteraction>& interaction : postInteractions)
    {
        if(interaction->receiverId != currentItemId)
        {
            // The receiver of this interaction isn't the current item: We must take it.
            for(int i = interaction->howToFindReceiver.size() - 1 ; i >= 0 ; i--)
            {
                FindStrategy strat = interaction->howToFindReceiver[i];
                if(strat.type == FindStrategyType::ROOT)
                {
                    write(cpp, 8, "tester.take(\"main window\", tester.getMainWindow());");
                }
                else if(strat.type == FindStrategyType::ACTIVE_MODAL_WIDGET)
                {
                    write(cpp, 8, "tester.take<QWidget*>(");
                    write(cpp, 12, QString("\"\\\"%1\\\" window\", ").arg(strat.string));
                    write(cpp, 12, "[]() -> QWidget* {return qApp->activeModalWidget();},");
                    write(
                        cpp,
                        12,
                        QString("[](QWidget* obj) -> bool {return obj->windowTitle() == \"%1\";}").arg(strat.string)
                    );
                    write(cpp, 8, ");");
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
                        "return sight::ui::test::Tester::getWidgetFromAction(qobject_cast<QAction*>(old));"
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
            std::shared_ptr<PostInteractionMouse> im = std::static_pointer_cast<PostInteractionMouse>(interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::test::MouseClick>(Qt::MouseButton::%1, %2, %3));")
                .arg(mouseButton.valueToKey(static_cast<int>(im->button))).arg(modifiersToString(im->modifiers))
                .arg(QTest::toString(im->from))
            );
        }
        else if(interaction->type == MOUSE_DRAG)
        {
            std::shared_ptr<PostInteractionMouse> im = std::static_pointer_cast<PostInteractionMouse>(interaction);
            write(
                cpp,
                8,
                QString(
                    "tester.interact(std::make_unique<sight::ui::test::MouseDrag>(%1, %2, Qt::MouseButton::%3, %4));"
                )
                .arg(QTest::toString(im->from)).arg(QTest::toString(im->to))
                .arg(mouseButton.valueToKey(static_cast<int>(im->button))).arg(modifiersToString(im->modifiers))
            );
        }
        else if(interaction->type == KEYBOARD_CLICK)
        {
            std::shared_ptr<PostInteractionKeyboard> ik =
                std::static_pointer_cast<PostInteractionKeyboard>(interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::test::KeyboardClick>(Qt::Key::%1, %2));")
                .arg(key.valueToKey(static_cast<int>(ik->key))).arg(modifiersToString(ik->modifiers))
            );
        }
        else if(interaction->type == KEYBOARD_SEQUENCE)
        {
            std::shared_ptr<PostInteractionKeyboard> ik =
                std::static_pointer_cast<PostInteractionKeyboard>(interaction);
            write(
                cpp,
                8,
                QString("tester.interact(std::make_unique<sight::ui::test::KeyboardSequence>(\"%1\", %2));")
                .arg(ik->sequence).arg(modifiersToString(ik->modifiers))
            );
        }
        else if(interaction->type == LIST_WIDGET_CLICK)
        {
            std::shared_ptr<PostInteractionListWidgetClick> ilwc =
                std::static_pointer_cast<PostInteractionListWidgetClick>(interaction);
            write(cpp, 8, "tester.doSomethingAsynchronously<QListWidget*>(");
            write(cpp, 12, "[](QListWidget* obj)");
            write(cpp, 8, "{");
            write(
                cpp,
                12,
                QString("obj->setCurrentItem(obj->findItems(\"%1\", Qt::MatchFlag::MatchExactly)[0]);")
                .arg(ilwc->name)
            );
            write(cpp, 8, "});");
        }
    }

    write(cpp, 4, "});");
    write(cpp, 0, "}");

    QFile hpp("GuiTest.hpp");
    hpp.open(QIODevice::WriteOnly);
    write(hpp, 0, "#pragma once");
    write(hpp, 0, "");
    write(hpp, 0, "#include <ui/test/ITest.hpp>");
    write(hpp, 0, "");
    write(hpp, 0, "#include <core/runtime/profile/Profile.hpp>");
    write(hpp, 0, "");
    write(hpp, 0, "#include <cppunit/extensions/HelperMacros.h>");
    write(hpp, 0, "");
    write(hpp, 0, "class GuiTest : public sight::ui::test::ITest");
    write(hpp, 0, "{");
    write(hpp, 0, "CPPUNIT_TEST_SUITE(GuiTest);");
    write(hpp, 0, "CPPUNIT_TEST(test);");
    write(hpp, 0, "CPPUNIT_TEST_SUITE_END();");
    write(hpp, 0, "");
    write(hpp, 0, "public:");
    write(hpp, 0, "");
    write(hpp, 4, "const char* getProfilePath() override;");
    write(hpp, 0, "");
    write(hpp, 4, "void test();");
    write(hpp, 0, "};");
}

//------------------------------------------------------------------------------

std::shared_ptr<PreInteraction> MacroSaver::createInteraction(QObject* target, QEvent* e)
{
    switch(e->type())
    {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        {
            QMouseEvent* me = dynamic_cast<QMouseEvent*>(e);
            QListWidget* lw = qobject_cast<QListWidget*>(target->parent());
            if(lw != nullptr)
            {
                if(e->type() == QEvent::MouseButtonPress)
                {
                    return std::make_shared<PreInteractionListWidgetClick>(
                        reinterpret_cast<intptr_t>(target),
                        find(lw),
                        Qt::KeyboardModifier::NoModifier,
                        ListWidgetClick,
                        lw->itemAt(me->pos())->text()
                    );
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return std::make_shared<PreInteractionMouse>(
                    reinterpret_cast<intptr_t>(target),
                    find(target),
                    me->modifiers(),
                    e->type(),
                    me->pos(),
                    me->pos(),
                    me->button()
                );
            }
        }

        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            QKeyEvent* ke = dynamic_cast<QKeyEvent*>(e);
            QObject* receiver;
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
            else if(e->type() == QEvent::KeyPress && !m_interactions.empty()
                    && m_interactions.back()->type == QEvent::KeyPress
                    && std::static_pointer_cast<PreInteractionKeyboard>(m_interactions.back())->key == ke->key()
                    && !ke->isAutoRepeat())
            {
                // Ignore duplicates (if the same key is pressed multiple times in a row without any releases, it must
                // be an auto repeat, else it means the event was duplicated).
                return nullptr;
            }
            else
            {
                return std::make_shared<PreInteractionKeyboard>(
                    reinterpret_cast<intptr_t>(receiver),
                    find(receiver),
                    ke->modifiers(),
                    e->type(),
                    static_cast<Qt::Key>(ke->key()),
                    ke->text()
                );
            }
        }

        default:
            return nullptr;
    }
}
