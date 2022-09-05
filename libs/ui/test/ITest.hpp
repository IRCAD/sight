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

#pragma once

#include "Tester.hpp"

#include "ui/test/config.hpp"

#include <core/runtime/profile/Profile.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <QEvent>
#include <QObject>

namespace sight::ui::test
{

class GUITEST_CLASS_API ITest : public CPPUNIT_NS::TestFixture
{
public:

    GUITEST_API void setUp() override;

protected:

    GUITEST_API void start(const std::string& testName, std::function<void(Tester&)> test, bool verboseMode = false);

    GUITEST_API virtual const char* getProfilePath() = 0;

    GUITEST_API static void compareImages(const std::filesystem::path& a, const std::filesystem::path& b);

private:

    sight::core::runtime::Profile::sptr m_profile;
};

} // namespace sight::ui::test
