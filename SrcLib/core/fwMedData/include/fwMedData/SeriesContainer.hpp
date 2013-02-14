/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_SERIESCONTAINER_HPP__
#define __FWMEDDATA_SERIESCONTAINER_HPP__

#include <vector>

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(SeriesContainer), FWMEDDATA_API);

namespace fwMedData
{

class Series;

/**
 * @class SeriesContainer
 *
 * @author  IRCAD (Research and Development Team).
 */
class FWMEDDATA_CLASS_API SeriesContainer : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (SeriesContainer)(::fwData::Object), (()), ::fwData::factory::New< SeriesContainer >) ;

    fwCampMakeFriendDataMacro((fwMedData)(SeriesContainer));

    fwDataObjectMacro();


    typedef std::vector< SPTR(Series) > ContainerType;

    typedef ContainerType::value_type ValueType;
    typedef ContainerType::reference ReferenceType;
    typedef ContainerType::const_reference ConstReferenceType;
    typedef ContainerType::iterator IteratorType;
    typedef ContainerType::const_iterator ConstIteratorType;
    typedef ContainerType::reverse_iterator ReverseIteratorType;
    typedef ContainerType::const_reverse_iterator ConstReverseIteratorType;
    typedef ContainerType::size_type SizeType;

    /// boost_foreach/stl compatibility
    /// @{
    typedef ContainerType::value_type value_type;
    typedef ContainerType::iterator iterator;
    typedef ContainerType::const_iterator const_iterator;
    typedef ContainerType::reverse_iterator reverse_iterator;
    typedef ContainerType::const_reverse_iterator const_reverse_iterator;
    typedef ContainerType::size_type size_type;


    IteratorType begin() { return m_attrContainer.begin(); }
    IteratorType end()   { return m_attrContainer.end(); }
    ConstIteratorType begin() const { return m_attrContainer.begin(); }
    ConstIteratorType end()   const { return m_attrContainer.end(); }

    ReverseIteratorType rbegin() { return m_attrContainer.rbegin(); }
    ReverseIteratorType rend()   { return m_attrContainer.rend(); }
    ConstReverseIteratorType rbegin() const { return m_attrContainer.rbegin(); }
    ConstReverseIteratorType rend()   const { return m_attrContainer.rend(); }

    bool empty() const { return m_attrContainer.empty(); }
    SizeType size() const { return m_attrContainer.size(); }

    ValueType front(){ return m_attrContainer.front(); }
    ValueType back(){ return m_attrContainer.back(); }

    ReferenceType operator[] ( size_type n ) {return this->m_attrContainer[n];}
    ConstReferenceType operator[] ( size_type n ) const {return this->m_attrContainer[n];}

    ReferenceType at ( SizeType n ) {return m_attrContainer.at(n);}
    ConstReferenceType at ( SizeType n ) const {return m_attrContainer.at(n);}
    /// @}


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API SeriesContainer(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~SeriesContainer();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( SeriesContainer::csptr _source );

    /// Defines deep copy
    FWMEDDATA_API void deepCopy( SeriesContainer::csptr _source );

    ContainerType &getContainer(){ return m_attrContainer; };
    fwDataGetSetCRefMacro(Container, ContainerType);

protected:

    ContainerType m_attrContainer;

};

}   //end namespace fwMedData

#endif // __FWMEDDATA_SERIESCONTAINER_HPP__


