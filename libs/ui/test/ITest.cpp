/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "ITest.hpp"

#include "Tester.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

namespace sight::ui::test
{

//------------------------------------------------------------------------------

void ITest::setUp()
{
    sight::core::runtime::init();
    const std::filesystem::path cwd  = sight::core::runtime::getWorkingPath();
    const std::filesystem::path path = cwd / getProfilePath();
    m_profile = sight::core::runtime::io::ProfileReader::createProfile(path);
    m_profile->start();
    sight::ui::test::Tester::init();
}

//------------------------------------------------------------------------------

void ITest::start(const std::string& testName, std::function<void(Tester&)> test, bool verboseMode)
{
    Tester tester(testName, verboseMode);
    tester.start([&tester, test]{test(tester);});
    m_profile->run();
    m_profile->stop();
    tester.end();
    CPPUNIT_ASSERT_MESSAGE(tester.getFailureMessage(), !tester.failed());
}

//------------------------------------------------------------------------------

void ITest::compareImages(const std::filesystem::path& a, const std::filesystem::path& b)
{
    const QImage ia(QString::fromStdString(a.string()));
    const QImage ib(QString::fromStdString(b.string()));
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
    CPPUNIT_ASSERT_MESSAGE(message + " (Histogram)\n" + score, histogram > 0.98);
    CPPUNIT_ASSERT_MESSAGE(message + " (Correlation)\n" + score, correlation > 0.71);
    CPPUNIT_ASSERT_MESSAGE(message + " (Voodoo)\n" + score, voodoo > 0.96);
}

} // namespace sight::ui::test
