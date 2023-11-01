/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "job_bar_test.hpp"

#include <core/com/slot_base.hxx>
#include <core/jobs/base.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/progress_base.hpp>
#include <ui/__/macros.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::ut::job_bar_test);

namespace sight::module::ui::ut
{

class dummy_progress_dialog : public sight::ui::dialog::progress_base
{
public:

    dummy_progress_dialog()
    {
        s_last_dialog = this;
    }

    ~dummy_progress_dialog() override
    {
        if(s_last_dialog == this)
        {
            s_last_dialog = nullptr;
        }
    }

    //------------------------------------------------------------------------------

    void set_title(const std::string& _title) override
    {
        m_title = _title;
    }

    //------------------------------------------------------------------------------

    void set_message(const std::string& _message) override
    {
        m_message = _message;
    }

    //------------------------------------------------------------------------------

    void operator()(float _percentage, std::string _message) override
    {
        m_percentage = _percentage;
        m_message    = _message;
    }

    //------------------------------------------------------------------------------

    std::string get_title()
    {
        return m_title;
    }

    //------------------------------------------------------------------------------

    std::string get_message()
    {
        return m_message;
    }

    //------------------------------------------------------------------------------

    float get_percentage() const
    {
        return m_percentage;
    }

    static dummy_progress_dialog* s_last_dialog;

private:

    std::string m_title;
    std::string m_message;
    float m_percentage {};
};

dummy_progress_dialog* dummy_progress_dialog::s_last_dialog = nullptr;

SIGHT_REGISTER_GUI(dummy_progress_dialog, sight::ui::dialog::progress_base::REGISTRY_KEY);

class dummy_job : public core::jobs::base
{
public:

    explicit dummy_job(const std::string& _name, std::uint64_t _total_work_unit = 100) :
        base(_name)
    {
        m_total_work_units = _total_work_unit;
    }

    //------------------------------------------------------------------------------

    std::shared_future<void> run_impl() override
    {
        return {};
    }

    using core::jobs::base::done_work;
};

//------------------------------------------------------------------------------

void job_bar_test::setUp()
{
    m_job_bar = service::add("sight::module::ui::job_bar");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::job_bar'", m_job_bar);
}

//------------------------------------------------------------------------------

void job_bar_test::tearDown()
{
    if(!m_job_bar->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_job_bar->stop().get());
    }

    service::remove(m_job_bar);
}

//------------------------------------------------------------------------------

void job_bar_test::basic_test()
{
    using namespace std::literals::string_literals;

    static const std::string s_JOB_NAME = "Your Dream Job";

    CPPUNIT_ASSERT_NO_THROW(m_job_bar->configure());
    CPPUNIT_ASSERT_NO_THROW(m_job_bar->start().get());

    auto job = std::make_shared<dummy_job>(s_JOB_NAME);
    m_job_bar->slot("show_job")->run(std::static_pointer_cast<core::jobs::base>(job));

    CPPUNIT_ASSERT_EQUAL(s_JOB_NAME, dummy_progress_dialog::s_last_dialog->get_title());
    CPPUNIT_ASSERT_EQUAL(""s, dummy_progress_dialog::s_last_dialog->get_message());

    for(int i = 1 ; i <= 100 ; i++)
    {
        std::string log = "Doing the thing " + std::to_string(i);
        job->log(log);
        job->done_work(std::uint64_t(i));
        CPPUNIT_ASSERT_EQUAL(log, dummy_progress_dialog::s_last_dialog->get_message());
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(i / 100.), dummy_progress_dialog::s_last_dialog->get_percentage());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::ut
