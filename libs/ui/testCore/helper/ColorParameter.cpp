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

#include "ColorParameter.hpp"

#include "Button.hpp"
#include "Dialog.hpp"

#include <QColorDialog>
#include <QPushButton>

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

static std::string colorToString(const QColor& color)
{
    return "{" + std::to_string(color.red()) + ", " + std::to_string(color.green()) + ", "
           + std::to_string(color.blue()) + "}";
}

//------------------------------------------------------------------------------

void ColorParameter::select(Tester& tester, const Select& colorParam, const QColor& color)
{
    auto bt = tester.addInBacktrace(
        "selecting color " + colorToString(color) + " in" + colorParam.getDescription(
            tester
        ) + "\" color parameter"
    );
    helper::Button::push(tester, colorParam);
    helper::Dialog::take<QColorDialog*>(tester, "color select dialog");
    tester.doSomethingAsynchronously<QColorDialog*>(
        [&color](QColorDialog* obj)
        {
            obj->setCurrentColor(color);
            obj->accept();
        });
}

//------------------------------------------------------------------------------

void ColorParameter::colorEquals(Tester& tester, const Select& colorParam, const QColor& color)
{
    auto bt = tester.addInBacktrace(
        "checking whether " + colorParam.getDescription(
            tester
        ) + "\" color parameter has the color " + colorToString(color)
    );
    colorParam.select(tester);
    tester.doubt<QPushButton*>(
        colorParam.getDescription(tester) + " should have the color " + colorToString(color),
        [&color](QPushButton* obj)
        {
            return obj->icon().pixmap(1).toImage().pixelColor(0, 0) == color;
        });
}

} // namespace sight::ui::testCore::helper
