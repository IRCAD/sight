/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "modules/ui/base/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Slot.hpp>
#include <core/jobs/IJob.hpp>

#include <io/base/service/ioTypes.hpp>

#include <ui/base/IDialogEditor.hpp>

namespace sight::module::ui::base
{

namespace io
{

/**
 * @brief  This service displays a list of available readers or writers and lets you select one to load or save a data.
 *
 * @section Signals Signals
 * - \b jobCreated(core::jobs::IJob::sptr) : emitted when a job is created.
 * - \b jobFailed() : emitted when the job has been cancelled by the user or has failed.
 * - \b jobSucceeded() : emitted when a job finishes correctly.
 *
 * @section Slots Slots
 * - \b forwardJob(core::jobs::IJob::sptr ) : slot connected to the reader/writer to forward the signal 'jobCreated'
 *
 * @section XML XML Configuration
 *
 * Sample of configuration :
 * @code{.xml}
          <service uid="..." type="sight::module::ui::base::io::SSelector">
              <inout key="data" uid="${selection}" />
              <type mode="writer" />
              <selection mode="include" />
              <addSelection service="::sight::module::io::atoms::SWriter" />
              <config id="ioAtomsConfig" service="::sight::module::io::atoms::SWriter" />
          </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::Object]: the read or saved object.
 * @subsection Configuration Configuration
 * - \b type
 *      - \b mode (mandatory) : selector type must be "reader" (to open file) or "writer" (to write a new file).
 *      - \b data (mandatory if the object is set as an output): classname of the object to read
 * - \b selection
 *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the
 * selection of the selector list).
 * - \b addSelection
 *      - \b service (mandatory) : Name of the service to include/exclude to the choice list of the selector.
 * - \b config
 *      - \b id (mandatory) : the id of the configuration to use.
 *      - \b service (mandatory) :  the name of the service.
 */
class MODULE_UI_BASE_CLASS_API SSelector : public sight::ui::base::IDialogEditor
{
public:

    /// IOMode enum definition
    typedef enum
    {
        READER_MODE, /**< this mode allows to configure the service as a reader */
        WRITER_MODE  /**< this mode allows to configure the service as a writer */
    } IOMode;

    SIGHT_DECLARE_SERVICE(SSelector, sight::ui::base::IDialogEditor);

    typedef core::com::Signal<void (core::jobs::IJob::sptr)> JobCreatedSignalType;
    typedef core::com::Signal<void ()> JobFailedSignalType;
    typedef core::com::Signal<void ()> JobSucceededSignalType;

    typedef core::com::Slot<void (core::jobs::IJob::sptr)> ForwardJobSlotType;

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, the SSelector::m_mode is defined as READER_MODE, and SSelector::m_servicesAreExcluded as true.
     */
    MODULE_UI_BASE_API SSelector();

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API virtual ~SSelector() noexcept;

    /**
     * @brief This method allows to configure the service in reader or writer mode (set SSelector::m_mode).
     *
     *@param[in] _mode the value can be SSelector::READER_MODE or SSelector::WRITER_MODE.
     */
    MODULE_UI_BASE_API void setIOMode(IOMode _mode);

protected:

    ///Starts the service. Do nothing.
    MODULE_UI_BASE_API void starting() override;

    /// Stops the service. Do nothing.
    MODULE_UI_BASE_API void stopping() override;

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, SSelector::m_selectedServices value is up to date (cleared before reconfiguring).
     **/
    MODULE_UI_BASE_API void configuring() override;

    /// Create a dialogue box to provide the user different available readers (writer) for the IOSelector associated
    // objects. Then, the selected reader (writer) is executed.
    MODULE_UI_BASE_API void updating() override;

    /// Gives the name of the class. Do nothing.
    MODULE_UI_BASE_API void info(std::ostream& _sstream) override;

private:

    void forwardJob(core::jobs::IJob::sptr iJob);

    /// Configure the service as writer or reader.
    IOMode m_mode;

    /// Configure if selected services are included or excluded.
    bool m_servicesAreExcluded;

    /**
     * @brief List of services to be included or excluded.
     *
     * @see SSelector::m_servicesAreExcluded.
     */
    std::vector<std::string> m_selectedServices;

    /// Map that specifies a configuration extension for a service
    std::map<std::string, std::string> m_serviceToConfig;

    /// classname of the readed object (used if the data is set as output instead of inout)
    std::string m_dataClassname;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
    SPTR(JobFailedSignalType) m_sigJobFailed;
    SPTR(JobSucceededSignalType) m_sigJobSucceeded;

    SPTR(ForwardJobSlotType) m_slotForwardJob;

    data::ptr<data::Object, data::Access::inout> m_data {this, sight::io::base::service::s_DATA_KEY, false};
};

} // namespace io

} // namespace sight::module::ui::base
