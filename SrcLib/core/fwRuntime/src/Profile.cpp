/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwRuntime/impl/profile/Profile.hpp"
#include "fwRuntime/Profile.hpp"

#include "fwRuntime/Bundle.hpp"
#include "fwRuntime/Extension.hpp"
#include "fwRuntime/impl/profile/Activater.hpp"
#include "fwRuntime/impl/profile/Initializer.hpp"
#include "fwRuntime/impl/profile/Starter.hpp"
#include "fwRuntime/impl/profile/Stopper.hpp"
#include "fwRuntime/impl/profile/Uninitializer.hpp"
#include "fwRuntime/Runtime.hpp"

#include <algorithm>
#include <cstring>
#include <functional>

namespace fwRuntime
{

namespace
{
template< typename E >
struct Apply
{
    //------------------------------------------------------------------------------

    void operator() ( E e )
    {
        e->apply();
    }
};
}

//------------------------------------------------------------------------------

Profile::sptr Profile::New()
{
    return std::make_shared< impl::profile::Profile >();
}

//------------------------------------------------------------------------------

Profile::Profile() :
    m_checkSingleInstance(false),
    m_argc(0),
    m_argv(nullptr)
{
}

//------------------------------------------------------------------------------

Profile::~Profile()
{
    if (m_argv)
    {
        delete[] m_argv;
    }
}

//------------------------------------------------------------------------------

void Profile::setParams(int argc, char** argv)
{
    ParamsContainer params;
    for(int i = 0; i < argc; ++i)
    {
        params.push_back( std::string(argv[i]) );
    }
    this->setParams(params);
}

//------------------------------------------------------------------------------

void Profile::setParams(const Profile::ParamsContainer& params)
{
    m_params = params;

    if (m_argv)
    {
        delete[] m_argv;
    }

    m_argc = static_cast<int>(m_params.size());
    // allocate memory for an array of character strings
    m_argv = new char*[m_params.size()];

    // for each string, allocate memory in the character array and copy
    for(size_t i = 0; i < m_params.size(); i++)
    {
        size_t paramSize = m_params[i].size();
        m_argv[i] = new char[paramSize+1];
#ifndef _WIN32
        strncpy(m_argv[i], m_params[i].c_str(), paramSize);
        m_argv[i][paramSize] = '\0';
#else
        strncpy_s(m_argv[i], paramSize+1, m_params[i].c_str(), paramSize);
#endif
    }
}

//------------------------------------------------------------------------------

void setCurrentProfile(Profile::sptr prof)
{
    impl::profile::setCurrentProfile(std::dynamic_pointer_cast<impl::profile::Profile>(prof));
}

//------------------------------------------------------------------------------

Profile::sptr getCurrentProfile()
{
    return impl::profile::getCurrentProfile();
}

//------------------------------------------------------------------------------

} // namespace fwRuntime
