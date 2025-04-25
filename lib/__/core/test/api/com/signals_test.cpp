/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "signals_test.hpp"

#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::com::ut::signals_test);

namespace sight::core::com::ut
{

//------------------------------------------------------------------------------

void signals_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void signals_test::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void signals_test::build_test()
{
    core::com::signals signals;

    core::com::signal<void()>::sptr sig = std::make_shared<core::com::signal<void()> >();
    CPPUNIT_ASSERT(sig);

    using signature = void (int, float);
    core::com::signal<signature>::sptr sig2 = std::make_shared<core::com::signal<signature> >();
    CPPUNIT_ASSERT(sig2);

    signals("sig", sig)
        ("sig2", sig2);

    CPPUNIT_ASSERT(sig == signals["sig"]);
    CPPUNIT_ASSERT(sig2 == signals["sig2"]);
    CPPUNIT_ASSERT(!signals["wrong_signal"]);

    CPPUNIT_ASSERT_EQUAL((std::size_t) 2, signals.get_signal_keys().size());
}

//-----------------------------------------------------------------------------

struct signals_test_has_signals : public has_signals
{
    using signal_type = core::com::signal<void ()>;

    signals_test_has_signals()
    {
        auto signal = std::make_shared<signal_type>();
        has_signals::m_signals("sig", signal);
    }
};

struct signals_test_has_signals2 : public has_signals
{
    using signal_type = core::com::signal<void (int, double)>;

    signals_test_has_signals2()
    {
        auto sig = this->new_signal<signal_type>("sig");
        CPPUNIT_ASSERT(sig);
    }
};

struct signal_test_a
{
    signal_test_a() = default;

    //------------------------------------------------------------------------------

    void change_status()
    {
        m_val = !m_val;
    }

    //------------------------------------------------------------------------------

    void change_values(int _int, double _double)
    {
        m_int    = _int;
        m_double = _double;
    }

    bool m_val {false};
    int m_int {0};
    double m_double {0.};
};
//-----------------------------------------------------------------------------

void signals_test::has_signals_test()
{
    {
        signals_test_has_signals obj;
        signal_test_a srv;
        auto slot = core::com::new_slot(&signal_test_a::change_status, &srv);
        obj.signal("sig")->connect(slot);
        obj.signal<signals_test_has_signals::signal_type>("sig")->emit();
        CPPUNIT_ASSERT(srv.m_val);
        obj.signal("sig")->disconnect(slot);
    }
    {
        signals_test_has_signals2 obj;
        signal_test_a srv;
        auto slot = core::com::new_slot(&signal_test_a::change_values, &srv);

        obj.signal("sig")->connect(slot);
        obj.signal<signals_test_has_signals2::signal_type>("sig")->emit(4, 5.);

        CPPUNIT_ASSERT_EQUAL(4, srv.m_int);
        CPPUNIT_ASSERT_EQUAL(5., srv.m_double);

        obj.emit("sig", -3, 22.3);

        CPPUNIT_ASSERT_EQUAL(-3, srv.m_int);
        CPPUNIT_ASSERT_EQUAL(22.3, srv.m_double);

        obj.signal("sig")->disconnect(slot);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com::ut
