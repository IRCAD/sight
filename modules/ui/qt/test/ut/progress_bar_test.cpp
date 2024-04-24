/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "progress_bar_test.hpp"

#include "loader.hpp"

#include <core/com/slot_base.hxx>
#include <core/jobs/base.hpp>

#include <service/op.hpp>

#include <ui/__/macros.hpp>

#include <QLabel>
#include <QProgressBar>
#include <QSvgWidget>
#include <QToolButton>
#include <QWidget>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::progress_bar_test);

namespace sight::module::ui::qt::ut
{

class dummy_job : public core::jobs::base
{
public:

    explicit dummy_job(const std::string& _name, std::uint64_t _total_work_unit = 100) :
        base(_name)
    {
        m_total_work_units = _total_work_unit;
        m_state            = sight::core::jobs::base::state::running;
    }

    ~dummy_job() override
    {
        m_state = sight::core::jobs::base::state::finished;
    }

    //------------------------------------------------------------------------------

    std::shared_future<void> run_impl() override
    {
        return {};
    }

    using core::jobs::base::done_work;
};

//------------------------------------------------------------------------------

void progress_bar_test::setUp()
{
    // Build container.
    std::tie(m_container, m_child_uuid) = make_container();
}

//------------------------------------------------------------------------------

void progress_bar_test::tearDown()
{
    // Destroy container.
    destroy_container(m_container);
}

//------------------------------------------------------------------------------

void progress_bar_test::basic_test()
{
    // Title and cancel button are shown.
    launch_test(true, true, false);

    // Destroy the container and recreate it.
    tearDown();
    setUp();

    // Cancel button is shown.
    launch_test(false, true, false);

    tearDown();
    setUp();

    //Title is shown.
    launch_test(true, false, false);
}

//------------------------------------------------------------------------------

void progress_bar_test::pulse_test()
{
    // Display a pulse progress bar.
    launch_test(true, true, true);
}

//------------------------------------------------------------------------------

void progress_bar_test::svg_test()
{
    // Display a pulse waiting icon.
    launch_test(true, true, true, "sight::module::ui::icons/wait.svg");
}

//------------------------------------------------------------------------------

void progress_bar_test::launch_test(bool _show_title, bool _show_cancel, bool _pulse, const std::string& _svg)
{
    // Build configuration
    service::config_t config;
    config.put("<xmlattr>.show_title", _show_title);
    config.put("<xmlattr>.show_cancel", _show_cancel);
    config.put("<xmlattr>.pulse", _pulse);

    if(!_svg.empty())
    {
        config.put("<xmlattr>.svg", _svg);
        config.put("<xmlattr>.svg_size", "48");
    }

    config.add_child("config", config);

    // Register the service.
    sight::service::base::sptr progress_bar(
        service::add("sight::module::ui::qt::progress_bar", m_child_uuid)
    );

    // Will stop the service and unregister it when destroyed.
    service_cleaner cleaner(progress_bar);

    CPPUNIT_ASSERT_NO_THROW(progress_bar->configure(config));
    CPPUNIT_ASSERT_NO_THROW(progress_bar->start().get());

    // Check that progress_bar is not visible before show_job().
    auto check_visibility = wait_for_widget(
        [_show_title, _show_cancel, _svg, this](QWidget* _widget)
        {
            if(_widget != nullptr && _widget->objectName().startsWith(QString::fromStdString(m_child_uuid)))
            {
                auto check_progress_widget = false;

                if(_svg.empty())
                {
                    if(auto* progress_bar_widget = _widget->findChild<QProgressBar*>("/QProgressBar");
                       progress_bar_widget != nullptr)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "The progress_bar widget should not be visible before show_job().",
                            false,
                            progress_bar_widget->isVisible()
                        );
                        check_progress_widget = true;
                    }
                }
                else
                {
                    if(auto* svg_widget = _widget->findChild<QSvgWidget*>("/QSvgWidget");
                       svg_widget != nullptr)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "The progress_bar widget should not be visible before show_job().",
                            false,
                            svg_widget->isVisible()
                        );
                        check_progress_widget = true;
                    }
                }

                auto check_label = !_show_title;
                if(auto* label = _widget->findChild<QLabel*>("/QLabel");
                   label != nullptr)
                {
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(
                        "The label should not be visible before show_job().",
                        false,
                        label->isVisible()
                    );
                    check_label = true;
                }

                auto check_button = !_show_cancel;
                if(auto* button = _widget->findChild<QToolButton*>("/QToolButton");
                   button != nullptr)
                {
                    CPPUNIT_ASSERT_EQUAL_MESSAGE(
                        "The cancel button should not be visible before show_job().",
                        false,
                        button->isVisible()
                    );
                    check_button = true;
                }

                return check_progress_widget && check_label && check_button;
            }

            return false;
        });

    // Wait for the script thread to finish.
    CPPUNIT_ASSERT_EQUAL(
        std::future_status::ready,
        check_visibility.wait_for(std::chrono::seconds(5))
    );

    // Should be true.
    CPPUNIT_ASSERT(check_visibility.get());

    // Create job and slot.
    static const std::string s_JOB_NAME = "Your Dream Job";
    static int job_count                = 0;
    const std::string job_name          = s_JOB_NAME + std::to_string(job_count++);
    auto job                            = std::make_shared<dummy_job>(job_name);
    progress_bar->slot("show_job")->run(std::static_pointer_cast<core::jobs::base>(job));

    // Check that progress_bar is setted with correct information.
    for(int i = 1 ; i <= 100 ; i++)
    {
        job->done_work(std::uint64_t(i));

        auto check_progress_info = wait_for_widget(
            [_show_title, _pulse, _svg, i, job, job_name, this](QWidget* _widget)
            {
                if(_widget != nullptr && _widget->objectName().startsWith(QString::fromStdString(m_child_uuid)))
                {
                    auto correct_title = !_show_title;
                    if(auto* label = _widget->findChild<QLabel*>("/QLabel"); label != nullptr)
                    {
                        CPPUNIT_ASSERT_EQUAL_MESSAGE(
                            "The title of progress_bar should be equal to job name.",
                            job_name + " - ",
                            label->text().toStdString()
                        );

                        correct_title = true;
                    }

                    auto correct_done_work_units = false;

                    if(_svg.empty())
                    {
                        if(auto* progress_bar_widget = _widget->findChild<QProgressBar*>("/QProgressBar");
                           progress_bar_widget != nullptr)
                        {
                            // In pulse mode, the value is irrelevant
                            if(!_pulse)
                            {
                                // Do the same operation that the progress_bar does.
                                int value = (int) (float(i) / float(job->get_total_work_units()) * 100);
                                CPPUNIT_ASSERT_EQUAL_MESSAGE(
                                    "The value of progress_bar should be equal to done work units.",
                                    value,
                                    progress_bar_widget->value()
                                );
                            }

                            correct_done_work_units = true;
                        }
                    }
                    else
                    {
                        if(auto* progress_bar_widget = _widget->findChild<QSvgWidget*>("/QSvgWidget");
                           progress_bar_widget != nullptr)
                        {
                            // In pulse mode, the value is irrelevant
                            correct_done_work_units = true;
                        }
                    }

                    return correct_title && correct_done_work_units;
                }

                return false;
            });

        // Wait for the script thread to finish.
        CPPUNIT_ASSERT_EQUAL(
            std::future_status::ready,
            check_progress_info.wait_for(std::chrono::seconds(10))
        );

        // Should be true.
        CPPUNIT_ASSERT(check_progress_info.get());
    }

    // Cleanup
    CPPUNIT_ASSERT_NO_THROW(progress_bar->stop().get());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
