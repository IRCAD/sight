/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwData/Image.hpp>

#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>

#include "opImageFilter/action/imageFilter.hpp"

namespace opImageFilter
{

namespace action
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::action::IAction , ::opImageFilter::action::imageFilter , ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

imageFilter::imageFilter() throw() :
	m_image1UID(""),
	m_image2UID("")
{}

//-----------------------------------------------------------------------------

imageFilter::~imageFilter() throw()
{}

//-----------------------------------------------------------------------------

void imageFilter::starting() throw ( ::fwTools::Failed )
{
	::gui::action::IAction::starting();
}

//-----------------------------------------------------------------------------

void imageFilter::stopping() throw ( ::fwTools::Failed )
{
	::gui::action::IAction::stopping();
}

//-----------------------------------------------------------------------------

void imageFilter::updating( fwServices::ObjectMsg::csptr _pMsg ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void imageFilter::configuring() throw ( ::fwTools::Failed )
{
	::gui::action::IAction::configuring();

	m_image1UID = m_configuration->findConfigurationElement("imageIn")->getExistingAttributeValue("uid");
	m_image2UID = m_configuration->findConfigurationElement("imageOut")->getExistingAttributeValue("uid");

	OSLM_INFO( "ImageIn UID = " << m_image1UID);
	OSLM_INFO( "ImageOut UID = " << m_image2UID);
}


struct ThresholdFilter
{
	struct Parameter
	{
		double thresholdValue;
		::boost::shared_ptr< ::fwData::Image > 	imageIn;
		::boost::shared_ptr< ::fwData::Image > 	imageOut;
	};

	template<class PIXELTYPE>
	void operator()(Parameter &param)
	{
		assert( param.imageIn->getSize().size()==3 );
		*param.imageOut = *param.imageIn; // La copie serait utile uniquement lors du changement de l'image d'origine
		// Il serait préféreable de copier uniquement les attributs de l'image et d'allouer le buffer sans le copier
		PIXELTYPE *buffer1 = (PIXELTYPE *)param.imageIn->getBuffer();
		PIXELTYPE *buffer2 = (PIXELTYPE *)param.imageOut->getBuffer();
		const unsigned int NbPixels = param.imageIn->getSize()[0] * param.imageIn->getSize()[1] * param.imageIn->getSize()[2];
		const PIXELTYPE ThresholdValue = ( PIXELTYPE )param.thresholdValue;
		unsigned int i;
		for( i=0 ; i<NbPixels ; ++i , ++buffer1, ++buffer2 )
			if ( *buffer1 < ThresholdValue ) *buffer2 = 0;
			else *buffer2 =  std::numeric_limits<PIXELTYPE>::max();
	}
};

//-----------------------------------------------------------------------------

void imageFilter::updating() throw ( ::fwTools::Failed )
{
	const double Threshold = 50.0;

	SLM_TRACE_FUNC();
	SLM_INFO("imageFilter updating");

	ThresholdFilter::Parameter param;
	OSLM_ASSERT("Image 1 not found. UID : " << m_image1UID, ::fwTools::UUID::exist(m_image1UID, ::fwTools::UUID::SIMPLE )) ;
	param.imageIn = ::fwTools::UUID::get< ::fwData::Image >( m_image1UID ) ;
	OSLM_ASSERT("Image 2 not found. UID : " << m_image2UID, ::fwTools::UUID::exist(m_image2UID, ::fwTools::UUID::SIMPLE )) ;
	param.imageOut = ::fwTools::UUID::get< ::fwData::Image >( m_image2UID ) ;
	param.thresholdValue = Threshold;
	
	::fwTools::DynamicType type = param.imageIn->getPixelType();
	fwTools::Dispatcher< fwTools::IntrinsicTypes , ThresholdFilter >::invoke( type , param );

	::fwComEd::ImageMsg::NewSptr msg;
	msg->addEvent( ::fwComEd::ImageMsg::NEW_IMAGE ) ;
//	msg->addEvent( ::fwComEd::ImageMsg::BUFFER ) ; // Ne fonctionne pas

	// Notify message to all service listeners
	::fwServices::IEditionService::notify(this->getSptr(), param.imageOut, msg);
}

//-----------------------------------------------------------------------------

void imageFilter::info ( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

}
}
