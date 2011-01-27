#include <sstream>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/String.hpp>
#include <fwData/Acquisition.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwCore/spyLog.hpp>
#include "opSofa/SofaSceneReaderSrv.hpp"
#include <QInputDialog>
#include <QFileDialog>

namespace opSofa
{

REGISTER_SERVICE( ::io::IReader, ::opSofa::SofaSceneReaderSrv, ::fwData::Acquisition );

/**
 * @brief Constructor
 */
SofaSceneReaderSrv::SofaSceneReaderSrv() throw()
{
}

/**
 * @brief Destructor
 */
SofaSceneReaderSrv::~SofaSceneReaderSrv() throw()
{
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaSceneReaderSrv::configuring() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Used to launch the service.
 */
void SofaSceneReaderSrv::starting() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Used to stop the service.
 */
void SofaSceneReaderSrv::stopping() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaSceneReaderSrv::updating() throw ( ::fwTools::Failed )
{
    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    SLM_ASSERT("Associated object is not an acquisition", acq);

     // Open dialog box to select file .scn
    static QString defaultPath;
    std::string fileScn = QFileDialog::getOpenFileName(0, "Ouvrir un fichier scene", defaultPath, "Scene (*.scn)").toStdString();
    defaultPath = QString::fromStdString(fileScn);

    if (fileScn != "") {
        // Notification
        ::fwServices::ObjectMsg::NewSptr msg;
        ::fwData::String::sptr data(new ::fwData::String(fileScn));
        msg->addEvent( "NEW_SOFA_SCENE", data );
        ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
    }
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param _pMsg : Incoming message
 */
void SofaSceneReaderSrv::updating( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{}

/**
 * @brief info of the class
 */
void SofaSceneReaderSrv::info( std::ostream & ostr )
{
}

/**
 * @brief Configure IHM
 */
void SofaSceneReaderSrv::configureWithIHM()
{
}

}
