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

#include "core/config.hpp"
#include <core/mt/types.hpp>
#include <core/thread/Worker.hpp>

namespace sight::core::thread
{

/**
 * @brief This class to register active worker in the system, creates a default worker
 */
class CORE_CLASS_API ActiveWorkers : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(ActiveWorkers, core::BaseObject, std::make_shared<ActiveWorkers>);

    typedef std::string WorkerKeyType;

    typedef std::map<WorkerKeyType, core::thread::Worker::sptr> WorkerMapType;

    /// Key of default worker in registry, created and registered by initRegistry method
    CORE_API static const std::string s_DEFAULT_WORKER;

    /// Constructor, does nothing
    CORE_API ActiveWorkers();

    /// Destructor, does nothing
    CORE_API virtual ~ActiveWorkers();

    /**
     * @brief Get a worker registered with key, if the key does not exist a null sptr is returned
     * @note This method is thread safe.
     */
    CORE_API core::thread::Worker::sptr getWorker(const WorkerKeyType& key) const;

    /// Register the default active worker.
    CORE_API static void setDefaultWorker(core::thread::Worker::sptr worker);

    /**
     * @brief Get the default registered worker
     * @note This method is thread safe.
     */
    static CORE_API core::thread::Worker::sptr getDefaultWorker();

    /**
     * @brief Registers a worker
     * @note This method is thread safe.
     */
    CORE_API void addWorker(const WorkerKeyType& key, core::thread::Worker::sptr worker);

    /**
     * @brief Initializes registry, creates and registers the default worker
     * @note This method is thread safe.
     */
    CORE_API void initRegistry();

    /**
     * @brief Unregisters all registered workers, s_DEFAULT_WORKER also
     * @note This method is thread safe.
     */
    CORE_API void clearRegistry();

    /// Returns an instance of ActiveWorkers.
    CORE_API static ActiveWorkers::sptr getDefault();

protected:

    /// Association key <=> worker
    WorkerMapType m_workers;

    /// Used to protect the registry access.
    mutable core::mt::ReadWriteMutex m_registryMutex;

    /// The global instance of the active referenced workers.
    static ActiveWorkers::sptr s_currentActiveWorkers;
};

} // namespace sight::core::thread
