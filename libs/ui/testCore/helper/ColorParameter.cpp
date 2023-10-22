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

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

static std::string color_to_string(const QColor& _color)
{
    return "{" + std::to_string(_color.red()) + ", " + std::to_string(_color.green()) + ", "
           + std::to_string(_color.blue()) + "}";
}

//------------------------------------------------------------------------------

void ColorParameter::select(Tester& _tester, const Select& _color_param, const QColor& _color)
{
    auto bt = _tester.addInBacktrace(
        "selecting color " + color_to_string(_color) + " in" + _color_param.getDescription(
            _tester
        ) + "\" color parameter"
    );
    helper::Button::push(_tester, _color_param);
    helper::Dialog::take<QColorDialog*>(_tester, "color select dialog");
    QPointer<QColorDialog> color_dialog = _tester.get<QColorDialog*>();
    _tester.doSomethingAsynchronously<QColorDialog*>(
        [&_color](QColorDialog* _obj)
        {
            _obj->setCurrentColor(_color);
            _obj->accept();
        });
    _tester.doubt(
        "the color select window is closed",
        [&color_dialog](QObject*)
        {
            return color_dialog == nullptr || !color_dialog->isVisible();
        });
}

//------------------------------------------------------------------------------

void ColorParameter::colorEquals(Tester& _tester, const Select& _color_param, const QColor& _color)
{
    auto bt = _tester.addInBacktrace(
        "checking whether " + _color_param.getDescription(
            _tester
        ) + "\" color parameter has the color " + color_to_string(_color)
    );
    _color_param.select(_tester);
    _tester.doubt<QPushButton*>(
        _color_param.getDescription(_tester) + " should have the color " + color_to_string(_color),
        [&_color](QPushButton* _obj)
        {
            return _obj->icon().pixmap(1).toImage().pixelColor(0, 0) == _color;
        });
}

} // namespace sight::ui::test_core::helper
