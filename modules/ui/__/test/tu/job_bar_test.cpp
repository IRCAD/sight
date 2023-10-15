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

class DummyProgressDialog : public sight::ui::dialog::progress_base
{
public:

    DummyProgressDialog()
    {
        lastDialog = this;
    }

    ~DummyProgressDialog() override
    {
        if(lastDialog == this)
        {
            lastDialog = nullptr;
        }
    }

    //------------------------------------------------------------------------------

    void setTitle(const std::string& title) override
    {
        m_title = title;
    }

    //------------------------------------------------------------------------------

    void setMessage(const std::string& message) override
    {
        m_message = message;
    }

    //------------------------------------------------------------------------------

    void operator()(float percentage, std::string message) override
    {
        m_percentage = percentage;
        m_message    = message;
    }

    //------------------------------------------------------------------------------

    std::string getTitle()
    {
        return m_title;
    }

    //------------------------------------------------------------------------------

    std::string getMessage()
    {
        return m_message;
    }

    //------------------------------------------------------------------------------

    float getPercentage() const
    {
        return m_percentage;
    }

    static DummyProgressDialog* lastDialog;

private:

    std::string m_title;
    std::string m_message;
    float m_percentage {};
};

DummyProgressDialog* DummyProgressDialog::lastDialog = nullptr;

SIGHT_REGISTER_GUI(DummyProgressDialog, sight::ui::dialog::progress_base::REGISTRY_KEY);

class DummyJob : public core::jobs::base
{
public:

    explicit DummyJob(const std::string& name, std::uint64_t totalWorkUnit = 100) :
        base(name)
    {
        m_total_work_units = totalWorkUnit;
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
    m_jobBar = service::add("sight::module::ui::job_bar");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::job_bar'", m_jobBar);
}

//------------------------------------------------------------------------------

void job_bar_test::tearDown()
{
    if(!m_jobBar->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_jobBar->stop().get());
    }

    service::remove(m_jobBar);
}

//------------------------------------------------------------------------------

void job_bar_test::basicTest()
{
    using namespace std::literals::string_literals;

    static const std::string JOB_NAME = "Your Dream Job";

    CPPUNIT_ASSERT_NO_THROW(m_jobBar->configure());
    CPPUNIT_ASSERT_NO_THROW(m_jobBar->start().get());

    auto job = std::make_shared<DummyJob>(JOB_NAME);
    m_jobBar->slot("showJob")->run(std::static_pointer_cast<core::jobs::base>(job));

    CPPUNIT_ASSERT_EQUAL(JOB_NAME, DummyProgressDialog::lastDialog->getTitle());
    CPPUNIT_ASSERT_EQUAL(""s, DummyProgressDialog::lastDialog->getMessage());

    for(int i = 1 ; i <= 100 ; i++)
    {
        std::string log = "Doing the thing " + std::to_string(i);
        job->log(log);
        job->done_work(std::uint64_t(i));
        CPPUNIT_ASSERT_EQUAL(log, DummyProgressDialog::lastDialog->getMessage());
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(i / 100.), DummyProgressDialog::lastDialog->getPercentage());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::ut
