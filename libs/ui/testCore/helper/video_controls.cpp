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

#include "video_controls.hpp"

#include "button.hpp"
#include "combo_box.hpp"
#include "dialog.hpp"
#include "file_dialog.hpp"
#include "selector_dialog.hpp"

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void video_controls::load(tester& _tester, const std::string& _controls_name, const std::filesystem::path& _path)
{
    // Some activity takes time to load and the control is not always there
    QTest::qWait(1000);

    _tester.take("controls", _controls_name);
    helper::combo_box::select(_tester, selector::from_parent(_controls_name, "videoSelectorSrv"), "File...");
    helper::file_dialog::fill(_tester, _path);

    // If RealSense is activated, another dialog appears
    helper::dialog::maybe_take(_tester, "choose video grabber implementation dialog");
    if(_tester.exists())
    {
        helper::selector_dialog::select(_tester, "OpenCV player");
    }
}

//------------------------------------------------------------------------------

void video_controls::start(tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.add_in_backtrace("Start the video controlled by " + _controls_name);
    helper::button::push(_tester, _controls_name + "/Start");
    _tester.should_be_hidden(
        "start button",
        [&_tester, &_controls_name]
        {
            return _tester.get_main_window()->findChild<QWidget*>(QString::fromStdString(_controls_name + "/Start"));
        });
}

//------------------------------------------------------------------------------

void video_controls::pause(tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.add_in_backtrace("Pause the video controlled by " + _controls_name);
    helper::button::push(_tester, _controls_name + "/Pause");
    _tester.should_be_hidden(
        "pause button",
        [&_tester, &_controls_name]
        {
            return _tester.get_main_window()->findChild<QWidget*>(QString::fromStdString(_controls_name + "/Pause"));
        });
}

//------------------------------------------------------------------------------

void video_controls::play(tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.add_in_backtrace("Play the video controlled by " + _controls_name);
    helper::button::push(_tester, _controls_name + "/Play");
    _tester.should_be_hidden(
        "play button",
        [&_tester, &_controls_name]
        {
            return _tester.get_main_window()->findChild<QWidget*>(QString::fromStdString(_controls_name + "/Play"));
        });
}

//------------------------------------------------------------------------------

void video_controls::stop(tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.add_in_backtrace("Stop the video controlled by " + _controls_name);
    helper::button::push(_tester, _controls_name + "/Stop");
}

//------------------------------------------------------------------------------

void video_controls::loop(tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.add_in_backtrace("Loop the video controlled by " + _controls_name);
    helper::button::push(_tester, _controls_name + "/Loop");
}

} // namespace sight::ui::test_core::helper
