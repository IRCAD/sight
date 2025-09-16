/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::com::signals")
{
//-----------------------------------------------------------------------------

    TEST_CASE("build")
    {
        sight::core::com::signals signals;

        auto sig = std::make_shared<sight::core::com::signal<void()> >();
        CHECK(sig);

        using signature = void (int, float);
        auto sig2 = std::make_shared<sight::core::com::signal<signature> >();
        CHECK(sig2);

        signals("sig", sig)
            ("sig2", sig2);

        CHECK(sig == signals["sig"]);
        CHECK(sig2 == signals["sig2"]);
        CHECK(!signals["wrong_signal"]);

        CHECK_EQ(signals.get_signal_keys().size(), 2);
    }

//-----------------------------------------------------------------------------

    struct signals_test_has_signals : public sight::core::com::has_signals
    {
        using signal_type = sight::core::com::signal<void ()>;

        signals_test_has_signals()
        {
            auto signal = std::make_shared<signal_type>();
            sight::core::com::has_signals::m_signals("sig", signal);
        }
    };

    struct signals_test_has_signals2 : public sight::core::com::has_signals
    {
        using signal_type = sight::core::com::signal<void (int, double)>;

        signals_test_has_signals2()
        {
            auto sig = this->new_signal<signal_type>("sig");
            CHECK(sig);
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

    TEST_CASE("has_signals")
    {
        SUBCASE("signals_test_has_signals")
        {
            signals_test_has_signals obj;
            signal_test_a srv;
            auto slot = sight::core::com::new_slot(&signal_test_a::change_status, &srv);
            obj.signal("sig")->connect(slot);
            obj.signal<signals_test_has_signals::signal_type>("sig")->emit();
            CHECK(srv.m_val);
            obj.signal("sig")->disconnect(slot);
        }

        SUBCASE("signals_test_has_signals2")
        {
            signals_test_has_signals2 obj;
            signal_test_a srv;
            auto slot = sight::core::com::new_slot(&signal_test_a::change_values, &srv);

            obj.signal("sig")->connect(slot);
            obj.signal<signals_test_has_signals2::signal_type>("sig")->emit(4, 5.);

            CHECK_EQ(srv.m_int, 4);
            CHECK_EQ(srv.m_double, 5.);

            obj.emit("sig", -3, 22.3);

            CHECK_EQ(srv.m_int, -3);
            CHECK_EQ(srv.m_double, 22.3);

            obj.signal("sig")->disconnect(slot);
        }
    }

//-----------------------------------------------------------------------------
}
