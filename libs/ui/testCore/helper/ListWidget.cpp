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

#include "ListWidget.hpp"

#include <QListWidget>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void ListWidget::countEquals(Tester& tester, const Select& list, int expected)
{
    auto bt = tester.addInBacktrace(
        "checking if \"" + list.getDescription(tester) + "\" list has " + std::to_string(expected) + " elements"
    );
    list.select(tester);
    tester.doubt<QListWidget*>(
        '"' + list.getDescription(tester) + "\" list has " + std::to_string(expected) + " elements",
        [&expected](QListWidget* list)
        {
            return list->count() == expected;
        });
}

//------------------------------------------------------------------------------

void ListWidget::setCurrentRow(Tester& tester, const Select& list, int index)
{
    auto bt = tester.addInBacktrace(
        "setting \"" + list.getDescription(tester) + "\" list current index to " + std::to_string(index)
    );
    list.select(tester);
    tester.doSomethingAsynchronously<QListWidget*>([index](QListWidget* l){l->setCurrentRow(index);});
}

//------------------------------------------------------------------------------

void ListWidget::setCurrentText(Tester& tester, const Select& list, const std::string& text)
{
    auto bt = tester.addInBacktrace(
        "setting \"" + list.getDescription(tester) + "\" list current text to \"" + text + '"'
    );
    list.select(tester);
    QString qText = QString::fromStdString(text);
    tester.doSomethingAsynchronously<QListWidget*>(
        [&qText](QListWidget* current)
        {
            current->setCurrentItem(current->findItems(qText, Qt::MatchExactly)[0]);
        });
    tester.doubt<QListWidget*>(
        "the selection is selected",
        [&qText](QListWidget* current)
        {
            return current->currentItem()->text() == qText;
        });
}

} // namespace sight::ui::testCore::helper
