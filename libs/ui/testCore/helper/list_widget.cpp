/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "list_widget.hpp"

#include <QListWidget>

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void list_widget::count_equals(tester& _tester, const selector& _list, int _expected)
{
    auto bt = _tester.add_in_backtrace(
        "checking if \"" + _list.get_description(_tester) + "\" list has " + std::to_string(_expected) + " elements"
    );
    _list.select(_tester);
    _tester.doubt<QListWidget*>(
        '"' + _list.get_description(_tester) + "\" list has " + std::to_string(_expected) + " elements",
        [&_expected](QListWidget* _list)
        {
            return _list->count() == _expected;
        });
}

//------------------------------------------------------------------------------

void list_widget::set_current_row(tester& _tester, const selector& _list, int _index)
{
    auto bt = _tester.add_in_backtrace(
        "setting \"" + _list.get_description(_tester) + "\" list current index to " + std::to_string(_index)
    );
    _list.select(_tester);
    _tester.do_something_asynchronously<QListWidget*>([_index](QListWidget* _l){_l->setCurrentRow(_index);});
}

//------------------------------------------------------------------------------

void list_widget::set_current_text(tester& _tester, const selector& _list, const std::string& _text)
{
    auto bt = _tester.add_in_backtrace(
        "setting \"" + _list.get_description(_tester) + "\" list current text to \"" + _text + '"'
    );
    _list.select(_tester);
    QString q_text = QString::fromStdString(_text);
    _tester.do_something_asynchronously<QListWidget*>(
        [&q_text](QListWidget* _current)
        {
            _current->setCurrentItem(_current->findItems(q_text, Qt::MatchExactly)[0]);
        });
    _tester.doubt<QListWidget*>(
        "the selection is selected",
        [&q_text](QListWidget* _current)
        {
            return _current->currentItem()->text() == q_text;
        });
}

} // namespace sight::ui::test_core::helper
