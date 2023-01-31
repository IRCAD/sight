/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SJobBarTest.hpp"

#include <core/com/SlotBase.hxx>
#include <core/jobs/IJob.hpp>

#include <service/base.hpp>

#include <ui/base/dialog/IProgressDialog.hpp>
#include <ui/base/registry/macros.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::ut::SJobBarTest);

namespace sight::module::ui::base::ut
{

class DummyProgressDialog : public sight::ui::base::dialog::IProgressDialog
{
public:

    explicit DummyProgressDialog(sight::ui::base::factory::Key /*key*/)
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

SIGHT_REGISTER_GUI(DummyProgressDialog, sight::ui::base::dialog::IProgressDialog::REGISTRY_KEY);

class DummyJob : public core::jobs::IJob
{
public:

    explicit DummyJob(const std::string& name, std::uint64_t totalWorkUnit = 100) :
        IJob(name)
    {
        m_totalWorkUnits = totalWorkUnit;
    }

    //------------------------------------------------------------------------------

    std::shared_future<void> runImpl() override
    {
        return {};
    }

    using core::jobs::IJob::doneWork;
};

//------------------------------------------------------------------------------

void SJobBarTest::setUp()
{
    m_jobBar = service::add("sight::module::ui::base::SJobBar");
    CPPUNIT_ASSERT_MESSAGE("Failed to create service 'sight::module::ui::base::SJobBar'", m_jobBar);
}

//------------------------------------------------------------------------------

void SJobBarTest::tearDown()
{
    if(!m_jobBar->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_jobBar->stop().get());
    }

    service::remove(m_jobBar);
}

//------------------------------------------------------------------------------

void SJobBarTest::basicTest()
{
    using namespace std::literals::string_literals;

    static const std::string JOB_NAME = "Your Dream Job";

    CPPUNIT_ASSERT_NO_THROW(m_jobBar->configure());
    CPPUNIT_ASSERT_NO_THROW(m_jobBar->start().get());

    auto job = std::make_shared<DummyJob>(JOB_NAME);
    m_jobBar->slot("showJob")->run(std::static_pointer_cast<core::jobs::IJob>(job));

    CPPUNIT_ASSERT_EQUAL(JOB_NAME, DummyProgressDialog::lastDialog->getTitle());
    CPPUNIT_ASSERT_EQUAL(""s, DummyProgressDialog::lastDialog->getMessage());

    for(int i = 1 ; i <= 100 ; i++)
    {
        std::string log = "Doing the thing " + std::to_string(i);
        job->log(log);
        job->doneWork(std::uint64_t(i));
        CPPUNIT_ASSERT_EQUAL(log, DummyProgressDialog::lastDialog->getMessage());
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(i / 100.), DummyProgressDialog::lastDialog->getPercentage());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::ut
