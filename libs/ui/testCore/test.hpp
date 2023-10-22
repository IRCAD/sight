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

#pragma once

#include "Tester.hpp"

#include "ui/testCore/config.hpp"

#include <core/runtime/profile/profile.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <QEvent>
#include <QObject>

namespace sight::ui::test_core
{

class GUITEST_CLASS_API test : public CPPUNIT_NS::TestFixture
{
public:

    GUITEST_API void setUp() override;

protected:

    GUITEST_API void start(
        const std::string& _test_name,
        std::function<void(Tester&)> _test,
        bool _verbose_mode = false
    );

    GUITEST_API virtual std::filesystem::path getProfilePath() = 0;

    GUITEST_API static void compareImages(const std::filesystem::path& _a, const std::filesystem::path& _b);

private:

    sight::core::runtime::profile::sptr m_profile;
};

} // namespace sight::ui::testCore
