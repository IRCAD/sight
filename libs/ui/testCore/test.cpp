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

#include "test.hpp"

#include "Tester.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

namespace sight::ui::test_core
{

//------------------------------------------------------------------------------

void test::setUp()
{
    sight::core::runtime::init();

    const auto profile_file_path = getProfilePath();

    //load the profiles' project modules
    const auto profile_module_path = profile_file_path.parent_path().parent_path();
    if(std::filesystem::exists(profile_module_path) && std::filesystem::is_directory(profile_module_path))
    {
        sight::core::runtime::add_modules(profile_module_path);
    }
    else
    {
        SIGHT_ERROR("Module path " << profile_module_path << " does not exists or is not a directory.");
    }

    m_profile = sight::core::runtime::io::profile_reader::create_profile(profile_file_path);
    m_profile->start();
    sight::ui::test_core::Tester::init();
}

//------------------------------------------------------------------------------

void test::start(const std::string& _test_name, std::function<void(Tester&)> _test, bool _verbose_mode)
{
    Tester tester(_test_name, _verbose_mode);
    tester.start([&tester, _test]{_test(tester);});
    m_profile->run();
    m_profile->stop();
    CPPUNIT_ASSERT_MESSAGE(tester.getFailureMessage(), !tester.failed());
}

//------------------------------------------------------------------------------

void test::compareImages(const std::filesystem::path& _a, const std::filesystem::path& _b)
{
    const QImage ia(QString::fromStdString(_a.string()));
    const QImage ib(QString::fromStdString(_b.string()));
    const double mse          = Tester::compareImagesMSE(ia, ib);
    const double histogram    = Tester::compareImagesHistogram(ia, ib);
    const double correlation  = Tester::compareImagesCorrelation(ia, ib);
    const double voodoo       = Tester::compareImagesVoodoo(ia, ib);
    const std::string message = "The generated image and the reference image aren't identical";
    const std::string score   = std::string("MSE: ") + std::to_string(mse) + "\nHistogram: " + std::to_string(histogram)
                                + "\nCorrelation: " + std::to_string(correlation) + "\nVoodoo: "
                                + std::to_string(voodoo)
                                + '\n';
    CPPUNIT_ASSERT_MESSAGE(message + " (MSE)\n" + score, mse > 0.96);
    CPPUNIT_ASSERT_MESSAGE(message + " (histogram)\n" + score, histogram > 0.98);
    CPPUNIT_ASSERT_MESSAGE(message + " (Correlation)\n" + score, correlation > 0.69);
    CPPUNIT_ASSERT_MESSAGE(message + " (Voodoo)\n" + score, voodoo > 0.96);
}

} // namespace sight::ui::test_core
