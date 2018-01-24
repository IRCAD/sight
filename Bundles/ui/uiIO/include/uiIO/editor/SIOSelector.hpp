/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
 * @brief   Defines the service interface managing the editor service for object.
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

    fwCoreServiceClassDefinitionsMacro( (SIOSelector)( ::fwGui::editor::IDialogEditor) );

    typedef ::fwCom::Signal< void ( ::fwJobs::IJob::sptr ) > JobCreatedSignalType;
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
     *
     * Sample of configuration :
     * @code{.xml}
          <service uid="GENERIC_UID_writer" type="::fwGui::editor::IDialogEditor" impl="::uiIO::editor::SIOSelector"
     * autoConnect="no">
              <type mode="writer" />
              <selection mode="include" />
              <addSelection service="::ioAtoms::SWriter" />
              <config id="ioAtomsConfig" service="::ioAtoms::SWriter" />
          </service>
     * @endcode
     * With :
     * None of these parameters are mandatory.
     *  - \b type
     *      - \b mode (mandatory) : selector type must be "reader" (to open file) or "writer" (to write a new file).
     *  - \b selection
     *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the
     * selection of the selector list).
     *  - \b addSelection
     *      - \b service (mandatory) : Name of the service to include/exclude to the choice list of the selector.
     *  - \b config
     *      - \b id (mandatory) : the id of the configuration to use.
     *      - \b service (mandatory) :  the name of the service.
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

    std::string m_inject;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
    SPTR(ForwardJobSlotType) m_slotForwardJob;

};

} // namespace editor

} // namespace uiIO
