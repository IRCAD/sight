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

#include "test.hpp"

#include "tester.hpp"

#include <core/runtime/profile/profile.hpp>
#include <core/runtime/runtime.hpp>

namespace sight::ui::test
{

//------------------------------------------------------------------------------

base::base(const std::filesystem::path& _profile_path)
{
    sight::core::runtime::init();

    //load the profiles' project modules
    const auto profile_module_path = _profile_path.parent_path().parent_path();
    if(std::filesystem::exists(profile_module_path) && std::filesystem::is_directory(profile_module_path))
    {
        sight::core::runtime::add_modules(profile_module_path);
    }
    else
    {
        SIGHT_ERROR("Module path " << profile_module_path << " does not exists or is not a directory.");
    }

    m_profile = sight::core::runtime::io::profile_reader::create_profile(_profile_path);
    m_profile->start();
    sight::ui::test::tester::init();
}

//------------------------------------------------------------------------------

base::~base() = default;

//------------------------------------------------------------------------------

std::string base::start(const std::string& _test_name, std::function<void(tester&)> _test, bool _verbose_mode)
{
    tester tester(_test_name, _verbose_mode);
    tester.start([&tester, _test]{_test(tester);});
    m_profile->run();
    m_profile->stop();

    // By the time m_profile->stop() returns, tester::start() has joined the scenario thread and
    // application::exit() has been processed: the failure state is stable and safe to read here,
    // on the main thread. This function must stay free of doctest usage - see the class-level
    // note on sight::ui::test::base in test.hpp.
    return tester.failed() ? tester.get_failure_message() : std::string();
}

//------------------------------------------------------------------------------

void base::compare_images(const std::filesystem::path& _a, const std::filesystem::path& _b)
{
    const QImage ia(QString::fromStdString(_a.string()));
    const QImage ib(QString::fromStdString(_b.string()));
    const double mse          = tester::compare_images_mse(ia, ib);
    const double histogram    = tester::compare_images_histogram(ia, ib);
    const double correlation  = tester::compare_images_correlation(ia, ib);
    const double voodoo       = tester::compare_images_voodoo(ia, ib);
    const std::string message = std::string("The generated image (" + _a.string() + ") and the reference image (")
                                + _b.string() + ") aren't identical";
    const std::string score = std::string("MSE: ") + std::to_string(mse) + "\nHistogram: " + std::to_string(histogram)
                              + "\nCorrelation: " + std::to_string(correlation) + "\nVoodoo: "
                              + std::to_string(voodoo)
                              + '\n';

    // tester::fail() throws tester_assertion_failed, which tester::start() catches on the scenario
    // thread: it takes a failure screenshot and composes the GIVEN/WHEN/THEN backtrace. A doctest
    // assertion must not be used here, since this can be called from the scenario thread.
    if(mse <= 0.96)
    {
        tester::fail(message + " (MSE)\n" + score);
    }

    if(histogram <= 0.95)
    {
        tester::fail(message + " (histogram)\n" + score);
    }

    if(correlation <= 0.69)
    {
        tester::fail(message + " (Correlation)\n" + score);
    }

    if(voodoo <= 0.96)
    {
        tester::fail(message + " (Voodoo)\n" + score);
    }
}

} // namespace sight::ui::test
