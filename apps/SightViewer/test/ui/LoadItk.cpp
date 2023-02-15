/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "LoadItk.hpp"

#include <utestData/Data.hpp>

#include <ui/testCore/helper/Button.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::LoadItk);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void LoadItk::test()
{
    namespace helper = sight::ui::testCore::helper;
    const std::string testName               = "sightViewerLoadItkTest";
    const std::string imageName              = testName + ".png";
    const std::filesystem::path snapshotPath = sight::ui::testCore::Tester::getImageOutputPath() / imageName;
    std::filesystem::remove(snapshotPath);

    const std::filesystem::path referencePath(utestData::Data::dir() / "sight/ui/SightViewer" / imageName);

    start(
        testName,
        [&snapshotPath, &referencePath](sight::ui::testCore::Tester& tester)
        {
            openFile(
                tester,
                "Nifti or Inr images",
                utestData::Data::dir() / "sight/image/inr/image.inr.gz"
            );

            helper::Button::push(tester, "toolBarView/Show/hide volume");

            saveSnapshot(tester, snapshotPath);

            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
