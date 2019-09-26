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

#pragma once

#include "uiIO/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>

#include <fwGui/editor/IDialogEditor.hpp>

#include <fwJobs/IJob.hpp>

namespace uiIO
{
namespace editor
{

/**
 * @brief  This service displays a list of available readers or writers and lets you select one to load or save a data.
 *
 * @section Signals Signals
 * - \b jobCreated(::fwJobs::IJob::sptr) : emitted when a job is created.
 * - \b jobFailed() : emitted when the job has been cancelled by the user or has failed.
 * - \b jobSucceeded() : emitted when a job finishes correctly.
 *
 * @section Slots Slots
 * - \b forwardJob(::fwJobs::IJob::sptr ) : slot connected to the reader/writer to forward the signal 'jobCreated'
 *
 * @section XML XML Configuration
 *
 * Sample of configuration :
 * @code{.xml}
          <service uid="..." type="::uiIO::editor::SIOSelector">
              <inout key="data" uid="${selection}" />
              <type mode="writer" />
              <selection mode="include" />
              <addSelection service="::ioAtoms::SWriter" />
              <config id="ioAtomsConfig" service="::ioAtoms::SWriter" />
          </service>
 * @endcode
 *
 * Readers can generate output data. In this case, you must define the type of the output data.
 *
 * @code{.xml}
          <service uid="..." type="::uiIO::editor::SIOSelector">
              <out key="data" uid="image" />
              <type mode="reader" class="::fwData::Image" />
              <selection mode="include" />
              <addSelection service="::ioAtoms::SReader" />
          </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwData::Object]: the read or saved object.
 * @subsection Output Output
 * - \b data [::fwData::Object]: the loaded object (not used if an inout data is defined).
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
class UIIO_CLASS_API SIOSelector : public ::fwGui::editor::IDialogEditor
{

public:

    /// IOMode enum definition
    typedef enum
    {
        READER_MODE, /**< this mode allows to configure the service as a reader */
        WRITER_MODE  /**< this mode allows to configure the service as a writer */
    } IOMode;

    fwCoreServiceMacro(SIOSelector,  ::fwGui::editor::IDialogEditor);

    typedef ::fwCom::Signal< void ( ::fwJobs::IJob::sptr ) > JobCreatedSignalType;
    typedef ::fwCom::Signal< void ( ) > JobFailedSignalType;
    typedef ::fwCom::Signal< void ( ) > JobSucceededSignalType;

    typedef ::fwCom::Slot< void ( ::fwJobs::IJob::sptr ) > ForwardJobSlotType;

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, the SIOSelector::m_mode is defined as READER_MODE, and SIOSelector::m_servicesAreExcluded as true.
     */
    UIIO_API SIOSelector();

    /// Destructor. Do nothing.
    UIIO_API virtual ~SIOSelector() noexcept;

    /**
     * @brief This method allows to configure the service in reader or writer mode (set SIOSelector::m_mode).
     *
     *@param[in] _mode the value can be SIOSelector::READER_MODE or SIOSelector::WRITER_MODE.
     */
    UIIO_API void setIOMode( IOMode _mode );

protected:

    ///Starts the service. Do nothing.
    UIIO_API void starting() override;

    /// Stops the service. Do nothing.
    UIIO_API void stopping() override;

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, SIOSelector::m_selectedServices value is up to date (cleared before reconfiguring).
     **/
    UIIO_API void configuring() override;

    /// Create a dialogue box to provide the user different available readers (writer) for the IOSelector associated
    // objects. Then, the selected reader (writer) is executed.
    UIIO_API void updating() override;

    /// Gives the name of the class. Do nothing.
    UIIO_API void info( std::ostream& _sstream ) override;

private:

    void forwardJob(::fwJobs::IJob::sptr iJob);

    /// Configure the service as writer or reader.
    IOMode m_mode;

    /// Configure if selected services are included or excluded.
    bool m_servicesAreExcluded;

    /**
     * @brief List of services to be included or excluded.
     *
     * @see SIOSelector::m_servicesAreExcluded.
     */
    std::vector< std::string > m_selectedServices;

    /// Map that specifies a configuration extension for a service
    std::map< std::string, std::string > m_serviceToConfig;

    /// classname of the readed object (used if the data is set as output instead of inout)
    std::string m_dataClassname;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
    SPTR(JobFailedSignalType) m_sigJobFailed;
    SPTR(JobSucceededSignalType) m_sigJobSucceeded;

    SPTR(ForwardJobSlotType) m_slotForwardJob;

};

} // namespace editor

} // namespace uiIO
