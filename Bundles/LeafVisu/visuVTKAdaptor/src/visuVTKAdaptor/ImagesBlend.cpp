/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkImageBlend.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkLookupTable.h>

#include <fwTools/helpers.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwServices/Base.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/TransfertFunction.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <vtkIO/vtk.hpp>

#include "visuVTKAdaptor/Image.hpp"
#include "visuVTKAdaptor/ImagesBlend.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ImagesBlend, ::fwData::Composite ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

ImagesBlend::ImagesBlend() throw()
{
    SLM_TRACE_FUNC();

    // Manage events
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER                     );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE                  );
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS           );
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS         );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS         );
}

//------------------------------------------------------------------------------

ImagesBlend::~ImagesBlend() throw()
{
    SLM_TRACE_FUNC();

    m_imageBlend = NULL;
}

//------------------------------------------------------------------------------

void ImagesBlend::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Image register is empty", !m_imageRegisterId.empty());
    m_imageBlend = vtkImageBlend::SafeDownCast(this->getVtkObject(m_imageRegisterId));

    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    doUpdate();
}

//------------------------------------------------------------------------------

void ImagesBlend::doUpdate() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->removeImageAdaptors();
    this->addImageAdaptors();
}

//------------------------------------------------------------------------------

void ImagesBlend::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(msg);
    ::fwComEd::ImageMsg::csptr imageMsg = ::fwComEd::ImageMsg::dynamicConstCast(msg);

    if (compositeMsg)
    {
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::ADDED_FIELDS)
                || compositeMsg->hasEvent(::fwComEd::CompositeMsg::REMOVED_FIELDS)
                || compositeMsg->hasEvent(::fwComEd::CompositeMsg::SWAPPED_FIELDS))
        {
            doUpdate();
        }
    }
    else if (imageMsg)
    {
        SLM_ASSERT("msg subject is expired", !imageMsg->getSubject().expired() )
        ::fwData::Image::sptr image = ::fwData::Image::dynamicCast(imageMsg->getSubject().lock());

        if ( imageMsg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            if (! ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity(image)
                || m_registeredImages.find(image->getID()) == m_registeredImages.end())
            {
                doUpdate();
            }
        }
    }
}

//------------------------------------------------------------------------------

void ImagesBlend::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");

    if(m_configuration->hasAttribute("vtkimageregister") )
    {
        this->setVtkImageRegisterId( m_configuration->getAttributeValue("vtkimageregister") );
    }

    std::vector< ::fwRuntime::ConfigurationElement::sptr > configs = m_configuration->find("image");
    SLM_ASSERT("Missing tag 'image' ", !configs.empty());
    BOOST_FOREACH(::fwRuntime::ConfigurationElement::sptr element, configs)
    {
        SPTR(ImageInfo) info = ::boost::shared_ptr< ImageInfo >(new ImageInfo());
        SLM_ASSERT("Missing attribute 'objectId'", element->hasAttribute("objectId"));
        std::string objectId = element->getAttributeValue("objectId");

        if(element->hasAttribute("opacity") )
        {
            info->m_imageOpacity = ::boost::lexical_cast<double>(element->getAttributeValue("opacity"));
        }
        if(element->hasAttribute("tfalpha") )
        {
            info->m_useTFAlfa = element->getAttributeValue("tfalpha") == "yes";
        }
        if(element->hasAttribute("useColorTF") )
        {
            info->m_useImageTF = element->getAttributeValue("useColorTF") == "yes";
        }

        info->parseTFConfig( element );

        typedef std::pair< std::string, std::string > ImagesIdPair;
        m_imageIds.push_back(objectId);
        m_imagesInfo[objectId] = info;
    }
}

//------------------------------------------------------------------------------

bool ImagesBlend::checkImageInformations()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::Image::SizeType size;
    ::fwData::Image::SpacingType spacing;
    ::fwData::Image::OriginType origin;

    bool haveSameInfo = true;

    BOOST_FOREACH(std::string id, m_imageIds)
    {
        if (composite->find(id) != composite->end())
        {
            ::fwData::Image::sptr img = ::fwData::Image::dynamicCast((*composite)[id]);

            bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
            if (imageIsValid)
            {
                if (size.empty() && spacing.empty() && origin.empty())
                {
                    size = img->getSize();
                    spacing = img->getSpacing();
                    origin = img->getOrigin();
                }
                else
                {
                    if (size != img->getSize() || spacing != img->getSpacing() || origin != img->getOrigin())
                    {
                        haveSameInfo = false;
                        std::string errorMsg = "Warning : images in blend have not the same";
                        errorMsg += (size != img->getSize())?" size":"";
                        errorMsg += (spacing != img->getSpacing())?" spacing":"";
                        errorMsg += (origin != img->getOrigin())?" origin":"";
                        errorMsg += ".\n Background image size, spacing and origin are use.";
                        ::fwGui::dialog::MessageDialog::showMessageDialog("Images blending",
                                errorMsg,
                                ::fwGui::dialog::MessageDialog::WARNING);
                        break;
                    }
                }
            }
        }
    }

    return haveSameInfo;
}

//------------------------------------------------------------------------------

void ImagesBlend::addImageAdaptors()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    this->checkImageInformations();

    BOOST_FOREACH(std::string id, m_imageIds)
    {
        if (composite->find(id) != composite->end())
        {
            ::fwData::Image::sptr img = ::fwData::Image::dynamicCast((*composite)[id]);

            SPTR(ImageInfo) info = m_imagesInfo[id];

            SLM_ASSERT("Sorry, '" << id << "' is not an image", img);

            bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( img );
            if (imageIsValid)
            {
                ::fwRenderVTK::IVtkAdaptorService::sptr imageAdaptor;
                imageAdaptor = ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >( img, "::visuVTKAdaptor::Image");
                imageAdaptor->setRenderService(this->getRenderService());
                imageAdaptor->setRenderId( this->getRenderId() );
                imageAdaptor->setPickerId( this->getPickerId() );
                imageAdaptor->setTransformId( this->getTransformId() );

                ::visuVTKAdaptor::Image::sptr IA;
                IA = ::visuVTKAdaptor::Image::dynamicCast(imageAdaptor);
                IA->setUseImageTF(info->m_useImageTF );
                IA->setVtkImageRegister(m_imageBlend);
                IA->setImageOpacity(info->m_imageOpacity);
                IA->setAllowAlphaInTF(info->m_useTFAlfa);
                IA->setSelectedTFKey( info->getSelectedTFKey() );
                IA->setTFPoolFwID( info->getTFPoolFwID() );

                m_registeredImages[img->getID()] = imageAdaptor;
                this->registerService(imageAdaptor);

                imageAdaptor->start();
            }

            info->m_comChannel = ::fwServices::registerCommunicationChannel(img, this->getSptr());
            ::fwServices::ComChannelService::dynamicCast(info->m_comChannel.lock())->setPriority(0.56);
            info->m_comChannel.lock()->start();
        }
    }
}

//------------------------------------------------------------------------------

void ImagesBlend::removeImageAdaptors()
{
    BOOST_REVERSE_FOREACH(std::string id, m_imageIds)
    {
        SPTR(ImageInfo) info = m_imagesInfo[id];

        if ( !info->m_comChannel.expired())
        {
            info->m_comChannel.lock()->stop();
            ::fwServices::OSR::unregisterService(info->m_comChannel.lock());
            info->m_comChannel.reset();
        }
    }
    this->unregisterServices();
}

//------------------------------------------------------------------------------


} //namespace visuVTKAdaptor
