/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataTools/helper/MedicalImageAdaptor.hpp"

#include "fwDataTools/fieldHelper/Image.hpp"
#include "fwDataTools/fieldHelper/MedicalImageHelpers.hpp"
#include "fwDataTools/helper/Composite.hpp"
#include "fwDataTools/helper/Image.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/TransferFunction.hpp>

namespace fwDataTools
{

namespace helper
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT         = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT      = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT      = "removeObjects";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_POINTS_SLOT    = "updateTFPoints";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_WINDOWING_SLOT = "updateTFWindowing";

//------------------------------------------------------------------------------

MedicalImageAdaptor::MedicalImageAdaptor() :
    m_orientation(Z_AXIS),
    m_selectedTFKey("")
{
}

//------------------------------------------------------------------------------

MedicalImageAdaptor::~MedicalImageAdaptor()
{
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSpacing(double spacing[3])
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SpacingType& imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageOrigin(double origin[3])
{
    ::fwData::Image::sptr image = this->getImage();

    std::copy(image->getOrigin().begin(), image->getOrigin().end(), origin);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageDataSize(int size[3])
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SizeType& imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::getImageSize(double size[3])
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
    for (char i = 0; i < 3; ++i )
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
    ::fwData::Image::sptr image = this->getImage();

    ::fwData::Integer::sptr sliceIndex[3];

    this->getSliceIndex(sliceIndex);

    if(    index[0] != sliceIndex[0]->value()
           || index[1] != sliceIndex[1]->value()
           || index[2] != sliceIndex[2]->value() )
    {
        sliceIndex[0]->value() = index[0];
        sliceIndex[1]->value() = index[1];
        sliceIndex[2]->value() = index[2];
        isModified             = true;
    }
    return isModified;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateImageInfos( ::fwData::Image::sptr image )
{
    m_weakImage  = image;
    m_axialIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_axialSliceIndexId, ::fwData::Integer::New(
                                              0));
    m_frontalIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_frontalSliceIndexId, ::fwData::Integer::New(
                                                0));
    m_sagittalIndex = image->setDefaultField(::fwDataTools::fieldHelper::Image::m_sagittalSliceIndexId, ::fwData::Integer::New(
                                                 0));
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateTransferFunction( ::fwData::Image::sptr image )
{
    if ( !m_tfSelection.expired() )
    {
        ::fwData::Composite::sptr tfSelection = m_tfSelection.lock();

        SLM_ASSERT( "The selectedTFKey must be defined, check your configuration.", !m_selectedTFKey.empty() );
        if ( tfSelection->find( m_selectedTFKey ) == tfSelection->end() )
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

            ::fwDataTools::helper::Composite compositeHelper(tfSelection);
            compositeHelper.add(m_selectedTFKey, tfGreyLevel);
            compositeHelper.notify();
        }
        m_tfSelection = tfSelection;
    }
    else
    {
        const std::string poolFieldName = ::fwDataTools::fieldHelper::Image::m_transferFunctionCompositeId;
        ::fwData::Composite::sptr tfSelection = image->getField< ::fwData::Composite >(poolFieldName);

        if ( m_tfSelection.expired() || m_tfSelection.lock() != tfSelection )
        {
            const std::string defaultTFName = ::fwData::TransferFunction::s_DEFAULT_TF_NAME;

            ::fwDataTools::helper::Image helper(image);
            helper.createTransferFunctionPool(); // do nothing if image tf pool already exist

            tfSelection = image->getField< ::fwData::Composite >(poolFieldName);

            m_selectedTFKey = defaultTFName;
            m_tfSelection   = tfSelection;
        }
    }
}

//------------------------------------------------------------------------------

::fwData::Composite::sptr MedicalImageAdaptor::getTransferFunctionSelection() const
{
    return m_tfSelection.lock();
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::sptr MedicalImageAdaptor::getTransferFunction() const
{
    return ::fwData::TransferFunction::dynamicCast((*m_tfSelection.lock())[m_selectedTFKey]);
}

//------------------------------------------------------------------------------

::fwData::Image::sptr MedicalImageAdaptor::getImage()
{
    SLM_ASSERT("Image weak pointer empty !", !m_weakImage.expired());
    return m_weakImage.lock();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setSelectedTFKey( const std::string& key )
{
    m_selectedTFKey = key;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::setTransferFunctionSelection(fwData::Composite::wptr selection)
{
    m_tfSelection = selection;
}

//------------------------------------------------------------------------------

const std::string& MedicalImageAdaptor::getSelectedTFKey() const
{
    return m_selectedTFKey;
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::parseTFConfig( ::fwRuntime::ConfigurationElement::sptr configuration )
{
    SLM_ASSERT("The analyzed configuration is not conformed.", configuration->getName() == "config");
    if ( configuration->hasAttribute("selectedTFKey") )
    {
        m_selectedTFKey = configuration->getAttributeValue("selectedTFKey");
        SLM_FATAL_IF("'selectedTFKey' must not be empty", m_selectedTFKey.empty());
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
    ::fwData::TransferFunction::TFValuePairType minMax(windowMin, windowMax);
    this->getTransferFunction()->setWLMinMax( minMax );
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

void MedicalImageAdaptor::installTFConnections()
{
    ::fwData::Composite::sptr tfComposite = this->getTransferFunctionSelection();
    SLM_ASSERT( "Missing transfer function selection composite", tfComposite);

    ::fwCom::Connection connection;
    connection = tfComposite->signal(::fwData::Composite::s_ADDED_OBJECTS_SIG)->connect(m_slotAddedObjects);
    m_tfSelectionConnections.addConnection(connection);
    connection = tfComposite->signal(::fwData::Composite::s_CHANGED_OBJECTS_SIG)->connect(m_slotChangedObjects);
    m_tfSelectionConnections.addConnection(connection);
    connection = tfComposite->signal(::fwData::Composite::s_REMOVED_OBJECTS_SIG)->connect(m_slotRemovedObjects);
    m_tfSelectionConnections.addConnection(connection);

    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    connection = tf->signal(::fwData::TransferFunction::s_POINTS_MODIFIED_SIG)->connect(m_slotUpdateTFPoints);
    m_tfConnections.addConnection(connection);
    connection = tf->signal(::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG)->connect(m_slotUpdateTFWindowing);
    m_tfConnections.addConnection(connection);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::removeTFConnections()
{
    m_tfSelectionConnections.disconnect();
    m_tfConnections.disconnect();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::addObjects(::fwData::Composite::ContainerType objects)
{
    ::fwData::Composite::iterator iter = objects.find(this->getSelectedTFKey());
    if( iter != objects.end())
    {
        ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
        ::fwCom::Connection connection;
        connection = tf->signal(::fwData::TransferFunction::s_POINTS_MODIFIED_SIG)->connect(
            m_slotUpdateTFPoints);
        m_tfConnections.addConnection(connection);
        connection = tf->signal(::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG)->connect(
            m_slotUpdateTFWindowing);
        m_tfConnections.addConnection(connection);

        this->updateTFPoints();
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::changeObjects(::fwData::Composite::ContainerType newObjects,
                                        ::fwData::Composite::ContainerType oldObjects)
{
    ::fwData::Composite::iterator iter = newObjects.find(this->getSelectedTFKey());
    if( iter != newObjects.end())
    {
        m_tfConnections.disconnect();
        ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
        ::fwCom::Connection connection;
        connection = tf->signal(::fwData::TransferFunction::s_POINTS_MODIFIED_SIG)->connect(
            m_slotUpdateTFPoints);
        m_tfConnections.addConnection(connection);
        connection = tf->signal(::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG)->connect(
            m_slotUpdateTFWindowing);
        m_tfConnections.addConnection(connection);

        this->updateTFPoints();
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::removeObjects(::fwData::Composite::ContainerType objects)
{
    ::fwData::Composite::iterator iter = objects.find(this->getSelectedTFKey());
    if( iter != objects.end())
    {
        m_tfConnections.disconnect();
    }
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::installTFSlots(::fwCom::HasSlots* hasslots)
{
    m_slotAddedObjects      = hasslots->newSlot(s_ADD_OBJECTS_SLOT, &MedicalImageAdaptor::addObjects, this);
    m_slotChangedObjects    = hasslots->newSlot(s_CHANGE_OBJECTS_SLOT, &MedicalImageAdaptor::changeObjects, this);
    m_slotRemovedObjects    = hasslots->newSlot(s_REMOVE_OBJECTS_SLOT, &MedicalImageAdaptor::removeObjects, this);
    m_slotUpdateTFPoints    = hasslots->newSlot(s_UPDATE_TF_POINTS_SLOT, &MedicalImageAdaptor::updateTFPoints, this);
    m_slotUpdateTFWindowing =
        hasslots->newSlot(s_UPDATE_TF_WINDOWING_SLOT, &MedicalImageAdaptor::updateTFWindowing, this);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateTFPoints()
{
    this->updatingTFPoints();
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updateTFWindowing(double window, double level)
{
    this->updatingTFWindowing(window, level);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updatingTFPoints()
{
    SLM_ASSERT("This methods (updatingTFPoints) must be reimplemented in subclass to manage TF modifications", false);
}

//------------------------------------------------------------------------------

void MedicalImageAdaptor::updatingTFWindowing(double window, double level)
{
    SLM_ASSERT("This methods (updatingTFWindowing) must be reimplemented in subclass to manage TF modifications",
               false);
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace fwDataTools
