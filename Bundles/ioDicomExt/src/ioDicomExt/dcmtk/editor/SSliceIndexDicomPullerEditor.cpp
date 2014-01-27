/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QComboBox>
#include <boost/asio/placeholders.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwDicomIOExt/exceptions/Base.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwComEd/helper/SeriesDB.hpp>
#include <fwComEd/helper/Composite.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwTools/System.hpp>

#include "ioDicomExt/dcmtk/editor/SSliceIndexDicomPullerEditor.hpp"

namespace ioDicomExt
{

namespace dcmtk
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor , ::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor , ::fwDicomData::DicomSeries ) ;

const ::fwCom::Slots::SlotKeyType SSliceIndexDicomPullerEditor::s_READ_IMAGE_SLOT = "readImage";
const ::fwCom::Slots::SlotKeyType SSliceIndexDicomPullerEditor::s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::SSliceIndexDicomPullerEditor() throw():
        m_delayWork( m_delayService),
        m_delayThread( ::boost::bind( &SSliceIndexDicomPullerEditor::runDelay, this)),
        m_delayTimer( m_delayService)
{

    m_slotReadImage = ::fwCom::newSlot(&SSliceIndexDicomPullerEditor::readImage, this);
    ::fwCom::HasSlots::m_slots(s_READ_IMAGE_SLOT, m_slotReadImage);

    m_slotDisplayMessage = ::fwCom::newSlot(&SSliceIndexDicomPullerEditor::displayErrorMessage, this);
    ::fwCom::HasSlots::m_slots(s_DISPLAY_MESSAGE_SLOT, m_slotDisplayMessage);

#ifdef COM_LOG
   m_slotReadImage->setID( s_READ_IMAGE_SLOT );
   m_slotDisplayMessage->setID( s_DISPLAY_MESSAGE_SLOT );
   ::fwCom::HasSignals::m_signals.setID();
#endif

   ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

}
//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::~SSliceIndexDicomPullerEditor() throw()
{
    m_delayService.stop();
    m_delayThread.join();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::info(std::ostream &_sstream )
{
    _sstream << "SSliceIndexDicomPullerEditor::info" ;
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get pacs configuration
    m_pacsConfiguration = ::fwDicomIOExt::data::PacsConfiguration::dynamicCast(::fwTools::fwID::getObject(m_pacsConfigurationUID));
    SLM_ASSERT("The pacs configuration object should not be null.", m_pacsConfiguration);

    // Composite
    m_composite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(m_compositeUID));
    SLM_ASSERT("Composite should not be null !", m_composite);

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();
    QHBoxLayout* layout = new QHBoxLayout();
    container->setLayout(layout);

    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    m_numberOfSlices = dicomSeries->getNumberOfInstances();

    // Slider
    m_sliceIndexSlider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_sliceIndexSlider,1);
    m_sliceIndexSlider->setRange(0, m_numberOfSlices-1);
    m_sliceIndexSlider->setValue(m_numberOfSlices/2);

    // Line Edit
    m_sliceIndexLineEdit = new QLineEdit();
    layout->addWidget(m_sliceIndexLineEdit,0);
    m_sliceIndexLineEdit->setReadOnly(true);
    m_sliceIndexLineEdit->setMaximumWidth(80);

    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());


    // Connect the signals
    QObject::connect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    // Create temporary SeriesDB
    m_tempSeriesDB = ::fwMedData::SeriesDB::New();

    // Create reader
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();
    m_dicomReader = ::io::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
            "::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor.", m_dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, m_dicomReader);
    m_dicomReader->configure();
    m_dicomReader->start();

    // Image Indecies
    m_axialIndex = ::fwData::Integer::New(0);
    m_frontalIndex = ::fwData::Integer::New(0);
    m_sagittalIndex = ::fwData::Integer::New(0);

    // Worker
    m_pullSeriesWorker = ::fwThread::Worker::New();

    // Create enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();

    // Load a slice
    m_delayTimer.expires_from_now( boost::posix_time::milliseconds( 500 ));
    m_delayTimer.async_wait( boost::bind( &SSliceIndexDicomPullerEditor::triggerNewSlice, this,
            boost::asio::placeholders::error));

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Worker
    m_pullSeriesWorker->stop();
    m_pullSeriesWorker.reset();

    // Stop dicom reader
    m_dicomReader->stop();
    ::fwServices::OSR::unregisterService(m_dicomReader);

    // Disconnect the signals
    QObject::disconnect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomExt::dcmtk::controller::SPacsConfigurationInitializer must have a \"config\" element.",config);

    bool success;

    // Pacs Configuration UID
    ::boost::tie(success, m_pacsConfigurationUID) = config->getSafeAttributeValue("pacsConfigurationUID");
    SLM_ASSERT("It should be a \"pacsConfigurationUID\" tag in the ::ioDicomExt::dcmtk::editor::SQueryEditor config element.", success);

    // Composite UID
    ::boost::tie(success, m_compositeUID) = config->getSafeAttributeValue("compositeUID");
    SLM_ASSERT("It should be a \"compositeUID\" tag in the ::ioDicomExt::dcmtk::editor::SQueryEditor config element.", success);

    // Image Key
    ::boost::tie(success, m_imageKey) = config->getSafeAttributeValue("imageKey");
    SLM_ASSERT("It should be an \"imageKey\" tag in the "
            "::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor config element.", success);

    // IODICOMEXT Reader
    ::boost::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the ::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor config element.", success);

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::changeSliceIndex(int value)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for 500 milliseconds
    m_delayTimer.expires_from_now( boost::posix_time::milliseconds( 500 ));
    m_delayTimer.async_wait( boost::bind( &SSliceIndexDicomPullerEditor::triggerNewSlice, this,
            boost::asio::placeholders::error));

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::triggerNewSlice(const boost::system::error_code& e)
{
    if(e != ::boost::asio::error::operation_aborted)
    {
        // DicomSeries
        ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
        SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

        // Compute slice index
        unsigned int selectedSliceIndex = m_numberOfSlices - m_sliceIndexSlider->value();
        if(!dicomSeries->isInstanceAvailable(selectedSliceIndex))
        {
            m_pullSeriesWorker->post(::boost::bind(&::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor::pullInstance, this));
        }
        else
        {
            m_slotReadImage->asyncRun(selectedSliceIndex);
        }
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::runDelay()
{
    m_delayService.run();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::readImage(unsigned int selectedSliceIndex)
{
    // DicomSeries
    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Clear temporary seriesDB
    ::fwComEd::helper::SeriesDB sDBTempohelper(m_tempSeriesDB);
    sDBTempohelper.clear();

    //Clear tmp Folder
    ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "dicom/";
    ::boost::filesystem::path tmpPath = ::boost::filesystem::path(path.string()+"tmp/");
    if (::boost::filesystem::exists(tmpPath))
    {
        ::boost::filesystem::remove_all(tmpPath);
    }
    ::boost::filesystem::create_directories(tmpPath);

    ::fwDicomData::DicomSeries::DicomPathContainerType paths = dicomSeries->getLocalDicomPaths();
    ::boost::filesystem::path src = paths[selectedSliceIndex];
    ::boost::filesystem::path dest = tmpPath.string() + src.filename().string();
    ::boost::filesystem::copy(src, dest);

    // Read image
    m_dicomReader->setFolder(tmpPath);
    m_dicomReader->update();

    //Copy image
    ::fwMedData::ImageSeries::sptr imageSeries =
            ::fwMedData::ImageSeries::dynamicCast(*(m_tempSeriesDB->getContainer().begin()));
    if(imageSeries)
    {
        ::fwComEd::helper::Composite helper(m_composite);
        ::fwData::Image::sptr newImage = imageSeries->getImage();
        ::fwData::Image::SizeType newSize = newImage->getSize();

        if(m_composite->find(m_imageKey) == m_composite->end())
        {
            helper.add(m_imageKey, newImage);
        }
        else
        {
            ::fwData::Image::sptr oldImage = ::fwData::Image::dynamicCast(m_composite->getContainer()[m_imageKey]);
            ::fwData::Image::SizeType oldSize = oldImage->getSize();
            if(newSize[0] == oldSize[0] && newSize[1] == oldSize[1] && newSize[2] == oldSize[2])
            {
                // Copy buffer
                oldImage->setDataArray(newImage->getDataArray(), false);
                ::fwComEd::ImageMsg::sptr msg = ::fwComEd::ImageMsg::New();
                msg->addEvent(::fwComEd::ImageMsg::MODIFIED);
                ::fwServices::IEditionService::notify(this->getSptr(),  oldImage, msg);
            }
            else
            {
                // Swap image
                helper.swap(m_imageKey, newImage);
            }

        }

        newImage->setField(::fwComEd::Dictionary::m_axialSliceIndexId, m_axialIndex);
        m_frontalIndex->setValue(newSize[0]/2);
        newImage->setField(::fwComEd::Dictionary::m_frontalSliceIndexId, m_frontalIndex);
        m_sagittalIndex->setValue(newSize[1]/2);
        newImage->setField(::fwComEd::Dictionary::m_sagittalSliceIndexId, m_sagittalIndex);

        helper.notify(this->getSptr());

    }

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::pullInstance()
{
    // Catch any errors
    try
    {
        // DicomSeries
        ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
        SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

        // Get selected slice
        unsigned int selectedSliceIndex = m_numberOfSlices - m_sliceIndexSlider->value();

        m_seriesEnquirer->initialize(
                m_pacsConfiguration->getLocalApplicationTitle(),
                m_pacsConfiguration->getPacsHostName(),
                m_pacsConfiguration->getPacsApplicationPort(),
                m_pacsConfiguration->getPacsApplicationTitle(),
                m_pacsConfiguration->getMoveApplicationTitle());

        m_seriesEnquirer->connect();
        std::string seriesInstanceUID = dicomSeries->getInstanceUID();
        std::string sopInstanceUID = m_seriesEnquirer->findSOPInstanceUID(seriesInstanceUID, selectedSliceIndex);

        if(!sopInstanceUID.empty())
        {
            // Pull Selected Series using C-GET Requests
            m_seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);
            m_seriesEnquirer->disconnect();
        }

        ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "dicom/";
        ::boost::filesystem::path filePath = path.string() + seriesInstanceUID + "/" + sopInstanceUID;
        dicomSeries->addDicomPath(selectedSliceIndex, filePath);
        m_slotReadImage->asyncRun(selectedSliceIndex);

    }
    catch (::fwDicomIOExt::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
                << "Pacs host name: " << m_pacsConfiguration->getPacsHostName() << "\n"
                << "Pacs application title: " << m_pacsConfiguration->getPacsApplicationTitle() << "\n"
                << "Pacs port: " << m_pacsConfiguration->getPacsApplicationPort() << "\n";
        m_slotDisplayMessage->asyncRun(ss.str());
        SLM_WARN(exception.what());
    }

}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::displayErrorMessage(const std::string& message) const
{
    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace editor
} // namespace dcmtk
} // namespace ioDicomExt
