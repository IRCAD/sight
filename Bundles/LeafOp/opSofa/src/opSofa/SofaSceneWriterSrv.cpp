#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/String.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwCore/spyLog.hpp>
#include <fwDataIO/writer/TriangularMeshWriter.hpp>
#include "opSofa/SofaSceneWriterSrv.hpp"
#include <QString>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

namespace opSofa
{


REGISTER_SERVICE( ::io::IWriter, ::opSofa::SofaSceneWriterSrv, ::fwData::Acquisition );

/**
 * @brief Constructor
 */
SofaSceneWriterSrv::SofaSceneWriterSrv() throw()
{
    writeTrian = false;
    m_sceneTemplate = "";
    m_useTempPath = false;
}

/**
 * @brief Destructor
 */
SofaSceneWriterSrv::~SofaSceneWriterSrv() throw()
{
}

/**
 * @brief Used to define the service parameters and analyze its configuration.
 */
void SofaSceneWriterSrv::configuring() throw ( ::fwTools::Failed )
{
    if(m_configuration->findConfigurationElement("writeTrian"))
    {
        std::string write = m_configuration->findConfigurationElement("writeTrian")->getValue();
        if (write == "yes") {
            writeTrian = true;
        }
    }

    if(m_configuration->findConfigurationElement("sceneTemplate"))
    {
        m_sceneTemplate = m_configuration->findConfigurationElement("sceneTemplate")->getValue();
    }

    if(m_configuration->findConfigurationElement("useTempPath"))
    {
        if (m_configuration->findConfigurationElement("useTempPath")->getValue() == "yes") {
            m_useTempPath = true;
        }
    }
}

/**
 * @brief Used to launch the service.
 */
void SofaSceneWriterSrv::starting() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Used to stop the service.
 */
void SofaSceneWriterSrv::stopping() throw ( ::fwTools::Failed )
{
}

/**
 * @brief Called to do an action on the data associated to the service.
 */
void SofaSceneWriterSrv::updating() throw ( ::fwTools::Failed )
{
    // Ask folder destination
    QString folder;
    if (m_useTempPath) {
        folder = QDir::tempPath() + QDir::separator().toAscii() + "opsofascene";
        QDir dir;
        dir.mkdir(folder);
    } else if (writeTrian) {
        folder = QFileDialog::getExistingDirectory(0, "Choose a folder to write file scene");
    } else {
        folder = QFileDialog::getSaveFileName(0, "Write file scn", QString(), "Scene (*.scn)");
    }
    if (folder == "") return;

    // Get acquisition
    ::fwData::Acquisition::sptr acq = this->getObject< ::fwData::Acquisition >();
    SLM_ASSERT("Associated object is not an acquisition", acq);

    // Get templates
    QString templateFile;
    QString nodeTemplateFile;
    if (m_sceneTemplate != "") {
        QFile file("./Bundles/opSofa_0-1/" + QString(m_sceneTemplate.c_str()));
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        templateFile = file.readAll();
        QFile file2("./Bundles/opSofa_0-1/node" + QString(m_sceneTemplate.c_str()));
        file2.open(QIODevice::ReadOnly | QIODevice::Text);
        nodeTemplateFile = file2.readAll();
    } else {
        QFile file("./Bundles/opSofa_0-1/template.xml");
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        templateFile = file.readAll();
        QFile file2("./Bundles/opSofa_0-1/nodeTemplate.xml");
        file2.open(QIODevice::ReadOnly | QIODevice::Text);
        nodeTemplateFile = file2.readAll();
    }

    // copy tools
    if (writeTrian && m_sceneTemplate != "") {
        QFile file("./Bundles/opSofa_0-1/cam.trian");
        file.copy(folder + "/cam.trian");
        QFile file2("./Bundles/opSofa_0-1/mors2.trian");
        file2.copy(folder + "/mors2.trian");
    }

    // Create nodes
    QString nodesData;

    // Travel each reconstructions
    std::pair< ::fwData::Acquisition::ReconstructionIterator, ::fwData::Acquisition::ReconstructionIterator > reconstructionIters = acq->getReconstructions();
    ::fwData::Acquisition::ReconstructionIterator reconstruction = reconstructionIters.first;
    for(; reconstruction != reconstructionIters.second;  ++reconstruction) {
        ::fwData::Reconstruction::sptr rec = (*reconstruction);

        // Get info organ
        QString organName = QString(rec->getOrganName().c_str());
        bool organVisible = rec->getIsVisible();
        QString organUid = QString(rec->getID().c_str());
        ::boost::filesystem::path filename = "";

        if (organVisible && organName != "mors2" && organName != "cam") {
            // Save mesh in filesystem
            if (writeTrian) {
                ::fwData::TriangularMesh::sptr mesh = rec->getTriangularMesh();
                std::stringstream meshPath;
                meshPath << folder.toStdString() << QDir::separator().toAscii() << organName.toStdString() << ".trian";
                filename = ::boost::filesystem::path(meshPath.str());
                ::fwDataIO::writer::TriangularMeshWriter writer;
                writer.setObject(mesh);
                writer.setFile(filename);
                writer.write();
            }

            // Parse nodeTemplate
            QString nodeFile = nodeTemplateFile;
            nodeFile.replace("ORGAN_NAME", organName);
            nodeFile.replace("ORGAN_ID", organUid);
            nodeFile.replace("TRIAN_LOCATION", QString(filename.string().c_str()));
            
            // Add node
            nodesData += nodeFile;
        }
    }    
    
    // Parse template
    templateFile.replace("AREMPLACER", nodesData);

    // Save file scn
    std::string scnFile;
    if (writeTrian) {
        scnFile =  folder.toStdString() + QDir::separator().toAscii()  + "sofa.scn";
    } else {
        scnFile =  folder.toStdString();
    }
    std::ofstream fileout (scnFile.c_str(), std::ios::out);
    fileout << templateFile.toStdString();
    fileout.close();

    // Ask launch animation
    int answer;
    if (m_useTempPath) {
        answer = QMessageBox::Yes;
    } else {
        answer = QMessageBox::question(0, "Write successful !", "Do you want to launch animation ?", QMessageBox::Yes | QMessageBox::No);
    }

    // If answer is yes
    if (answer == QMessageBox::Yes) {
        // Notification
        ::fwServices::ObjectMsg::NewSptr msg;
        ::fwData::String::sptr data(new ::fwData::String(scnFile));
        msg->addEvent( "NEW_SOFA_SCENE", data );
        ::fwServices::IEditionService::notify(this->getSptr(), acq, msg);
    }
}

/**
 * @brief Called by a data to notify a service.
 *
 * @param _pMsg : Incoming message
 */
void SofaSceneWriterSrv::updating( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed )
{
}

/**
 * @brief info of the class
 */
void SofaSceneWriterSrv::info( std::ostream & ostr )
{
}

/**
 * @brief Configure IHM
 */
void SofaSceneWriterSrv::configureWithIHM()
{
}

}
