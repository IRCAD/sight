/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include "load_vtk.hpp"
#include <QPainter>

#include <ui/test/helper/button.hpp>

#include <utest_data/data.hpp>

#include <QImage>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_viewer::uit::load_vtk);

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

static void normalize_snapshot_to_reference(
    const std::filesystem::path& _snapshot_path,
    const std::filesystem::path& _reference_path
)
{
    const QImage snapshot(QString::fromStdString(_snapshot_path.string()));
    const QImage reference(QString::fromStdString(_reference_path.string()));

    CPPUNIT_ASSERT(!snapshot.isNull());
    CPPUNIT_ASSERT(!reference.isNull());

    QImage normalized(reference.size(), QImage::Format_RGBA8888);
    normalized.fill(Qt::black);

    {
        QPainter painter(&normalized);
        painter.drawImage(
            QPoint(
                (reference.width() - snapshot.width()) / 2,
                (reference.height() - snapshot.height()) / 2
            ),
            snapshot
        );
    }

    CPPUNIT_ASSERT(normalized.save(QString::fromStdString(_snapshot_path.string())));
}

//------------------------------------------------------------------------------

void load_vtk::test()
{
    namespace helper = sight::ui::test::helper;
    const auto snapshot_show1_path(sight::ui::test::tester::get_image_output_path() / "sightViewerLoadVtkTest_1.png");
    if(std::filesystem::exists(snapshot_show1_path))
    {
        std::filesystem::remove(snapshot_show1_path);
    }

    const auto snapshot_hide_path(sight::ui::test::tester::get_image_output_path() / "sightViewerLoadVtkTest_2.png");
    if(std::filesystem::exists(snapshot_hide_path))
    {
        std::filesystem::remove(snapshot_hide_path);
    }

    const auto snapshot_show2_path(sight::ui::test::tester::get_image_output_path() / "sightViewerLoadVtkTest_3.png");
    if(std::filesystem::exists(snapshot_show2_path))
    {
        std::filesystem::remove(snapshot_show2_path);
    }

    const auto* const  dir = "sight/ui/sight_viewer";
    const std::filesystem::path reference_show_path(utest_data::dir() / dir / "sightViewerLoadVtkTest.png");
    const std::filesystem::path reference_hide_path(utest_data::dir() / dir / "sightViewerHideMesh.png");

    const std::string test_name = "sightViewerLoadVtkTest";
    start(
        test_name,
        [&](sight::ui::test::tester& _tester)
        {
            open_file(
                _tester,
                "VTK Legacy Files(.vtk) (*.vtk)",
                utest_data::dir() / "sight/mesh/vtk/sphere.vtk"
            );

            save_snapshot(_tester, snapshot_show1_path);
            normalize_snapshot_to_reference(snapshot_show1_path, reference_show_path);
            compare_images(snapshot_show1_path, reference_show_path);

            helper::button::push(_tester, "list_organ_editor_srv/Hide all organs");

            save_snapshot(_tester, snapshot_hide_path);
            normalize_snapshot_to_reference(snapshot_hide_path, reference_hide_path);
            compare_images(snapshot_hide_path, reference_hide_path);

            helper::button::push(_tester, "list_organ_editor_srv/Hide all organs");

            save_snapshot(_tester, snapshot_show2_path);
            normalize_snapshot_to_reference(snapshot_show2_path, reference_show_path);
            compare_images(snapshot_show2_path, reference_show_path);
        },
        true
    );
}

} // namespace sight::sight_viewer::uit
