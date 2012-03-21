/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Image.hpp>
#include <fwData/TransfertFunction.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/CompositeMsg.hpp>

#include "fwComEd/helper/MedicalImageAdaptor.hpp"
#include "fwComEd/helper/Image.hpp"

namespace fwComEd
{

namespace helper
{

//------------------------------------------------------------------------------

MedicalImageAdaptor::MedicalImageAdaptor()
    : m_orientation(Z_AXIS),
      m_tfPoolFwID(""),
      m_selectedTFKey("")
{}

//------------------------------------------------------------------------------

MedicalImageAdaptor::~MedicalImageAdaptor()
{}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSpacing(double spacing[3])
{
    ::fwData::Image::sptr image = this->getImage();;

    const ::fwData::Image::SpacingType& imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageOrigin(double origin[3])
{
    ::fwData::Image::sptr image = this->getImage();;

    std::copy(image->getOrigin().begin(), image->getOrigin().end(), origin);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageDataSize(int size[3])
{
    ::fwData::Image::sptr image = this->getImage();;

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSize(double size[3])
{
    ::fwData::Image::sptr image = this->getImage();;
    double spacing[3];

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
    this->getImageSpacing(spacing);

    size[0] *= spacing[0];
    size[1] *= spacing[1];
    size[2] *= spacing[2];
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getCurrentSliceCenter(double center[3])
{
    ::fwData::Image::sptr image = this->getImage();;
    double imageSize[3];
    this->getImageSize(imageSize);
    double origin[3];
    this->getImageOrigin(origin);

    ::fwData::Integer::sptr sliceIndex[3];
    this->getSliceIndex(sliceIndex);
    double index[3] = {sliceIndex[0]->value(), sliceIndex[1]->value(), sliceIndex[2]->value()};

    center[0] = origin[0] + (imageSize[0]-1.)/ 2.;
    center[1] = origin[1] + (imageSize[1]-1.)/ 2.;
    center[2] = origin[2] + (imageSize[2]-1.)/ 2.;

    double spacing[3];
    this->getImageSpacing(spacing);
    center[m_orientation] = origin[m_orientation] + index[m_orientation]*spacing[m_orientation];
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setOrientation( MedicalImageAdaptor::Orientation orientation )
{
    m_orientation = orientation;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setOrientation( int orientation )
{
    OSLM_FATAL_IF(" orientation value must be  0,1 or 2 (value=" << orientation << ")" , orientation<0 || orientation>3);
    m_orientation = static_cast< ::fwComEd::helper::MedicalImageAdaptor::Orientation >(orientation);
}

//------------------------------------------------------------------------------

static const int indexZ[12] = { 0,2,4, 1,2,4,  1,3,4 ,0,3,4 };
static const int indexY[12] = { 0,2,4, 1,2,4,  1,2,5 ,0,2,5 };
static const int indexX[12] = { 0,2,4, 0,2,5,  0,3,5 ,0,3,4 };
static const int *indexSet[3] = { indexX, indexY, indexZ  };
void MedicalImageAdaptor::getPlane( double points[4][3] , int sliceNumber)
{
    ::fwData::Image::sptr image = this->getImage();;
    double extent[6];
    for (char i=0;  i<3; ++i )
    {
        extent[2*i]   =  0;
        extent[2*i+1] = image->getSize()[i]*image->getSpacing()[i];
    }
    extent[2*m_orientation] = sliceNumber*image->getSpacing()[m_orientation];
    extent[2*m_orientation+1] = sliceNumber*image->getSpacing()[m_orientation];

    const int *extentIndex = indexSet[ m_orientation ];
    for (int p=0; p<4 ; ++p)
    {
        for (int i=0; i<3 ; ++i)
        {
            points[p][i]= extent[ *(extentIndex++) ];
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::sliceIndexToWorld(const int index[3], double world[3] )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i=0 ; i<3 ; ++i )
    {
        world[i] = static_cast<int>( (index[i]*spacing[i]) + 0.5*spacing[i] + origin[i] );
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::worldToSliceIndex(const double world[3], int index[3] )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i=0 ; i<3 ; ++i )
    {
        // nearest integer
        index[i] = static_cast<int>( ( (world[i]-origin[i])/spacing[i] ) + ( ( (world[i]-origin[i])/spacing[i] ) >= 0 ? 0.5 : -0.5 ) );
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::worldToImageSliceIndex(const double world[3], int index[3] )
{
    int imageSize[3];
    this->getImageDataSize(imageSize);
    this->worldToSliceIndex(world, index);

    int idval;
    for (int i = 0; i < 3; i++)
    {
        int max = imageSize[i]-1;
        idval = index[i];
        if (idval < 0)
        {
            index[i] = 0;
        }
        else if (idval > max)
        {
            index[i] = max;
        }
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getSliceIndex(::fwData::Integer::sptr index[3])
{
    index[0] = m_sagittalIndex;
    index[1] = m_frontalIndex;
    index[2] = m_axialIndex;
}

//------------------------------------------------------------------------------

bool MedicalImageAdaptor::setSliceIndex(const int index[3])
{
    bool isModified = false;
    ::fwData::Image::sptr image = this->getImage();;

    ::fwData::Integer::sptr sliceIndex[3];

    this->getSliceIndex(sliceIndex);

    if(    index[0] != sliceIndex[0]->value()
        || index[1] != sliceIndex[1]->value()
        || index[2] != sliceIndex[2]->value() )
    {
        sliceIndex[0]->value() = index[0];
        sliceIndex[1]->value() = index[1];
        sliceIndex[2]->value() = index[2];
        isModified = true;
    }
    return isModified;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateImageInfos( ::fwData::Image::sptr image  )
{
    m_weakImage = image;
    m_axialIndex    = image->setDefaultField_NEWAPI(::fwComEd::Dictionary::m_axialSliceIndexId   , ::fwData::Integer::New(0));
    m_frontalIndex  = image->setDefaultField_NEWAPI(::fwComEd::Dictionary::m_frontalSliceIndexId , ::fwData::Integer::New(0));
    m_sagittalIndex = image->setDefaultField_NEWAPI(::fwComEd::Dictionary::m_sagittalSliceIndexId, ::fwData::Integer::New(0));

    // Set TF data if not still set
    if ( m_tfPool.expired() )
    {
        if ( ! m_tfPoolFwID.empty() )
        {
            ::fwData::Composite::sptr pool = ::fwData::Composite::dynamicCast( ::fwTools::fwID::getObject( m_tfPoolFwID ) );
            OSLM_ASSERT( "Sorry, object with fwID " << m_tfPoolFwID << " doesn't exist.", pool );
            OSLM_ASSERT( "Sorry, selectedTFKey must be defined, check your configuration.", ! m_selectedTFKey.empty() );
            if ( pool->find( m_selectedTFKey ) == pool->end() )
            {
                ::fwData::TransferFunction::NewSptr tf;
                tf->setLevel(100);
                tf->setWindow(200);
                tf->setName( m_selectedTFKey );
                tf->setInterpolationMode( ::fwData::TransferFunction::NEAREST );
                tf->setIsClamped(true);
                tf->addTFColor( 0   , ::fwData::TransferFunction::TFColor(1.0,0.0,0.0,0.0));
                tf->addTFColor( 1   , ::fwData::TransferFunction::TFColor(0.0,1.0,0.0,1.0));
                tf->addTFColor( 2   , ::fwData::TransferFunction::TFColor(0.0,0.0,1.0,1.0));
                tf->addTFColor(200  , ::fwData::TransferFunction::TFColor(1.0,1.0,0.0,1.0));
                (*pool)[m_selectedTFKey] = tf;
            }
            m_tfPool = pool;
        }
        else
        {
            ::fwComEd::helper::Image helper ( image );
            helper.createTransferFunctionPool(); // do nothing if image tf pool already exist

            m_selectedTFKey = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;
            m_tfPool = image->getField_NEWAPI< ::fwData::Composite >( ::fwComEd::Dictionary::m_transfertFunctionCompositeId );
        }
    }
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr MedicalImageAdaptor::getTransferFunctionPool() const
{
    return m_tfPool.lock();
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr MedicalImageAdaptor::getTransferFunction() const
{
    return ::fwData::TransferFunction::dynamicCast((*m_tfPool.lock())[m_selectedTFKey]);
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImageAdaptor::getImage()
{
    SLM_ASSERT("Image weak pointer empty !", !m_weakImage.expired());
    return m_weakImage.lock();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setTFParameters( ::fwData::Composite::sptr tfPool, std::string tfSelectionId )
{
    if (!tfSelectionId.empty())
    {
        m_selectedTFKey = tfSelectionId;
        m_tfPool = tfPool;
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setTFPoolFwID( const std::string & fwid )
{
    m_tfPoolFwID = fwid;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setSelectedTFKey( const std::string & key )
{
    m_selectedTFKey = key;
}

//------------------------------------------------------------------------------

const std::string & MedicalImageAdaptor::getTFPoolFwID() const
{
    return m_tfPoolFwID;
}

//------------------------------------------------------------------------------

const std::string & MedicalImageAdaptor::getSelectedTFKey() const
{
    return m_selectedTFKey;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::parseTFConfig( ::fwRuntime::ConfigurationElement::sptr configuration )
{
   SLM_ASSERT("Sorry, analyzed configuration is not conformed.", configuration->getName() == "config");
   if ( configuration->hasAttribute("selectedTFKey") )
   {
       m_selectedTFKey = configuration->getAttributeValue("selectedTFKey");
       SLM_FATAL_IF("'selectedTFKey' must not be empty", m_selectedTFKey.empty());
   }
   if ( configuration->hasAttribute("tfPoolFwID") )
   {
       m_tfPoolFwID = configuration->getAttributeValue("tfPoolFwID");
       SLM_FATAL_IF("'tfPoolFwID' must not be empty", m_tfPoolFwID.empty());
   }
}

//------------------------------------------------------------------------------

double MedicalImageAdaptor::getWindow() const
{
    return this->getTransferFunction()->getWindow();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setWindow( double window )
{
    this->getTransferFunction()->setWindow( window );
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setWindowLevel( double windowMin, double windowMax )
{
    double window = windowMax - windowMin;
    double level = windowMin + window/2.f;
    this->setWindow( window );
    this->setLevel( level );
}

//------------------------------------------------------------------------------

double MedicalImageAdaptor::getLevel() const
{
    return this->getTransferFunction()->getLevel();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setLevel( double level )
{
    this->getTransferFunction()->setLevel( level );
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::installTFPoolEventHandler( ::fwServices::IService* srv )
{
   srv->addNewHandledEvent(::fwComEd::CompositeMsg::CHANGED_KEYS);
   srv->addNewHandledEvent(::fwComEd::CompositeMsg::ADDED_KEYS);
   srv->addNewHandledEvent(::fwComEd::CompositeMsg::REMOVED_KEYS);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::installTFObserver( ::fwServices::IService::sptr srv )
{
   SLM_ASSERT( "Sorry TF pool observer already exist", m_tfPoolComChannelSrv.expired() );
   SLM_ASSERT( "Sorry TF observer already exist", m_tfComChannelSrv.expired() );


   ::fwServices::IService::sptr comChannel;
   comChannel = ::fwServices::registerCommunicationChannel( this->getTransferFunctionPool(), srv );
   comChannel->start();
   m_tfPoolComChannelSrv = comChannel;

   comChannel = ::fwServices::registerCommunicationChannel( this->getTransferFunction(), srv );
   comChannel->start();
   m_tfComChannelSrv = comChannel;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::removeTFObserver()
{
   SLM_ASSERT( "Sorry, TF pool observer must exist", ! m_tfPoolComChannelSrv.expired() );
   SLM_ASSERT( "Sorry, TF observer must exist", ! m_tfComChannelSrv.expired() );

   m_tfPoolComChannelSrv.lock()->stop();
   ::fwServices::OSR::unregisterService( m_tfPoolComChannelSrv.lock() );

   m_tfComChannelSrv.lock()->stop();
   ::fwServices::OSR::unregisterService( m_tfComChannelSrv.lock() );
}

//------------------------------------------------------------------------------

bool MedicalImageAdaptor::upadteTFObserver(::fwServices::ObjectMsg::csptr msg)
{
    bool needUpdate = false;
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(msg);
    if(compositeMsg)
    {
//        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) )
//        {
//        }
//
//        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS ) )
//        {
//        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::CHANGED_KEYS ) )
        {
            SLM_ASSERT( "Sorry, TF observer must exist", ! m_tfComChannelSrv.expired() );
            ::fwData::Composite::sptr fields = compositeMsg->getNewChangedKeys();
            ::fwData::Composite::iterator iter = fields->find(this->getSelectedTFKey());
            if( iter != fields->end())
            {
                ::fwServices::IService::sptr tfComChannel = m_tfComChannelSrv.lock();
                tfComChannel->swap(iter->second);
                needUpdate = true;
            }
        }
    }
    return needUpdate;
}

//------------------------------------------------------------------------------

::fwComEd::TransferFunctionMsg::sptr MedicalImageAdaptor::notifyTFWindowing( ::fwServices::IService::sptr srv )
{
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    // Fire the message
    ::fwComEd::TransferFunctionMsg::NewSptr msg;
    msg->setWindowLevel( tf->getWindow(), tf->getLevel() );
    ::fwServices::IEditionService::notify( srv, tf, msg );
    return msg;
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwComEd
