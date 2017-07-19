/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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
#include <fwDataTools/helper/Image.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace uiImage
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiImage::ImageInfo, ::fwData::Image );

static const ::fwCom::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

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
    SLM_TRACE_FUNC();
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
    SLM_TRACE_FUNC();

    this->destroy();
}

//------------------------------------------------------------------------------

void ImageInfo::configuring()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void ImageInfo::updating()
{
}

//------------------------------------------------------------------------------

void ImageInfo::swapping()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    m_valueText->setEnabled(imageIsValid);
}

//------------------------------------------------------------------------------

void ImageInfo::getInteraction(::fwDataTools::PickingInfo info)
{
    SLM_TRACE_FUNC();

    if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_MOVE)
    {
        ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
        bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        m_valueText->setEnabled(imageIsValid);
        if (imageIsValid)
        {
            double* point = info.m_worldPos;

            ::fwDataTools::helper::Image imageHelper(image);

            std::string intensity = imageHelper.getPixelAsString(point[0], point[1], point[2] );
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
}

