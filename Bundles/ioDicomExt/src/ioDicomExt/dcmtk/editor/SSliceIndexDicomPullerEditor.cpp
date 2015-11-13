/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicomExt/dcmtk/editor/SSliceIndexDicomPullerEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/helper/Array.hpp>
#include <fwComEd/helper/Composite.hpp>
#include <fwComEd/helper/SeriesDB.hpp>

#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>

#include <fwDicomData/DicomSeries.hpp>
#include <fwDicomIOExt/exceptions/Base.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTools/System.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QComboBox>

#include <boost/asio/placeholders.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iterator>
#include <functional>

namespace ioDicomExt
{

namespace dcmtk
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor,
                         ::fwDicomData::DicomSeries );

const ::fwCom::Slots::SlotKeyType SSliceIndexDicomPullerEditor::s_READ_IMAGE_SLOT      = "readImage";
const ::fwCom::Slots::SlotKeyType SSliceIndexDicomPullerEditor::s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomPullerEditor::SSliceIndexDicomPullerEditor() throw() :
    m_delayWork( m_delayService),
    m_delayThread( std::bind( &SSliceIndexDicomPullerEditor::runDelay, this)),
    m_delayTimer( m_delayService),
    m_delay(500)
{

    m_slotReadImage = ::fwCom::newSlot(&SSliceIndexDicomPullerEditor::readImage, this);
    ::fwCom::HasSlots::m_slots(s_READ_IMAGE_SLOT, m_slotReadImage);

    m_slotDisplayMessage = ::fwCom::newSlot(&SSliceIndexDicomPullerEditor::displayErrorMessage, this);
    ::fwCom::HasSlots::m_slots(s_DISPLAY_MESSAGE_SLOT, m_slotDisplayMessage);



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
    _sstream << "SSliceIndexDicomPullerEditor::info";
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get pacs configuration
    m_pacsConfiguration = ::fwDicomIOExt::data::PacsConfiguration::dynamicCast(
        ::fwTools::fwID::getObject(m_pacsConfigurationUID));

    // Composite
    m_composite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(m_compositeUID));
    SLM_ASSERT("Composite should not be null !", m_composite);

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();
    QHBoxLayout* layout      = new QHBoxLayout();
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
    m_dicomReader                                           =
        ::io::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
               "::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor.", m_dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, m_dicomReader);
    m_dicomReader->configure();
    m_dicomReader->start();

    // Image Indecies
    m_axialIndex    = ::fwData::Integer::New(0);
    m_frontalIndex  = ::fwData::Integer::New(0);
    m_sagittalIndex = ::fwData::Integer::New(0);

    // Worker
    m_pullSeriesWorker = ::fwThread::Worker::New();

    // Create enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();

    // Load a slice
    m_delayTimer.expires_from_now( ::boost::posix_time::milliseconds( m_delay ));
    m_delayTimer.async_wait( std::bind( &SSliceIndexDicomPullerEditor::triggerNewSlice, this, std::placeholders::_1));
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
    SLM_ASSERT("The service ::ioDicomExt::dcmtk::controller::SPacsConfigurationInitializer must have "
               "a \"config\" element.",config);

    bool success;

    // Pacs Configuration UID
    ::boost::tie(success, m_pacsConfigurationUID) = config->getSafeAttributeValue("pacsConfigurationUID");

    // Composite UID
    ::boost::tie(success, m_compositeUID) = config->getSafeAttributeValue("compositeUID");
    SLM_ASSERT("It should be a \"compositeUID\" tag in the ::ioDicomExt::dcmtk::editor::SQueryEditor config element.",
               success);

    // Image Key
    ::boost::tie(success, m_imageKey) = config->getSafeAttributeValue("imageKey");
    SLM_ASSERT("It should be an \"imageKey\" tag in the "
               "::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor config element.", success);

    // Reader
    ::boost::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the ::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor "
               "config element.", success);

    // Delay
    std::string delayStr;
    ::boost::tie(success, delayStr) = config->getSafeAttributeValue("delay");
    if(success)
    {
        m_delay = ::boost::lexical_cast< unsigned int >(delayStr);
    }

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

    // Get the new slice if there is no change for X milliseconds
    m_delayTimer.expires_from_now( ::boost::posix_time::milliseconds( m_delay ));
    m_delayTimer.async_wait( std::bind( &SSliceIndexDicomPullerEditor::triggerNewSlice, this, std::placeholders::_1));
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::triggerNewSlice(const ::boost::system::error_code& e)
{
    if(e != ::boost::asio::error::operation_aborted)
    {
        // DicomSeries
        ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
        SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

        // Compute slice index
        unsigned int selectedSliceIndex = m_numberOfSlices - m_sliceIndexSlider->value() - 1;
        OSLM_TRACE("triggered new slice : " << selectedSliceIndex);
        if(!dicomSeries->isInstanceAvailable(selectedSliceIndex))
        {
            if(m_pacsConfiguration)
            {
                m_pullSeriesWorker->post(
                    std::bind(&::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor::pullInstance, this));
            }
            else
            {
                SLM_ERROR("There is no instance available for selected slice index.");
            }
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
    if( dicomSeries->getModality() != "CT" && dicomSeries->getModality() != "MR")
    {
        return;
    }

    // Clear temporary seriesDB
    ::fwComEd::helper::SeriesDB sDBTempohelper(m_tempSeriesDB);
    sDBTempohelper.clear();

    // Creates unique temporary folder, no need to check if exists before (see ::fwTools::System::getTemporaryFolder)
    ::boost::filesystem::path path    = ::fwTools::System::getTemporaryFolder("dicom");
    ::boost::filesystem::path tmpPath = path / "tmp";

    SLM_INFO("Create " + tmpPath.string());
    ::boost::filesystem::create_directories(tmpPath);


    SLM_ASSERT("Dicom data shall be available before reading them.",
               dicomSeries->getDicomAvailability() != ::fwDicomData::DicomSeries::NONE
               || dicomSeries->isInstanceAvailable(selectedSliceIndex));

    if(dicomSeries->getDicomAvailability() != ::fwDicomData::DicomSeries::BINARIES )
    {
        ::fwDicomData::DicomSeries::DicomPathContainerType paths = dicomSeries->getLocalDicomPaths();
        ::boost::filesystem::path src                            = paths[selectedSliceIndex];
        ::boost::filesystem::path dest                           = tmpPath / src.filename();

        ::boost::system::error_code err;
        ::boost::filesystem::create_hard_link( src, dest, err );
        if (err.value() != 0)
        {
            SLM_INFO("Copying " + src.string() + " to " + dest.string());
            ::boost::filesystem::copy( src, dest );

            ::boost::system::error_code errPerm;
            ::boost::filesystem::permissions(dest, ::boost::filesystem::owner_all, errPerm);
            SLM_ERROR_IF("set permission error : " + errPerm.message(), errPerm.value());
        }
    }
    else if(dicomSeries->getDicomAvailability() == ::fwDicomData::DicomSeries::BINARIES)
    {

        const ::fwDicomData::DicomSeries::DicomBinaryContainerType& binaries = dicomSeries->getDicomBinaries();
        ::fwDicomData::DicomSeries::DicomBinaryContainerType::const_iterator binary = binaries.begin();
        std::advance(binary, selectedSliceIndex);

        ::fwData::Array::sptr array = binary->second;
        ::fwComEd::helper::Array arrayHelper(array);
        char* buffer = static_cast<char*>(arrayHelper.getBuffer());
        size_t size  = array->getSizeInBytes();

        ::boost::filesystem::path dest = tmpPath / binary->first;
        ::boost::filesystem::ofstream fs(dest, std::ios::binary|std::ios::trunc);
        FW_RAISE_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

        fs.write(buffer, size);
        fs.close();
    }

    // Read image
    m_dicomReader->setFolder(tmpPath);
    m_dicomReader->update();

    //Copy image
    ::fwMedData::ImageSeries::sptr imageSeries =
        ::fwMedData::ImageSeries::dynamicCast(*(m_tempSeriesDB->getContainer().begin()));
    if(imageSeries)
    {
        ::fwComEd::helper::Composite helper(m_composite);
        ::fwData::Image::sptr newImage    = imageSeries->getImage();
        ::fwData::Image::SizeType newSize = newImage->getSize();

        if(m_composite->find(m_imageKey) == m_composite->end())
        {
            helper.add(m_imageKey, newImage);
        }
        else
        {
            ::fwData::Image::sptr oldImage    = ::fwData::Image::dynamicCast(m_composite->getContainer()[m_imageKey]);
            ::fwData::Image::SizeType oldSize = oldImage->getSize();
            if(newSize[0] == oldSize[0] && newSize[1] == oldSize[1] && newSize[2] == oldSize[2])
            {
                // Copy buffer
                oldImage->setDataArray(newImage->getDataArray(), false);
                auto sig = oldImage->signal< ::fwData::Object::ModifiedSignalType >(
                    ::fwData::Object::s_MODIFIED_SIG);
                {
                    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
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

        helper.notify();
    }

    ::boost::system::error_code ec;
    ::boost::filesystem::remove_all(path, ec);
    SLM_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomPullerEditor::pullInstance()
{
    SLM_ASSERT("Pacs not configured.",m_pacsConfiguration);

    if( m_pacsConfiguration )
    {
        // Catch any errors
        try
        {
            // DicomSeries
            ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
            SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

            // Get selected slice
            unsigned int selectedSliceIndex = m_numberOfSlices - m_sliceIndexSlider->value() - 1;

            m_seriesEnquirer->initialize(m_pacsConfiguration->getLocalApplicationTitle(),
                                         m_pacsConfiguration->getPacsHostName(),
                                         m_pacsConfiguration->getPacsApplicationPort(),
                                         m_pacsConfiguration->getPacsApplicationTitle(),
                                         m_pacsConfiguration->getMoveApplicationTitle());

            m_seriesEnquirer->connect();
            std::string seriesInstanceUID = dicomSeries->getInstanceUID();
            std::string sopInstanceUID    = m_seriesEnquirer->findSOPInstanceUID(seriesInstanceUID, selectedSliceIndex);

            if(!sopInstanceUID.empty())
            {
                // Pull Selected Series using C-GET Requests
                m_seriesEnquirer->pullInstanceUsingGetRetrieveMethod(seriesInstanceUID, sopInstanceUID);
                m_seriesEnquirer->disconnect();
            }

            ::boost::filesystem::path path     = ::fwTools::System::getTemporaryFolder() / "dicom/";
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
    else
    {
        SLM_ERROR("Pacs pull aborted : no pacs configuration found.");
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
