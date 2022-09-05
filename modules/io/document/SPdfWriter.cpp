/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "SPdfWriter.hpp"

#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/ConfigurationElementContainer.hpp>
#include <core/thread/Pool.hpp>
#include <core/thread/Worker.hpp>

#include <data/iterator/types.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/GuiRegistry.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QPainter>
#include <QPixmap>

namespace sight::module::io::document
{

//-----------------------------------------------------------------------------

SPdfWriter::SPdfWriter()
= default;

//-----------------------------------------------------------------------------

void SPdfWriter::info(std::ostream& _sstream)
{
    this->IWriter::info(_sstream);
    _sstream << std::endl << " External data file reader";
}

//-----------------------------------------------------------------------------

SPdfWriter::~SPdfWriter() noexcept
{
    this->SPdfWriter::stopping();
}

//------------------------------------------------------------------------------

void SPdfWriter::configuring()
{
    static constexpr auto s_CONTAINER_INPUT = "container";
    this->IWriter::configuring();

    using ConfigurationType = core::runtime::ConfigurationElement::sptr;
    const ConfigurationType containersConfig = m_configuration->findConfigurationElement(s_CONTAINER_INPUT);
    if(containersConfig)
    {
        const std::vector<ConfigurationType> containersCfg = containersConfig->find(s_CONTAINER_INPUT);
        for(const auto& cfg : containersCfg)
        {
            SIGHT_ASSERT("Missing attribute 'uid'.", cfg->hasAttribute("uid"));
            const std::string id = cfg->getAttributeValue("uid");
            m_containersIDs.push_back(id);
        }
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an external data file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("pdf", "*.pdf");

    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->setFile(result->getFile());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::updating()
{
    if(!this->hasLocationDefined())
    {
        openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        QPdfWriter pdfWriter(this->getLocations().front().string().c_str());
        QPainter painter(&pdfWriter);
        pdfWriter.setPageSize(QPagedPaintDevice::A4);

        // Scale value to fit the images to a PDF page
        const int scale          = static_cast<int>(pdfWriter.logicalDpiX() * 8);
        core::thread::Pool& pool = core::thread::getDefaultPool();

        // Adding fwImage from generic scene to the list of images to scale
        ImagesScaledListType imagesToScale;
        std::vector<std::shared_future<QImage> > futuresQImage;

        {
            // Keep locked_ptr until tasks are done
            std::vector<data::mt::locked_ptr<const data::Image> > lockedImages;

            for(const auto& imagePtr : m_images)
            {
                std::shared_future<QImage> future;
                auto lockedImage = imagePtr.second.lock();
                future = pool.post(&SPdfWriter::convertFwImageToQImage, std::ref(*lockedImage));
                lockedImages.push_back(std::move(lockedImage));
                futuresQImage.push_back(future);
            }

            std::for_each(futuresQImage.begin(), futuresQImage.end(), std::mem_fn(&std::shared_future<QImage>::wait));
        }
        for(auto& future : futuresQImage)
        {
            QImage imageToDraw = future.get();
            imagesToScale.push_back(imageToDraw);
        }

        // Adding QImage from Qt containers to the list of images to scale
        for(QWidget*& qtContainer : m_containersToExport)
        {
            QImage imageToDraw = qtContainer->grab().toImage();
            imagesToScale.push_back(imageToDraw);
        }

        // Scales images to fit the A4 format
        std::vector<std::shared_future<void> > futures;
        const std::size_t sizeImagesToScale = imagesToScale.size();
        for(std::size_t idx = 0 ; idx < sizeImagesToScale ; ++idx)
        {
            std::shared_future<void> future;
            future = pool.post(&SPdfWriter::scaleQImage, std::ref(imagesToScale[idx]), scale);
            futures.push_back(future);
        }

        std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));

        // Draws images onto the PDF.
        for(QImage& qImage : imagesToScale)
        {
            if(pdfWriter.newPage())
            {
                pdfWriter.setPageSize(QPagedPaintDevice::A4);
                if(!qImage.isNull() && qImage.bits() != nullptr)
                {
                    painter.drawImage(0, 0, qImage);
                }
            }
        }

        painter.end();
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::starting()
{
    for(const auto& id : m_containersIDs)
    {
        ui::qt::container::QtContainer::sptr containerElt;
        sight::ui::base::container::fwContainer::sptr fwContainerFromConfig;
        if(sight::ui::base::GuiRegistry::hasSIDContainer(id))
        {
            fwContainerFromConfig = sight::ui::base::GuiRegistry::getSIDContainer(id);
        }
        else
        {
            fwContainerFromConfig = sight::ui::base::GuiRegistry::getWIDContainer(id);
        }

        if(fwContainerFromConfig)
        {
            containerElt = ui::qt::container::QtContainer::dynamicCast(fwContainerFromConfig);
            m_containersToExport.push_back(containerElt->getQtContainer());
        }
    }
}

//------------------------------------------------------------------------------

void SPdfWriter::stopping()
{
    for(QWidget*& qtContainer : m_containersToExport)
    {
        qtContainer = nullptr;
    }

    m_containersToExport.clear();
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SPdfWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SPdfWriter::scaleQImage(QImage& qImage, const int scale)
{
    SIGHT_ERROR_IF("Image is null.", qImage.isNull());
    qImage = qImage.scaledToWidth(scale, Qt::FastTransformation);
}

//------------------------------------------------------------------------------

QImage SPdfWriter::convertFwImageToQImage(const data::Image& fwImage)
{
    if(fwImage.numComponents() == 3
       && fwImage.getType().name() == "uint8"
       && fwImage.getSize()[2] == 1)
    {
        // Initialize QImage parameters
        const data::Image::Size dimension = fwImage.getSize();
        const int width                   = static_cast<int>(dimension[0]);
        const int height                  = static_cast<int>(dimension[1]);

        QImage qImage(width, height, QImage::Format_ARGB32);
        std::uint8_t* qImageBuffer = qImage.bits();

        const auto dumpLock = fwImage.dump_lock();

        auto imageItr = fwImage.begin<data::iterator::rgb>();

        const unsigned int size = static_cast<unsigned int>(width * height) * 4;
        for(unsigned int idx = 0 ; idx < size ; idx += 4, ++imageItr)
        {
            qImageBuffer[idx + 3] = (255 & 0xFF);
            qImageBuffer[idx + 2] = (imageItr->r & 0xFF);
            qImageBuffer[idx + 1] = (imageItr->g & 0xFF);
            qImageBuffer[idx + 0] = (imageItr->b & 0xFF);
        }

        return qImage.mirrored(false, true);
    }

    return {};
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::document
