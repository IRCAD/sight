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

#include "ui/qt/namer.hpp"
#include "ui/qt/naming.hpp"

#include <service/base.hpp>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractSpinBox>
#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileDialog>
#include <QFontDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMetaObject>
#include <QPlainTextEdit>
#include <QPointer>
#include <QProgressBar>
#include <QProperty>
#include <QRadioButton>
#include <QSlider>
#include <QString>
#include <QTabBar>
#include <QTableView>
#include <QTextEdit>
#include <QToolButton>
#include <QTreeView>
#include <QVariant>
#include <QWidget>

#include <algorithm>
#include <array>
#include <stdexcept>

namespace sight::ui::qt::detail
{

namespace
{

/// The name a sanctioned path gave an object: fixed, or `<owner>/<part>` while bound to its owner.
struct name_record
{
    QString name;
    QPointer<const QObject> owner;
    bool bound {false};
};

} // namespace

// Kept out of any header: only the functions below can write it.
static constexpr auto RECORD_PROPERTY = "_sight_name_record";

//------------------------------------------------------------------------------

/// Refuses a caption, camelCase or an ordinal as a part, before anything is named.
static void require_part(QStringView _part)
{
    if(!is_part(_part))
    {
        throw std::invalid_argument("'" + _part.toString().toStdString() + "' is not a snake_case part");
    }
}

//------------------------------------------------------------------------------

void set_name(QObject& _object, const QString& _name)
{
    // Recorded: a later rename no longer matches the record.
    _object.setObjectName(_name);
    _object.setProperty(RECORD_PROPERTY, QVariant::fromValue(name_record {.name = _name, .owner = {}, .bound = false}));
}

//------------------------------------------------------------------------------

void set_part_name(QObject& _object, const QString& _owner, QStringView _part)
{
    require_part(_part);
    set_name(_object, _owner + u'/' + _part.toString());
}

//------------------------------------------------------------------------------

void bind_part_name(QObject& _object, const QObject& _owner, QStringView _part)
{
    require_part(_part);

    // Reading the owner's name in the binding makes it a dependency: the part follows its renames.
    const QPointer<const QObject> owner(&_owner);
    const QString part = _part.toString();
    _object.bindableObjectName().setBinding(
        [owner, part]
        {
            const QString name = owner.isNull() ? QString() : owner->objectName();
            return name.isEmpty() ? QString() : name + u'/' + part;
        });

    // The record keeps the owner, not a name: the expected name is computed when checked.
    const name_record bound {.name = part, .owner = owner, .bound = true};
    _object.setProperty(RECORD_PROPERTY, QVariant::fromValue(bound));
}

//------------------------------------------------------------------------------

bool is_part(QStringView _part) noexcept
{
    const auto is_lower = [](QChar _c){return _c >= u'a' && _c <= u'z';};
    const auto is_digit = [](QChar _c){return _c >= u'0' && _c <= u'9';};

    // A lower case letter first, never an underscore last.
    if(_part.isEmpty() || !is_lower(_part.front()) || _part.back() == u'_')
    {
        return false;
    }

    // Then letters and digits, an underscore only between them.
    for(qsizetype i = 1 ; i < _part.size() ; ++i)
    {
        const QChar c = _part[i];
        if(!is_lower(c) && !is_digit(c) && (c != u'_' || _part[i - 1] == u'_'))
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

bool has_sanctioned_name(const QObject& _object)
{
    // A name set by hand has no record, and a property set by hand is not one.
    const QVariant property = _object.property(RECORD_PROPERTY);
    if(!property.canConvert<name_record>())
    {
        return false;
    }

    // A bound part expects its owner's current name, a fixed one the name it was given.
    const auto entry = property.value<name_record>();
    QString expected = entry.name;
    if(entry.bound)
    {
        const QString owner = entry.owner.isNull() ? QString() : entry.owner->objectName();
        expected = owner.isEmpty() ? QString() : owner + u'/' + entry.name;
    }

    return !expected.isEmpty() && _object.objectName() == expected;
}

} // namespace sight::ui::qt::detail

namespace sight::ui::qt
{

static constexpr auto CONTAINER_PROPERTY = "sight_names_widgets_below";
static constexpr auto PART_PROPERTY      = "sight_part";

//------------------------------------------------------------------------------

static bool is_container(const QWidget& _widget)
{
    return _widget.property(CONTAINER_PROPERTY).isValid();
}

//------------------------------------------------------------------------------

/// The last component of the nearest Sight class of lib/ui/qt: such a widget is its own role, whatever Qt type it
/// derives from (`switch_button`, `tickmarks_slider`...).
static QString shared_widget_role(const QWidget& _widget)
{
    static const QString s_PREFIX = QStringLiteral("sight::ui::qt::");

    for(const QMetaObject* meta = _widget.metaObject() ; meta != nullptr ; meta = meta->superClass())
    {
        const QString class_name = QString::fromLatin1(meta->className());
        if(class_name.startsWith(s_PREFIX))
        {
            return class_name.mid(class_name.lastIndexOf(u':') + 1);
        }
    }

    return {};
}

//------------------------------------------------------------------------------

/// The role of a widget a user acts on or reads, empty for the others.
static QString role_of(const QWidget& _widget)
{
    if(QString shared = shared_widget_role(_widget); !shared.isEmpty())
    {
        return shared;
    }

    // The action of a tool button bears the name: one element, one name.
    if(const auto* const button = qobject_cast<const QToolButton*>(&_widget);
       button != nullptr && button->defaultAction() != nullptr && !button->defaultAction()->objectName().isEmpty())
    {
        return {};
    }

    struct role
    {
        const QMetaObject* type {nullptr};
        const char* name {nullptr};
    };

    // Most specific types first.
    static const std::array s_ROLES {
        role {.type = &QCheckBox::staticMetaObject, .name = "check_box"},
        role {.type = &QRadioButton::staticMetaObject, .name = "radio_button"},
        role {.type = &QAbstractButton::staticMetaObject, .name = "button"},
        role {.type = &QSlider::staticMetaObject, .name = "slider"},
        role {.type = &QLineEdit::staticMetaObject, .name = "line_edit"},
        role {.type = &QComboBox::staticMetaObject, .name = "combo_box"},
        role {.type = &QAbstractSpinBox::staticMetaObject, .name = "spin_box"},
        role {.type = &QTreeView::staticMetaObject, .name = "tree"},
        role {.type = &QTableView::staticMetaObject, .name = "table"},
        role {.type = &QListView::staticMetaObject, .name = "list"},
        role {.type = &QAbstractItemView::staticMetaObject, .name = "item_view"},
        role {.type = &QTextEdit::staticMetaObject, .name = "text_edit"},
        role {.type = &QPlainTextEdit::staticMetaObject, .name = "text_edit"},
        role {.type = &QTabBar::staticMetaObject, .name = "tab_bar"},
        role {.type = &QProgressBar::staticMetaObject, .name = "progress_bar"},
        role {.type = &QLabel::staticMetaObject, .name = "label"},
    };

    for(const role& candidate : s_ROLES)
    {
        if(_widget.metaObject()->inherits(candidate.type))
        {
            return QString::fromLatin1(candidate.name);
        }
    }

    return {};
}

//------------------------------------------------------------------------------

/// Whether the widgets below this one are its own business, not widgets of the service.
static bool owns_its_children(const QWidget& _widget)
{
    static const std::array<QString, 4> s_CONTAINERS {
        QStringLiteral("collapsible_section"),
        QStringLiteral("toolbox"),
        QStringLiteral("accordion_menu"),
        QStringLiteral("slide_bar")
    };

    const QString role = role_of(_widget);
    return !role.isEmpty() && std::ranges::find(s_CONTAINERS, role) == s_CONTAINERS.end();
}

//------------------------------------------------------------------------------

static QString button_box_role(const QDialogButtonBox& _box, QAbstractButton& _button)
{
    switch(_box.standardButton(&_button))
    {
        case QDialogButtonBox::Ok:
            return QStringLiteral("ok");

        case QDialogButtonBox::Cancel:
            return QStringLiteral("cancel");

        case QDialogButtonBox::Apply:
            return QStringLiteral("apply");

        case QDialogButtonBox::Yes:
            return QStringLiteral("yes");

        case QDialogButtonBox::No:
            return QStringLiteral("no");

        case QDialogButtonBox::Close:
            return QStringLiteral("close");

        case QDialogButtonBox::Save:
            return QStringLiteral("save");

        case QDialogButtonBox::Retry:
            return QStringLiteral("retry");

        default:
            break;
    }

    switch(_box.buttonRole(&_button))
    {
        case QDialogButtonBox::AcceptRole:
            return QStringLiteral("ok");

        case QDialogButtonBox::RejectRole:
            return QStringLiteral("cancel");

        case QDialogButtonBox::ApplyRole:
            return QStringLiteral("apply");

        default:
            return QStringLiteral("button");
    }
}

//------------------------------------------------------------------------------

/// A dialog of Qt, driven through the names Qt gives it, or none of its parts would be recognized.
static bool is_qt_dialog(const QDialog& _dialog)
{
    return qobject_cast<const QFileDialog*>(&_dialog) != nullptr
           || qobject_cast<const QColorDialog*>(&_dialog) != nullptr
           || qobject_cast<const QFontDialog*>(&_dialog) != nullptr;
}

//------------------------------------------------------------------------------

/// The name of what opened this window, a dialog of a service or none: the service that runs, else its parents.
static QString dialog_owner(const QDialog& _dialog)
{
    if(const std::string running = sight::service::base::running_id(); !running.empty())
    {
        return QString::fromStdString(running);
    }

    for(const QWidget* parent = _dialog.parentWidget() ; parent != nullptr ; parent = parent->parentWidget())
    {
        if(is_container(*parent))
        {
            return parent->objectName();
        }
    }

    return {};
}

//------------------------------------------------------------------------------

/// The name of the nearest container in the window of this widget. A role stops at a control that owns it, a part
/// does not: it is named on purpose.
static QString owner_of(const QWidget& _widget, bool _is_part)
{
    for(const QWidget* parent = _widget.parentWidget() ; parent != nullptr ; parent = parent->parentWidget())
    {
        if(is_container(*parent))
        {
            return parent->objectName();
        }

        if(parent->isWindow() || (!_is_part && owns_its_children(*parent)))
        {
            return {};
        }
    }

    return {};
}

//------------------------------------------------------------------------------

/// The role a widget is named after: a button of a dialog button box says what it does, not its caption.
static QString role_name(QWidget& _widget)
{
    auto* const button    = qobject_cast<QAbstractButton*>(&_widget);
    const auto* const box = qobject_cast<const QDialogButtonBox*>(_widget.parentWidget());
    return button != nullptr && box != nullptr ? button_box_role(*box, *button) : role_of(_widget);
}

//------------------------------------------------------------------------------

/// The window of a dialog that a service opened, or nullptr for the others.
static const QDialog* service_dialog(const QWidget& _widget)
{
    const auto* const dialog = qobject_cast<const QDialog*>(&_widget);
    return dialog != nullptr && dialog->isWindow() && !is_qt_dialog(*dialog) ? dialog : nullptr;
}

//------------------------------------------------------------------------------

static void name(QWidget& _widget)
{
    // A part of a shared widget follows its owner's name.
    if(_widget.bindableObjectName().hasBinding())
    {
        return;
    }

    const QVariant part_name = _widget.property(PART_PROPERTY);
    if(!part_name.isValid() && !_widget.objectName().isEmpty())
    {
        return;
    }

    const QDialog* const dialog = service_dialog(_widget);
    const QString owner         = dialog != nullptr ? dialog_owner(*dialog) : owner_of(_widget, part_name.isValid());
    if(owner.isEmpty())
    {
        return;
    }

    if(part_name.isValid())
    {
        // name_part() checked it; a property set by hand is no part, and an event filter must not throw.
        if(const QString component = part_name.toString(); detail::is_part(component))
        {
            detail::set_part_name(_widget, owner, component);
        }
    }
    else if(dialog != nullptr)
    {
        detail::set_name(_widget, owner + QStringLiteral("/dialog"));
    }
    else if(const QString role = role_name(_widget); !role.isEmpty())
    {
        detail::set_name(_widget, owner + u'/' + role);
    }

    // Its children are polished after it, and find it.
    if(dialog != nullptr)
    {
        _widget.setProperty(CONTAINER_PROPERTY, true);
    }
}

//------------------------------------------------------------------------------

namespace
{

class namer final : public QObject
{
public:

    using QObject::QObject;

    //------------------------------------------------------------------------------

    bool eventFilter(QObject* _watched, QEvent* _event) override
    {
        // Shown as well: a widget polished before it joined its service is named once it is there.
        if(_event->type() == QEvent::Polish || _event->type() == QEvent::Show)
        {
            if(auto* const widget = qobject_cast<QWidget*>(_watched); widget != nullptr)
            {
                name(*widget);
            }
        }

        return false;
    }
};

} // namespace

//------------------------------------------------------------------------------

/// One filter per application, gone with it: the first container or part of an application installs it.
static void install_namer()
{
    static QPointer<namer> s_namer;
    if(s_namer.isNull())
    {
        s_namer = new namer(qApp);
        qApp->installEventFilter(s_namer);
    }
}

//------------------------------------------------------------------------------

void name_widgets_below(QWidget& _container)
{
    install_namer();
    _container.setProperty(CONTAINER_PROPERTY, true);
}

//------------------------------------------------------------------------------

void name_part(QWidget& _widget, std::string_view _part)
{
    const QString component = QString::fromUtf8(_part.data(), static_cast<qsizetype>(_part.size()));
    if(!detail::is_part(component))
    {
        throw std::invalid_argument("'" + std::string(_part) + "' is not a snake_case part");
    }

    install_namer();
    _widget.setProperty(PART_PROPERTY, component);
    name(_widget);
}

} // namespace sight::ui::qt
