/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/thread/ActiveWorkers.hpp"

#include "core/util/LazyInstantiator.hpp"

namespace sight::core::thread
{

//-----------------------------------------------------------------------------

const std::string ActiveWorkers::s_DEFAULT_WORKER = "DEFAULT_WORKER";

//-----------------------------------------------------------------------------

ActiveWorkers::ActiveWorkers()
{
}

//-----------------------------------------------------------------------------

ActiveWorkers::~ActiveWorkers()
{
}

//-----------------------------------------------------------------------------

ActiveWorkers::sptr ActiveWorkers::getDefault()
{
    return core::util::LazyInstantiator< ActiveWorkers >::getInstance();
}

//-----------------------------------------------------------------------------

core::thread::Worker::sptr ActiveWorkers::getWorker( const WorkerKeyType& key ) const
{
    core::mt::ReadLock lock(m_registryMutex);

    WorkerMapType::const_iterator it = m_workers.find(key);

    if( it != m_workers.end() )
    {
        return it->second;
    }

    return core::thread::Worker::sptr();
}

//-----------------------------------------------------------------------------

void ActiveWorkers::setDefaultWorker(core::thread::Worker::sptr worker)
{
    getDefault()->addWorker(s_DEFAULT_WORKER, worker);
}

//-----------------------------------------------------------------------------

core::thread::Worker::sptr ActiveWorkers::getDefaultWorker()
{
    return ActiveWorkers::getDefault()->getWorker( s_DEFAULT_WORKER );
}

//-----------------------------------------------------------------------------

void ActiveWorkers::addWorker( const WorkerKeyType& key, core::thread::Worker::sptr worker )
{
    core::mt::WriteLock lock(m_registryMutex);
    m_workers.insert( WorkerMapType::value_type(key, worker) );
}

//-----------------------------------------------------------------------------

void ActiveWorkers::initRegistry()
{
    this->addWorker( s_DEFAULT_WORKER, core::thread::Worker::New() );
}

//-----------------------------------------------------------------------------

void ActiveWorkers::clearRegistry()
{
    core::mt::WriteLock lock(m_registryMutex);

    for (const auto& elt: m_workers)
    {
        elt.second->stop();
    }
    m_workers.clear();
}

//-----------------------------------------------------------------------------

} // namespace sight::core::thread
