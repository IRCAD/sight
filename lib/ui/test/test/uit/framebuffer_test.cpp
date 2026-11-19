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

// cspell:ignore qgetenv qputenv qunsetenv

#include "../ut/gui_test_support.hpp"

#include <ui/test/detail/image.hpp>
#include <ui/test/qt_application.hpp>
#include <ui/test/scenario.hpp>

#include <core/os/temp_path.hpp>

#include <doctest/doctest.h>

#include <QColor>
#include <QMetaObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QScopeGuard>
#include <QtGlobal>

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace sight::ui::test::detail
{

namespace
{

class color_scene final : public QOpenGLWidget
{
public:

    QColor color {20, 40, 60};
    QColor next_color {160, 180, 200};
    int paints {0};
    int change_after {0};

protected:

    //------------------------------------------------------------------------------

    void paintGL() override
    {
        ++paints;
        if(change_after > 0 && paints >= change_after)
        {
            color = next_color;
        }

        auto* const gl = context()->functions();
        gl->glClearColor(color.redF(), color.greenF(), color.blueF(), 1.F);
        gl->glClear(GL_COLOR_BUFFER_BIT);
    }
};

struct render_fixture
{
    sight::ui::test::qt_application qt {"ui_test_uit", sight::ui::test::qt_platform::system};
    QApplication& application {qt.get()};
    gui_gateway gateway {application};
    core::os::temp_dir temporary;
    color_scene scene;

    render_fixture()
    {
        QApplication::setQuitOnLastWindowClosed(false);
        scene.setObjectName("scene");
        scene.resize(64, 48);
        scene.show();
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] static QImage solid(QSize _size, QColor _color)
    {
        QImage result(_size, QImage::Format_RGB32);
        result.fill(_color);
        return result;
    }
};

} // namespace

/// A negative control must have captured the scene, even if a loaded runner missed the first short deadline.
static std::string fail_after_capture(
    const std::function<void(std::chrono::milliseconds)>& _operation,
    QSize _actual_size
)
{
    const auto dimensions = std::format("actual {}x{}", _actual_size.width(), _actual_size.height());
    return conclusive_failure(
        _operation,
        [&](const std::string& _message){return _message.find(dimensions) != std::string::npos;});
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::ui::test::framebuffer")
{
    TEST_CASE("leaves_the_foreground_of_the_user_alone")
    {
#ifdef WIN32
        // Each window it opens and closes would otherwise bring the user's other applications to the front.
        const QString visible = qEnvironmentVariable("SIGHT_UI_TEST_VISIBLE");
        CHECK((qEnvironmentVariableIsSet("SIGHT_GUI_TEST_DESKTOP") || (!visible.isEmpty() && visible != "0")));
#endif
    }

    TEST_CASE_FIXTURE(render_fixture, "captures_the_fbo_even_when_another_window_covers_it")
    {
        scenario actions(application, temporary / "output");
        QWidget cover;
        cover.setStyleSheet("background: magenta;");
        run_scenario(
            *this,
            [&]
            {
                const auto before = actions.image("scene");
                REQUIRE(before == solid(before.size(), QColor(20, 40, 60)));
                gateway.call_until(
                    operation_deadline(),
                    [&]
                {
                    cover.resize(scene.size());
                    cover.move(scene.mapToGlobal(QPoint(0, 0)));
                    cover.show();
                    cover.raise();
                });
                const auto covered = actions.image("scene");
                CHECK(covered == before);
            });
        CHECK(scene.isValid());
    }

    TEST_CASE_FIXTURE(render_fixture, "retries_a_stable_wrong_frame_until_the_reference_is_rendered")
    {
        scenario actions(application, temporary / "output");
        const auto reference = temporary / "reference.png";
        run_scenario(
            *this,
            [&]
            {
                const auto initial = actions.image("scene");
                save_image(solid(initial.size(), QColor(160, 180, 200)), reference);
                gateway.call_until(operation_deadline(), [&]{scene.change_after = scene.paints + 6;});
                actions.expect_image("scene", reference, threshold(0, 0., "exact synthetic clear color"));
                CHECK(actions.image("scene") == solid(initial.size(), QColor(160, 180, 200)));
            });
        CHECK(scene.paints >= scene.change_after);
    }

    TEST_CASE_FIXTURE(render_fixture, "rejects_wrong_renders_and_sizes_and_writes_all_artifacts")
    {
        const auto output = temporary / "output";
        scenario actions(application, output);
        const auto reference = temporary / "reference.png";
        bool wrong_size      = false;
        SUBCASE("wrong color")
        {
        }
        SUBCASE("wrong dimensions")
        {
            wrong_size = true;
        }

        std::string failure;
        run_scenario(
            *this,
            [&]
            {
                const auto baseline = actions.image("scene");
                save_image(
                    solid(
                        wrong_size ? QSize(baseline.width() + 1, baseline.height()) : baseline.size(),
                        Qt::white
                    ),
                    reference
                );
                failure = fail_after_capture(
                    [&](std::chrono::milliseconds _timeout){actions.expect_image("scene", reference, {}, _timeout);},
                    baseline.size()
                );
            });
        CHECK(failure.find(wrong_size ? "different dimensions" : "ratio 1.000000") != std::string::npos);
        bool complete_artifacts = false;
        for(const auto& entry : std::filesystem::directory_iterator(output))
        {
            const auto& directory = entry.path();
            if(std::filesystem::exists(directory / "actual.png"))
            {
                CHECK(std::filesystem::exists(directory / "expected.png"));
                CHECK(std::filesystem::exists(directory / "diff.png"));
                CHECK(std::filesystem::file_size(directory / "metrics.txt") > 0);
                complete_artifacts = true;
            }
        }

        CHECK(complete_artifacts);
    }

    TEST_CASE_FIXTURE(render_fixture, "update_mode_saves_every_candidate_fails_and_never_replaces_the_reference")
    {
        const bool was_set   = qEnvironmentVariableIsSet("SIGHT_UI_TEST_UPDATE_REFERENCES");
        const QByteArray old = qgetenv("SIGHT_UI_TEST_UPDATE_REFERENCES");
        const auto restore   = qScopeGuard(
            [&]
            {
                const bool restored = was_set ? qputenv("SIGHT_UI_TEST_UPDATE_REFERENCES", old)
                                              : qunsetenv("SIGHT_UI_TEST_UPDATE_REFERENCES");
                CHECK(restored);
            });
        REQUIRE(qputenv("SIGHT_UI_TEST_UPDATE_REFERENCES", "1"));

        const auto output    = temporary / "output";
        const auto reference = temporary / "canonical.png";
        scenario actions(application, output);
        bool missing  = false;
        bool matching = false;
        SUBCASE("different reference")
        {
        }
        SUBCASE("matching reference")
        {
            matching = true;
        }
        SUBCASE("missing reference")
        {
            missing = true;
        }

        QImage canonical;
        std::filesystem::file_time_type modified;
        run_scenario(
            *this,
            [&]
            {
                const auto baseline = actions.image("scene");
                canonical           = matching ? baseline : solid(baseline.size(), Qt::white);
                if(!missing)
                {
                    save_image(canonical, reference);
                    modified = std::filesystem::last_write_time(reference);
                }

                // Every image of the scenario gets its candidate, then the scenario fails at its end.
                actions.expect_image("scene", reference);
                actions.expect_image("scene", reference);
                CHECK_THROWS_WITH_AS(
                    actions.finish(),
                    doctest::Contains("2 candidates saved, test intentionally failed"),
                    std::runtime_error
                );
                for(const auto* const image : {"image-0001", "image-0002"})
                {
                    CHECK(
                        QImage(QString::fromStdU16String((output / image / "candidate.png").u16string()))
                        == baseline
                    );
                }
            });
        CHECK(std::filesystem::exists(reference) == !missing);
        if(!missing)
        {
            CHECK(QImage(QString::fromStdU16String(reference.u16string())) == canonical);
            CHECK(std::filesystem::last_write_time(reference) == modified);
        }
    }

    TEST_CASE_FIXTURE(render_fixture, "waits_for_a_significant_change_and_invokes_the_action_once")
    {
        scenario actions(application, temporary / "output");
        int calls = 0;
        run_scenario(
            *this,
            [&]
            {
                actions.expect_image_changes(
                    "scene",
                    [&]
                {
                    ++calls;
                    gateway.call_until(operation_deadline(), [&]{scene.change_after = scene.paints + 6;});
                });
            });
        CHECK(calls == 1);
        CHECK(scene.color == scene.next_color);
    }

    TEST_CASE_FIXTURE(render_fixture, "unchanged_render_cannot_satisfy_the_relative_assertion")
    {
        scenario actions(application, temporary / "output");
        run_scenario(
            *this,
            [&]
            {
                const auto baseline = actions.image("scene");
                int calls           = 0;
                const auto failure  = fail_after_capture(
                    [&](std::chrono::milliseconds _timeout)
                {
                    // A short deadline may expire before the baseline, without invoking the action.
                    calls = 0;
                    actions.expect_image_changes(
                        "scene",
                        [&]
                    {
                        ++calls;
                        CHECK(calls == 1);
                    },
                        {},
                        _timeout
                    );
                },
                    baseline.size()
                );
                CHECK(failure.find("expected an image change") != std::string::npos);
                CHECK(failure.find("ratio 0.000000") != std::string::npos);
                CHECK(calls == 1);
            });
    }

    TEST_CASE_FIXTURE(render_fixture, "propagates_action_exceptions_without_replaying_the_action")
    {
        scenario actions(application, temporary / "output");
        int calls = 0;
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    actions.expect_image_changes(
                        "scene",
                        [&]
                {
                    ++calls;
                    throw std::runtime_error("action failed");
                }),
                    "action failed",
                    std::runtime_error
                );
            });
        CHECK(calls == 1);
    }

    TEST_CASE_FIXTURE(render_fixture, "a_resize_alone_cannot_satisfy_the_relative_assertion")
    {
        scenario actions(application, temporary / "output");
        run_scenario(
            *this,
            [&]
            {
                const auto initial_size = actions.image("scene").size();
                const QSize resized(initial_size.width() + 1, initial_size.height());
                int calls          = 0;
                const auto failure = fail_after_capture(
                    [&](std::chrono::milliseconds _timeout)
                {
                    // Count each attempt separately: baseline capture can exhaust a short deadline.
                    calls = 0;
                    gateway.call_until(operation_deadline(), [&]{scene.resize(initial_size);});
                    actions.expect_image_changes(
                        "scene",
                        [&]
                    {
                        ++calls;
                        CHECK(calls == 1);
                        gateway.call_until(operation_deadline(), [&]{scene.resize(resized);});
                    },
                        {},
                        _timeout
                    );
                },
                    resized
                );
                CHECK(failure.find("different dimensions") != std::string::npos);
                CHECK(failure.find("expected an image change") != std::string::npos);
                CHECK(calls == 1);
            });
    }

    TEST_CASE_FIXTURE(render_fixture, "the_time_of_the_action_is_not_taken_from_the_observation")
    {
        scenario actions(application, temporary / "output");
        run_scenario(
            *this,
            [&]
            {
                // The action outlasts the deadline each time, so a shared deadline can never observe its effect.
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout)
                {
                    actions.expect_image_changes(
                        "scene",
                        [&]
                    {
                        gateway.call_until(operation_deadline(), [&]{std::swap(scene.color, scene.next_color);});
                        std::this_thread::sleep_for(_timeout + std::chrono::milliseconds(50));
                    },
                        {},
                        _timeout
                    );
                },
                    [](const std::string&){return false;});
                CHECK(failure == "no timeout");
            });
    }

    TEST_CASE_FIXTURE(render_fixture, "an_expired_deadline_is_a_timeout_and_not_a_misuse")
    {
        const auto output    = temporary / "output";
        const auto reference = temporary / "reference.png";
        scenario actions(application, output);
        int calls = 0;
        run_scenario(
            *this,
            [&]
            {
                save_image(solid(actions.image("scene").size(), Qt::white), reference);
                const auto expired = std::chrono::nanoseconds(1);
                CHECK_THROWS_AS(actions.expect_image_changes("scene", [&]{++calls;}, {}, expired), std::runtime_error);
                CHECK_THROWS_AS(actions.expect_image("scene", reference, {}, expired), std::runtime_error);
                CHECK_THROWS_AS(static_cast<void>(actions.image("scene", expired)), std::runtime_error);
            });
        CHECK(calls == 0);
    }

    TEST_CASE_FIXTURE(render_fixture, "says_so_when_no_frame_could_be_measured")
    {
        const auto reference = temporary / "reference.png";
        scenario actions(application, temporary / "output");
        run_scenario(
            *this,
            [&]
            {
                save_image(solid(actions.image("scene").size(), Qt::white), reference);
                gateway.call_until(operation_deadline(), [&]{scene.hide();});
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout){actions.expect_image("scene", reference, {}, _timeout);},
                    [](const std::string& _message){return _message.find("no visible widget") != std::string::npos;});
                INFO(failure);
                CHECK(failure.find("no visible widget") != std::string::npos);
                CHECK(failure.find("-1x-1") == std::string::npos);
                CHECK(failure.find("dimensions") == std::string::npos);
            });
    }

    TEST_CASE_FIXTURE(render_fixture, "compares_the_frames_a_gui_busy_past_the_first_look_shows_afterwards")
    {
        const auto reference = temporary / "reference.png";
        scenario actions(application, temporary / "output");
        run_scenario(
            *this,
            [&]
            {
                save_image(actions.image("scene"), reference);

                // Scaled down, the first look waits 3 s for a GUI busy for 3.5 s: the frames compared come after.
                qputenv("SIGHT_UI_TEST_TIMEOUT_SCALE", "0.1");
                const auto unscale = qScopeGuard([]{qunsetenv("SIGHT_UI_TEST_TIMEOUT_SCALE");});
                QMetaObject::invokeMethod(
                    &application,
                    []{std::this_thread::sleep_for(std::chrono::milliseconds(3500));},
                    Qt::QueuedConnection
                );
                CHECK_NOTHROW(actions.expect_image("scene", reference, {}, std::chrono::milliseconds(100)));
            });
    }

    TEST_CASE_FIXTURE(render_fixture, "gives_the_scenario_its_output_directory")
    {
        const auto output = temporary / "output";
        scenario actions(application, output);
        run_scenario(*this, [&]{CHECK(actions.output_directory() == output);});
    }

    TEST_CASE_FIXTURE(render_fixture, "preserves_image_failure_metrics_when_artifact_writing_fails")
    {
        const auto blocked = temporary / "blocked";
        std::ofstream(blocked).put('x');
        scenario actions(application, blocked);
        const auto reference = temporary / "reference.png";
        run_scenario(
            *this,
            [&]
            {
                const auto baseline = actions.image("scene");
                save_image(solid(baseline.size(), Qt::white), reference);
                const auto failure = fail_after_capture(
                    [&](std::chrono::milliseconds _timeout){actions.expect_image("scene", reference, {}, _timeout);},
                    baseline.size()
                );
                CHECK(failure.find("ratio 1.000000") != std::string::npos);
                CHECK(failure.find("artifact error") != std::string::npos);
            });
        CHECK(std::filesystem::is_regular_file(blocked));
    }

    TEST_CASE_FIXTURE(render_fixture, "fails_explicitly_when_the_reference_cannot_be_read")
    {
        scenario actions(application, temporary / "output");
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    actions.expect_image("scene", temporary / "missing.png"),
                    doctest::Contains("cannot read image reference"),
                    std::invalid_argument
                );
            });
    }
}

} // namespace sight::ui::test::detail
