/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/MedicalImageAdaptor.hpp"

#include "fwCore/macros.hpp"

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"
#include "fwDataTools/helper/Composite.hpp"
#include "fwDataTools/helper/Image.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransferFunction.hpp>

namespace fwDataTools
{

namespace helper
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_POINTS_SLOT    = "updateTFPoints";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_WINDOWING_SLOT = "updateTFWindowing";

//------------------------------------------------------------------------------

MedicalImageAdaptor::MedicalImageAdaptor() :
    m_orientation(Z_AXIS)
{
    FW_DEPRECATED_MSG(
        "fwDataTools::helper::MedicalImageAdaptor is deprecated, please use fwDataTools::helper::MedicalImage or fwDataTools::helper::TransferFunction instead",
        19.0);
}

//------------------------------------------------------------------------------

MedicalImageAdaptor::~MedicalImageAdaptor()
{
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSpacing(double spacing[3]) const
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SpacingType& imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageOrigin(double origin[3]) const
{
    ::fwData::Image::sptr image = this->getImage();

    std::copy(image->getOrigin().begin(), image->getOrigin().end(), origin);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageDataSize(int size[3]) const
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSize(double size[3]) const
{
    ::fwData::Image::sptr image = this->getImage();
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
    ::fwData::Image::sptr image = this->getImage();
    double imageSize[3];
    this->getImageSize(imageSize);
    double origin[3];
    this->getImageOrigin(origin);

    ::fwData::Integer::sptr sliceIndex[3];
    this->getSliceIndex(sliceIndex);
    double index[3] = {
        static_cast<double>(sliceIndex[0]->value()),
        static_cast<double>(sliceIndex[1]->value()),
        static_cast<double>(sliceIndex[2]->value())
    };

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
    OSLM_ASSERT("orientation value must be  0,1 or 2 (value = " << orientation << ")",
                orientation == 0 || orientation == 1 || orientation == 2);
    this->setOrientation(static_cast< ::fwDataTools::helper::MedicalImageAdaptor::Orientation >(orientation));
}

//------------------------------------------------------------------------------

static const int indexZ[12]   = { 0, 2, 4, 1, 2, 4,  1, 3, 4, 0, 3, 4 };
static const int indexY[12]   = { 0, 2, 4, 1, 2, 4,  1, 2, 5, 0, 2, 5 };
static const int indexX[12]   = { 0, 2, 4, 0, 2, 5,  0, 3, 5, 0, 3, 4 };
static const int* indexSet[3] = { indexX, indexY, indexZ  };
//------------------------------------------------------------------------------

void MedicalImageAdaptor::getPlane( double points[4][3], int sliceNumber)
{
    ::fwData::Image::sptr image = this->getImage();
    double extent[6];
    for (unsigned char i = 0; i < 3; ++i )
    {
        extent[2*i]   = 0;
        extent[2*i+1] = image->getSize()[i]*image->getSpacing()[i];
    }
    extent[2*m_orientation]   = sliceNumber*image->getSpacing()[m_orientation];
    extent[2*m_orientation+1] = sliceNumber*image->getSpacing()[m_orientation];

    const int* extentIndex = indexSet[ m_orientation ];
    for (int p = 0; p < 4; ++p)
    {
        for (int i = 0; i < 3; ++i)
        {
            points[p][i] = extent[ *(extentIndex++) ];
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
    for ( int i = 0; i < 3; ++i )
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
    for ( int i = 0; i < 3; ++i )
    {
        // nearest integer
        index[i] =
            static_cast<int>( ( (world[i]-origin[i])/spacing[i] ) +
                              ( ( (world[i]-origin[i])/spacing[i] ) >= 0 ? 0.5 : -0.5 ) );
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

    ::fwData::Integer::sptr sliceIndex[3];

    this->getSliceIndex(sliceIndex);

    if(    index[0] != m_sagittalIndex->value()
           || index[1] != m_frontalIndex->value()
           || index[2] != m_axialIndex->value() )
    {
        m_sagittalIndex->value() = index[0];
        m_frontalIndex->value()  = index[1];
        m_axialIndex->value()    = index[2];
        isModified               = true;
    }
    return isModified;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateImageInfos( ::fwData::Image::sptr image )
{
    m_weakImage  = image;
    m_axialIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId,
                                          ::fwData::Integer::New(0));
    m_frontalIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId,
                                            ::fwData::Integer::New(0));
    m_sagittalIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId,
                                             ::fwData::Integer::New(0));
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::createTransferFunction( ::fwData::Image::sptr image )
{
    ::fwData::Composite::sptr tfPool =
        image->setDefaultField(::fwDataTools::fieldHelper::Image::m_transferFunctionCompositeId,
                               ::fwData::Composite::New());

    // create the default transfer function in the image tf field if it does not exist
    if (tfPool->find(::fwData::TransferFunction::s_DEFAULT_TF_NAME) == tfPool->end())
    {
        ::fwData::TransferFunction::sptr tfGreyLevel = ::fwData::TransferFunction::createDefaultTF();
        if (image->getWindowWidth() != 0 )
        {
            tfGreyLevel->setWindow( image->getWindowWidth() );
            tfGreyLevel->setLevel( image->getWindowCenter() );
        }
        else if(::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity(image))
        {
            double min, max;
            ::fwDataTools::fieldHelper::MedicalImageHelpers::getMinMax(image, min, max);
            ::fwData::TransferFunction::TFValuePairType wlMinMax(min, max);
            tfGreyLevel->setWLMinMax(wlMinMax);
        }

        ::fwDataTools::helper::Composite compositeHelper(tfPool);
        compositeHelper.add(::fwData::TransferFunction::s_DEFAULT_TF_NAME, tfGreyLevel);
        compositeHelper.notify();
    }

    if (m_transferFunction.expired())
    {
        ::fwData::TransferFunction::sptr tfGreyLevel =
            tfPool->at< ::fwData::TransferFunction >(::fwData::TransferFunction::s_DEFAULT_TF_NAME);
        m_transferFunction = tfGreyLevel;
    }
    else if (m_transferFunction.lock()->getTFValues().empty())
    {
        ::fwData::TransferFunction::sptr tfGreyLevel =
            tfPool->at< ::fwData::TransferFunction >(::fwData::TransferFunction::s_DEFAULT_TF_NAME);
        m_transferFunction.lock()->deepCopy(tfGreyLevel);
    }
}

//------------------------------------------------------------------------------v

void MedicalImageAdaptor::setOrCreateTF(const fwData::TransferFunction::sptr& _tf, const fwData::Image::sptr& _image)
{
    this->removeTFConnections();
    if (_tf)
    {
        ::fwData::mt::ObjectWriteLock tfLock(_tf);
        this->setTransferFunction(_tf);
    }
    else
    {
        ::fwData::mt::ObjectWriteLock tfLock(_image);
        this->createTransferFunction(_image);
    }
    this->installTFConnections();
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr MedicalImageAdaptor::getTransferFunction() const
{
    SLM_ASSERT("Transfer funtion is not defined, you must call setTransferFunction() or createTransferFunction() first."
               , !m_transferFunction.expired());
    return m_transferFunction.lock();
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImageAdaptor::getImage() const
{
    SLM_ASSERT("Image weak pointer empty !", !m_weakImage.expired());
    return m_weakImage.lock();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setTransferFunction(const ::fwData::TransferFunction::sptr& tf )
{
    m_transferFunction = tf;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::installTFConnections()
{
    ::fwCom::Connection connection;

    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
    {
        ::fwData::mt::ObjectWriteLock tfLock(tf);
        connection = tf->signal(::fwData::TransferFunction::s_POINTS_MODIFIED_SIG)->connect(m_slotUpdateTFPoints);
        m_tfConnections.addConnection(connection);
        connection = tf->signal(::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG)->connect(m_slotUpdateTFWindowing);
        m_tfConnections.addConnection(connection);
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::removeTFConnections()
{
    m_tfConnections.disconnect();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::installTFSlots(::fwCom::HasSlots* hasslots)
{
    m_slotUpdateTFPoints    = hasslots->newSlot(s_UPDATE_TF_POINTS_SLOT, &MedicalImageAdaptor::updateTFPoints, this);
    m_slotUpdateTFWindowing =
        hasslots->newSlot(s_UPDATE_TF_WINDOWING_SLOT, &MedicalImageAdaptor::updateTFWindowing, this);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateTFPoints()
{
    SLM_ASSERT("This methods (updateTFPoints) must be reimplemented in subclass to manage TF modifications", false);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateTFWindowing(double /*window*/, double /*level*/)
{
    SLM_ASSERT("This methods (updateTFWindowing) must be reimplemented in subclass to manage TF modifications",
               false);
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwDataTools
