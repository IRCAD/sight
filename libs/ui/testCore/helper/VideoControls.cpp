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

#include "VideoControls.hpp"

#include "Button.hpp"
#include "ComboBox.hpp"
#include "Dialog.hpp"
#include "FileDialog.hpp"
#include "selector_dialog.hpp"

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void VideoControls::load(Tester& _tester, const std::string& _controls_name, const std::filesystem::path& _path)
{
    // Some activity takes time to load and the control is not always there
    QTest::qWait(1000);

    _tester.take("controls", _controls_name);
    helper::ComboBox::select(_tester, Select::fromParent(_controls_name, "videoSelectorSrv"), "File...");
    helper::FileDialog::fill(_tester, _path);

    // If RealSense is activated, another dialog appears
    helper::Dialog::maybeTake(_tester, "choose video grabber implementation dialog");
    if(_tester.exists())
    {
        helper::selector_dialog::select(_tester, "OpenCV player");
    }
}

//------------------------------------------------------------------------------

void VideoControls::start(Tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.addInBacktrace("Start the video controlled by " + _controls_name);
    helper::Button::push(_tester, _controls_name + "/Start");
    _tester.shouldBeHidden(
        "start button",
        [&_tester, &_controls_name]
        {
            return _tester.getMainWindow()->findChild<QWidget*>(QString::fromStdString(_controls_name + "/Start"));
        });
}

//------------------------------------------------------------------------------

void VideoControls::pause(Tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.addInBacktrace("Pause the video controlled by " + _controls_name);
    helper::Button::push(_tester, _controls_name + "/Pause");
    _tester.shouldBeHidden(
        "pause button",
        [&_tester, &_controls_name]
        {
            return _tester.getMainWindow()->findChild<QWidget*>(QString::fromStdString(_controls_name + "/Pause"));
        });
}

//------------------------------------------------------------------------------

void VideoControls::play(Tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.addInBacktrace("Play the video controlled by " + _controls_name);
    helper::Button::push(_tester, _controls_name + "/Play");
    _tester.shouldBeHidden(
        "play button",
        [&_tester, &_controls_name]
        {
            return _tester.getMainWindow()->findChild<QWidget*>(QString::fromStdString(_controls_name + "/Play"));
        });
}

//------------------------------------------------------------------------------

void VideoControls::stop(Tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.addInBacktrace("Stop the video controlled by " + _controls_name);
    helper::Button::push(_tester, _controls_name + "/Stop");
}

//------------------------------------------------------------------------------

void VideoControls::loop(Tester& _tester, const std::string& _controls_name)
{
    auto bt = _tester.addInBacktrace("Loop the video controlled by " + _controls_name);
    helper::Button::push(_tester, _controls_name + "/Loop");
}

} // namespace sight::ui::test_core::helper
