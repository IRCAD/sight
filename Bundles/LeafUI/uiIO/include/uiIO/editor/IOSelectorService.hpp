/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIO_EDITOR_IOSELECTORSERVICE_HPP_
#define _UIIO_EDITOR_IOSELECTORSERVICE_HPP_

#include <gui/editor/IDialogEditor.hpp>

#include "uiIO/config.hpp"

namespace uiIO
{
namespace editor
{

/**
 * @brief   Defines the service interface managing the editor service for object.
 * @class   IOSelectorService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class UIIO_CLASS_API IOSelectorService : public ::gui::editor::IDialogEditor
{

public :

    /// IOMode enum definition
    typedef enum {
        READER_MODE, /**< this mode allows to configure the service as a reader */
        WRITER_MODE  /**< this mode allows to configure the service as a writer */
    } IOMode;

    fwCoreServiceClassDefinitionsMacro ( (IOSelectorService)( ::gui::editor::IDialogEditor) ) ;

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     *
     * By default, the IOSelectorService::m_mode is defined as READER_MODE, and IOSelectorService::m_servicesAreExcluded as true.
     */
    UIIO_API IOSelectorService();

    /// Destructor. Do nothing.
    UIIO_API virtual ~IOSelectorService() throw() ;

    /**
     * @brief This method allows to configure the service in reader or writer mode (set IOSelectorService::m_mode).
     *
     *@param[in] _mode the value can be IOSelectorService::READER_MODE or IOSelectorService::WRITER_MODE.
     */
    UIIO_API void setIOMode( IOMode _mode ) ;

protected:

    ///Starts the service. Do nothing.
    UIIO_API void starting() throw( ::fwTools::Failed ) ;

    /// Stops the service. Do nothing.
    UIIO_API void stopping() throw( ::fwTools::Failed ) ;

    /**
     * @brief   This method initializes class member parameters from configuration elements.
     *
     * The method verifies if the configuration is well written and retrieves user parameter values.
     * Thanks to this method, IOSelectorService::m_selectedServices value is up to date (cleared before reconfiguring).
     *
     * Sample of configuration :
     * @verbatim
          <service uid="GENERIC_UID_writerMedicalWorkspace" type="::gui::editor::IDialogEditor" implementation="::uiIO::editor::IOSelectorService" autoComChannel="no">
              <type mode="writer" />
              <selection mode="include" />
              <addSelection service="::ioXML::FwXMLGenericWriterService" />
              <config id="FwXMLGenericWriterReaderMWZConfig" service="::ioXML::FwXMLGenericWriterService" />
          </service>
     * @endverbatim
     * With :
     * None of these parameters are mandatory.
     *  - \b type
     *      - \b mode (mandatory) : selector type must be "reader" (to open file) or "writer" (to write a new file).
     *  - \b selection
     *      - \b mode (mandatory) : must be include (to add the selection to selector list ) or exclude (to exclude the selection of the selector list).
     *  - \b addSelection
     *      - \b service (mandatory) : Name of the service to include/exclude to the choice list of the selector.
     *  - \b config
     *      - \b id (mandatory) : the id of the configuration to use.
     *      - \b service (mandatory) :  the name of the service.
    **/
    UIIO_API void configuring() throw( ::fwTools::Failed ) ;

    /// Create a dialogue box to provide the user different available readers (writer) for the IOSelector associated objects. Then, the selected reader (writer) is executed.
    UIIO_API void updating() throw( ::fwTools::Failed ) ;

    /// SLM_FATAL require an implementation gui::editor::IEditor::updating(msg)
    UIIO_API void updating( fwServices::ObjectMsg::csptr ) throw( ::fwTools::Failed );

    /// Gives the name of the class. Do nothing.
    UIIO_API void info( std::ostream &_sstream ) ;

private :

    /// Configure the service as writer or reader.
    IOMode m_mode;

    /// Configure if selected services are included or excluded.
    bool m_servicesAreExcluded;

    /**
    * @brief List of services to be included or excluded.
    *
    * @see IOSelectorService::m_servicesAreExcluded.
    */
    std::vector< std::string > m_selectedServices;

    /// Map that specifies a configuration extension for a service
    std::map< std::string, std::string > m_serviceToConfig;
};

} // namespace editor

} // namespace uiIO

#endif // _UIIO_EDITOR_IOSELECTORSERVICE_HPP_

