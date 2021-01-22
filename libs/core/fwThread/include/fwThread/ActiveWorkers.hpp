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

#pragma once

#include "fwThread/config.hpp"
#include <fwThread/Worker.hpp>

#include <core/mt/types.hpp>

namespace fwThread
{
/**
 * @brief This class to register active worker in the system, creates a default worker
 */
class FWTHREAD_CLASS_API ActiveWorkers : public core::BaseObject
{

public:

    fwCoreClassMacro(ActiveWorkers, core::BaseObject, std::make_shared< ActiveWorkers >)

    typedef std::string WorkerKeyType;

    typedef std::map< WorkerKeyType, ::fwThread::Worker::sptr > WorkerMapType;

    /// Key of default worker in registry, created and registered by initRegistry method
    FWTHREAD_API static const std::string s_DEFAULT_WORKER;

    /// Constructor, does nothing
    FWTHREAD_API ActiveWorkers();

    /// Destructor, does nothing
    FWTHREAD_API virtual ~ActiveWorkers();

    /**
     * @brief Get a worker registered with key, if the key does not exist a null sptr is returned
     * @note This method is thread safe.
     */
    FWTHREAD_API ::fwThread::Worker::sptr getWorker( const WorkerKeyType& key ) const;

    /// Register the default active worker.
    FWTHREAD_API static void setDefaultWorker( ::fwThread::Worker::sptr worker );

    /**
     * @brief Get the default registered worker
     * @note This method is thread safe.
     */
    static FWTHREAD_API ::fwThread::Worker::sptr getDefaultWorker();

    /**
     * @brief Registers a worker
     * @note This method is thread safe.
     */
    FWTHREAD_API void addWorker( const WorkerKeyType& key, ::fwThread::Worker::sptr worker );

    /**
     * @brief Initializes registry, creates and registers the default worker
     * @note This method is thread safe.
     */
    FWTHREAD_API void initRegistry();

    /**
     * @brief Unregisters all registered workers, s_DEFAULT_WORKER also
     * @note This method is thread safe.
     */
    FWTHREAD_API void clearRegistry();

    /// Returns an instance of ActiveWorkers.
    FWTHREAD_API static ActiveWorkers::sptr getDefault();

protected:

    /// Association key <=> worker
    WorkerMapType m_workers;

    /// Used to protect the registry access.
    mutable core::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the active referenced workers.
    static ActiveWorkers::sptr s_currentActiveWorkers;

};

} // namespace FWTHREAD
