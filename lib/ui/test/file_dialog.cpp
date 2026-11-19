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

#include "detail/file_dialog.hpp"

#include "detail/image.hpp"
#include "detail/window_input.hpp"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QApplication>
#include <QCompleter>
#include <QDialogButtonBox>
#include <QDir>
#include <QLineEdit>
#include <QMessageBox>

#include <stdexcept>
#include <utility>

namespace sight::ui::test::detail
{

file_dialog::file_dialog(std::vector<std::filesystem::path> _paths, QString _filter, bool _folder) :
    m_filter(std::move(_filter)),
    m_folder(_folder),
    m_path_count(_paths.size())
{
    // Typed in one field: several paths are written the way the dialog itself writes them.
    for(const auto& path : _paths)
    {
        if(path.empty() || !path.is_absolute())
        {
            throw std::invalid_argument("file dialog paths must be nonempty and absolute");
        }

        const QString text = QDir::toNativeSeparators(qt_path(path));
        if(text.contains(QChar('\0')) || text.contains('"') || text.contains('\n') || text.contains('\r'))
        {
            throw std::invalid_argument("file dialog paths cannot contain quotes or control characters");
        }

        if(!m_text.isEmpty())
        {
            m_text += ' ';
        }

        m_text += _paths.size() == 1 ? text : '"' + text + '"';
        m_paths.push_back(normalized(qt_path(path)));

        const QString directory = normalized(qt_path(path.parent_path()));
        if(!m_directory.isEmpty() && directory != m_directory)
        {
            throw std::invalid_argument("the files chosen at once must be in one folder");
        }

        m_directory = directory;
    }

    m_paths.sort();
}

//------------------------------------------------------------------------------

QString file_dialog::normalized(const QString& _path)
{
    return QDir::cleanPath(QDir::fromNativeSeparators(_path));
}

//------------------------------------------------------------------------------

bool file_dialog::selects_requested(const QFileDialog& _dialog) const
{
    QStringList selected;
    for(const QString& file : _dialog.selectedFiles())
    {
        selected.push_back(normalized(file));
    }

    selected.sort();
    return selected == m_paths;
}

//------------------------------------------------------------------------------

QFileDialog& file_dialog::target(QWidget& _modal)
{
    auto* const dialog = qobject_cast<QFileDialog*>(&_modal);
    if(dialog == nullptr)
    {
        throw std::invalid_argument("the active modal dialog is not a file dialog");
    }

    // The first dialog seen is the one to follow: another one is not what the scenario was told about.
    if(!m_bound)
    {
        m_dialog = dialog;
        m_bound  = true;
    }
    else if(m_dialog != dialog)
    {
        throw std::runtime_error("the original file dialog was replaced");
    }

    return *dialog;
}

//------------------------------------------------------------------------------

static void close_completion(QLineEdit& _edit)
{
    if(const QCompleter* completer = _edit.completer();
       completer != nullptr && completer->popup()->isVisible())
    {
        completer->popup()->hide();
    }
}

//------------------------------------------------------------------------------

void file_dialog::prepare(QWidget& _modal)
{
    // Misuses first: waiting would not fix them.
    QFileDialog& dialog  = target(_modal);
    const bool directory = dialog.fileMode() == QFileDialog::Directory;
    if(directory != m_folder)
    {
        throw std::invalid_argument("choose_file/choose_folder does not match the file dialog mode");
    }

    if(m_path_count > 1 && dialog.fileMode() != QFileDialog::ExistingFiles)
    {
        throw std::invalid_argument("choosing several files requires an ExistingFiles dialog");
    }

    // Qt's dialog is not what a scenario tests: its own API drives it, where input could fail on Qt's timing.
    if(!m_filter.isEmpty())
    {
        if(!dialog.nameFilters().contains(m_filter))
        {
            throw std::invalid_argument("the file dialog has no filter '" + m_filter.toStdString() + "'");
        }

        dialog.selectNameFilter(m_filter);
    }

    QPointer<QLineEdit> edit = dialog.findChild<QLineEdit*>("fileNameEdit");
    if(edit.isNull())
    {
        throw std::invalid_argument("the file dialog has no fileNameEdit");
    }

    close_completion(*edit);

    // From its folder: the dialog then writes the name of the file it selects, which still means that file.
    if(normalized(dialog.directory().absolutePath()) != m_directory)
    {
        dialog.setDirectory(m_directory);
    }

    if(!selects_requested(dialog))
    {
        edit->setText(m_text);
        if(edit.isNull() || !selects_requested(dialog))
        {
            throw input_failure("the file dialog did not retain the requested path", false);
        }
    }
}

//------------------------------------------------------------------------------

static void click_role(QFileDialog& _dialog, QDialogButtonBox::ButtonRole _role)
{
    auto* const box = _dialog.findChild<QDialogButtonBox*>();
    if(box == nullptr)
    {
        throw std::invalid_argument("the file dialog has no button box");
    }

    for(QAbstractButton* const button : box->buttons())
    {
        if(box->buttonRole(button) == _role)
        {
            // click() ignores a disabled button silently, where a user sees why nothing happens.
            if(!button->isVisible() || !button->isEnabled())
            {
                throw input_failure("button '" + describe_object(button) + "' is hidden or disabled");
            }

            button->click();
            return;
        }
    }

    throw std::invalid_argument("the file dialog has no button with the requested role");
}

//------------------------------------------------------------------------------

void file_dialog::accept(QWidget& _modal)
{
    // The path was checked by prepare(), but the dialog runs between the two.
    QFileDialog& dialog = target(_modal);
    auto* const edit    = dialog.findChild<QLineEdit*>("fileNameEdit");
    if(edit == nullptr || !selects_requested(dialog))
    {
        throw input_failure("the file dialog path changed before acceptance", false);
    }

    close_completion(*edit);
    click_role(dialog, QDialogButtonBox::AcceptRole);
}

//------------------------------------------------------------------------------

void file_dialog::cancel(QWidget& _modal)
{
    QFileDialog& dialog = target(_modal);

    // It may have opened at any time, once the folder of the typed path was loaded.
    if(auto* const edit = dialog.findChild<QLineEdit*>("fileNameEdit"); edit != nullptr)
    {
        close_completion(*edit);
    }

    click_role(dialog, QDialogButtonBox::RejectRole);
}

//------------------------------------------------------------------------------

file_dialog::status file_dialog::observe() const
{
    if(m_dialog.isNull() || !m_dialog->isVisible())
    {
        return status::closed;
    }

    // Its own question is expected, another dialog is not: it would keep this one open until the deadline.
    QWidget* const modal = QApplication::activeModalWidget();
    if(modal != nullptr && modal != m_dialog && !is_owned_by(*modal, *m_dialog))
    {
        throw std::runtime_error("an unrelated modal dialog interrupted file selection");
    }

    return qobject_cast<QMessageBox*>(modal) != nullptr ? status::overwrite : status::open;
}

//------------------------------------------------------------------------------

void file_dialog::confirm_overwrite(QWidget& _modal)
{
    // Only the overwrite question of a save dialog: a warning about a missing file is a failure of the test.
    auto* const question = qobject_cast<QMessageBox*>(&_modal);
    if(m_dialog.isNull() || m_dialog->acceptMode() != QFileDialog::AcceptSave
       || question == nullptr || !is_owned_by(*question, *m_dialog)
       || question->standardButtons() != (QMessageBox::Yes | QMessageBox::No))
    {
        throw std::runtime_error("the file dialog opened an unexpected confirmation");
    }

    question->button(QMessageBox::Yes)->click();
}

} // namespace sight::ui::test::detail
