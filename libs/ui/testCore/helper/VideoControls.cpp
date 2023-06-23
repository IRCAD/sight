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
#include "SelectorDialog.hpp"

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void VideoControls::load(Tester& tester, const std::string& controlsName, const std::filesystem::path& path)
{
    // Some activity takes time to load and the control is not always there
    QTest::qWait(1000);

    tester.take("controls", controlsName);
    helper::ComboBox::select(tester, Select::fromParent(controlsName, "videoSelectorSrv"), "File...");
    helper::FileDialog::fill(tester, path);

    // If RealSense is activated, another dialog appears
    helper::Dialog::maybeTake(tester, "choose video grabber implementation dialog");
    if(tester.exists())
    {
        helper::SelectorDialog::select(tester, "OpenCV player");
    }
}

//------------------------------------------------------------------------------

void VideoControls::start(Tester& tester, const std::string& controlsName)
{
    auto bt = tester.addInBacktrace("Start the video controlled by " + controlsName);
    helper::Button::push(tester, controlsName + "/Start");
    tester.shouldBeHidden(
        "start button",
        [&tester, &controlsName]
        {
            return tester.getMainWindow()->findChild<QWidget*>(QString::fromStdString(controlsName + "/Start"));
        });
}

//------------------------------------------------------------------------------

void VideoControls::pause(Tester& tester, const std::string& controlsName)
{
    auto bt = tester.addInBacktrace("Pause the video controlled by " + controlsName);
    helper::Button::push(tester, controlsName + "/Pause");
    tester.shouldBeHidden(
        "pause button",
        [&tester, &controlsName]
        {
            return tester.getMainWindow()->findChild<QWidget*>(QString::fromStdString(controlsName + "/Pause"));
        });
}

//------------------------------------------------------------------------------

void VideoControls::play(Tester& tester, const std::string& controlsName)
{
    auto bt = tester.addInBacktrace("Play the video controlled by " + controlsName);
    helper::Button::push(tester, controlsName + "/Play");
    tester.shouldBeHidden(
        "play button",
        [&tester, &controlsName]
        {
            return tester.getMainWindow()->findChild<QWidget*>(QString::fromStdString(controlsName + "/Play"));
        });
}

//------------------------------------------------------------------------------

void VideoControls::stop(Tester& tester, const std::string& controlsName)
{
    auto bt = tester.addInBacktrace("Stop the video controlled by " + controlsName);
    helper::Button::push(tester, controlsName + "/Stop");
}

//------------------------------------------------------------------------------

void VideoControls::loop(Tester& tester, const std::string& controlsName)
{
    auto bt = tester.addInBacktrace("Loop the video controlled by " + controlsName);
    helper::Button::push(tester, controlsName + "/Loop");
}

} // namespace sight::ui::testCore::helper
