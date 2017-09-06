/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioQt/SPdfWriter.hpp"

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/ConfigurationElementContainer.hpp>

#include <fwServices/macros.hpp>

#include <fwThread/Pool.hpp>
#include <fwThread/Worker.hpp>

#include <QPainter>
#include <QPixmap>

fwServicesRegisterMacro( ::io::IWriter, ::ioQt::SPdfWriter, ::fwData::Object );

namespace ioQt
{

const ::fwServices::IService::KeyType s_IMAGE_INPUT     = "image";
const ::fwServices::IService::KeyType s_CONTAINER_INPUT = "container";

//-----------------------------------------------------------------------------

SPdfWriter::SPdfWriter()
{
}

//-----------------------------------------------------------------------------

void SPdfWriter::info(std::ostream& _sstream )
{
    this->::io::IWriter::info( _sstream );
    _sstream << std::endl << " External data file reader";
}

//-----------------------------------------------------------------------------

SPdfWriter::~SPdfWriter() noexcept
{
    this->stopping();
}

//------------------------------------------------------------------------------

void SPdfWriter::configuring()
{
    this->::io::IWriter::configuring();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    const ConfigurationType containersConfig = m_configuration->findConfigurationElement("container");
    if (containersConfig)
    {
        const std::vector< ConfigurationType > containersCfg = containersConfig->find(s_CONTAINER_INPUT);
        for (const auto& cfg : containersCfg)
        {
            SLM_ASSERT("Missing attribute 'uid'.", cfg->hasAttribute("uid"));
            const std::string id = cfg->getAttributeValue("uid");
            m_containersIDs.push_back( id );
        }
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an external data file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("pdf", "*.pdf");

    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::updating()
{
    if( !this->hasLocationDefined() )
    {
        configureWithIHM();
    }
    if( this->hasLocationDefined() )
    {
        QPdfWriter pdfWriter( this->getLocations().front().string().c_str() );
        QPainter painter( &pdfWriter );
        pdfWriter.setPageSize( QPagedPaintDevice::A4 );

        // Scale value to fit the images to a PDF page
        const int scale = static_cast<const int>(pdfWriter.logicalDpiX() * 8);
        ::fwThread::Pool& pool = ::fwThread::getDefaultPool();

        // Adding fwImage from generic scene to the list of images to scale
        ImagesScaledListType imagesToScale;
        std::vector< ::std::shared_future< QImage > > futuresQImage;
        for( const ::fwData::Image::sptr& fwImage : m_imagesToExport )
        {
            std::shared_future< QImage > future;
            future = pool.post(&SPdfWriter::convertFwImageToQImage, fwImage);
            futuresQImage.push_back( future );
        }
        std::for_each(futuresQImage.begin(), futuresQImage.end(), std::mem_fn(&::std::shared_future< QImage >::wait));
        for (auto& future : futuresQImage)
        {
            QImage imageToDraw = future.get();
            imagesToScale.push_back(imageToDraw);
        }

        // Adding QImage from Qt containers to the list of images to scale
        for( QWidget*& qtContainer : m_containersToExport )
        {
            QImage imageToDraw = qtContainer->grab().toImage();
            imagesToScale.push_back(imageToDraw);
        }

        // Scales images to fit the A4 format
        std::vector< ::std::shared_future< void > > futures;
        const size_t sizeImagesToScale = imagesToScale.size();
        for( size_t idx = 0; idx < sizeImagesToScale; ++idx )
        {
            std::shared_future<void> future;
            future = pool.post(&SPdfWriter::scaleQImage, std::ref(imagesToScale[idx]), scale);
            futures.push_back( future );
        }
        std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

        // Draws images onto the PDF.
        for( QImage& qImage : imagesToScale )
        {
            if ( pdfWriter.newPage() )
            {
                pdfWriter.setPageSize( QPagedPaintDevice::A4 );
                if ( !qImage.isNull() && qImage.bits() != nullptr )
                {
                    painter.drawImage( 0, 0, qImage);
                }
            }
        }
        painter.end();
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::starting()
{
    const size_t groupImageSize = this->getKeyGroupSize(s_IMAGE_INPUT);
    for (size_t idxImage = 0; idxImage < groupImageSize; ++idxImage)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INPUT, idxImage);
        m_imagesToExport.push_back(image);
    }

    for(const auto& id : m_containersIDs)
    {
        ::fwGuiQt::container::QtContainer::sptr containerElt;
        ::fwGui::container::fwContainer::sptr fwContainerFromConfig;
        if ( ::fwGui::GuiRegistry::hasSIDContainer( id ) )
        {
            fwContainerFromConfig = ::fwGui::GuiRegistry::getSIDContainer( id );
        }
        else
        {
            fwContainerFromConfig = ::fwGui::GuiRegistry::getWIDContainer( id );
        }
        if (fwContainerFromConfig)
        {
            containerElt = ::fwGuiQt::container::QtContainer::dynamicCast(fwContainerFromConfig);
            m_containersToExport.push_back(containerElt->getQtContainer());
        }
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::stopping()
{
    for( QWidget*& qtContainer : m_containersToExport )
    {
        qtContainer = nullptr;
    }
    m_containersToExport.clear();
    m_imagesToExport.clear();
}

//------------------------------------------------------------------------------

::io::IOPathType SPdfWriter::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SPdfWriter::scaleQImage(QImage& qImage, const int scale)
{
    SLM_ERROR_IF("Image is null.", qImage.isNull());
    qImage = qImage.scaledToWidth(scale, Qt::FastTransformation);
}

//------------------------------------------------------------------------------

QImage SPdfWriter::convertFwImageToQImage(::fwData::Image::sptr fwImage)
{
    if (fwImage->getNumberOfComponents() == 3
        && fwImage->getType().string() == "uint8"
        && fwImage->getSize()[2] == 1)
    {
        // Initialize QImage parameters
        const ::fwData::Image::SizeType dimension = fwImage->getSize();
        const int width                           = static_cast<int>(dimension[0]);
        const int height                          = static_cast<int>(dimension[1]);

        QImage qImage(width, height, QImage::Format_ARGB32);
        ::boost::uint8_t* qImageBuffer = qImage.bits();

        ::fwDataTools::helper::Image imgHelper(fwImage);
        const ::boost::uint8_t* fwImageBuffer = reinterpret_cast< const ::boost::uint8_t*>( imgHelper.getBuffer() );

        const unsigned int size = static_cast<unsigned int>( width * height) * 4;
        for(unsigned int idx = 0; idx < size; idx += 4)
        {
            qImageBuffer[idx+3] = (255 & 0xFF);
            qImageBuffer[idx+2] = (*fwImageBuffer++ & 0xFF);
            qImageBuffer[idx+1] = (*fwImageBuffer++ & 0xFF);
            qImageBuffer[idx+0] = (*fwImageBuffer++ & 0xFF);
        }
        return qImage.mirrored(0, 1);
    }
    return QImage();
}

//------------------------------------------------------------------------------

} // namespace ioQt
