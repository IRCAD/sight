/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiImageQt/ImageInfo.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace uiImageQt
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiImageQt::ImageInfo, ::fwData::Image );

static const ::fwCom::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";

ImageInfo::ImageInfo() noexcept
{
    newSlot(s_GET_INTERACTION_SLOT, &ImageInfo::getInteraction, this);
}

//------------------------------------------------------------------------------

ImageInfo::~ImageInfo() noexcept
{
}

//------------------------------------------------------------------------------

void ImageInfo::starting()
{
    this->::fwGui::IGuiContainerSrv::create();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );

    QHBoxLayout* hLayout = new QHBoxLayout();

    QLabel* staticText = new QLabel( QObject::tr("intensity:"));
    hLayout->addWidget( staticText, 0, Qt::AlignVCenter );

    m_valueText = new QLineEdit();
    m_valueText->setReadOnly(true);
    hLayout->addWidget( m_valueText, 1, Qt::AlignVCenter );

    qtContainer->setLayout( hLayout );
}

//------------------------------------------------------------------------------

void ImageInfo::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void ImageInfo::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void ImageInfo::updating()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    if (!image)
    {
        FW_DEPRECATED_KEY(s_IMAGE_INPUT, "in", "fw4spl_18.0");
        image = this->getObject< ::fwData::Image >();
    }
    SLM_ASSERT("The input '" + s_IMAGE_INPUT + "' is not defined", image);
    const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    m_valueText->setEnabled(imageIsValid);
}

//------------------------------------------------------------------------------

void ImageInfo::getInteraction(::fwDataTools::PickingInfo info)
{
    if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_MOVE)
    {
        ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
        if (!image)
        {
            FW_DEPRECATED_KEY(s_IMAGE_INPUT, "in", "fw4spl_18.0");
            image = this->getObject< ::fwData::Image >();
        }
        SLM_ASSERT("The input '" + s_IMAGE_INPUT + "' is not defined", image);

        const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        m_valueText->setEnabled(imageIsValid);
        if (imageIsValid)
        {
            const double* point                  = info.m_worldPos;
            const ::fwData::Image::SizeType size = image->getSize();

            if (point[0] < 0 || point[1] < 0 || point[2] < 0)
            {
                SLM_ERROR("The received coordinates are not in the image space, maybe you use the wrong picker "
                          "interator (see ::visuVTKAdaptor::SImagePickerInteractor)");
                return;
            }

            const ::fwData::Image::SizeType coords =
            {{ static_cast< ::fwData::Image::SizeType::value_type >(point[0]),
               static_cast< ::fwData::Image::SizeType::value_type >(point[1]),
               static_cast< ::fwData::Image::SizeType::value_type >(point[2])}};

            bool isInside = (coords[0] < size[0] && coords[1] < size[1]);
            if (image->getNumberOfDimensions() < 3)
            {
                isInside = (isInside && coords[2] == 0);
            }
            else
            {
                isInside = (isInside && coords[2] < size[2]);
            }

            if (!isInside)
            {
                SLM_ERROR("The received coordinates are not in the image space, maybe you use the wrong picker "
                          "interator (see ::visuVTKAdaptor::SImagePickerInteractor)");
                return;
            }

            ::fwDataTools::helper::ImageGetter imageHelper(image);

            const std::string intensity = imageHelper.getPixelAsString(coords[0], coords[1], coords[2] );
            m_valueText->setText(QString::fromStdString(intensity));
        }
    }
}

//------------------------------------------------------------------------------

void ImageInfo::info( std::ostream& _sstream )
{
    _sstream << "Image Info Editor";
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap ImageInfo::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------
}
