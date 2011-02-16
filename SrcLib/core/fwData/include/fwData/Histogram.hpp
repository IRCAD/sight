/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_HISTOGRAM_HPP_
#define _FWDATA_HISTOGRAM_HPP_

#include <fwData/Object.hpp>
#include "fwData/config.hpp"

#include <boost/shared_ptr.hpp>


namespace fwData
{

typedef std::vector< long > ObjectHistogramType;

/**
 * @class   Histogram
 * @brief   This class defines the histogram of an image.
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class FWDATA_CLASS_API Histogram : public Object, private ObjectHistogramType
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Histogram)(::fwData::Object), (()), ::fwTools::Factory::New< Histogram > );

    typedef Histogram Container;
    typedef ObjectHistogramType ContainerType;

    using ObjectHistogramType::reference;
    using ObjectHistogramType::const_reference;
    using ObjectHistogramType::iterator;
    using ObjectHistogramType::const_iterator;
    using ObjectHistogramType::size_type;
    using ObjectHistogramType::difference_type;
    using ObjectHistogramType::value_type;
    using ObjectHistogramType::allocator_type;
    using ObjectHistogramType::pointer;
    using ObjectHistogramType::const_pointer;
    using ObjectHistogramType::reverse_iterator;
    using ObjectHistogramType::const_reverse_iterator;

    using ObjectHistogramType::begin;
    using ObjectHistogramType::end;
    using ObjectHistogramType::rbegin;
    using ObjectHistogramType::rend;

    using ObjectHistogramType::size;
    using ObjectHistogramType::max_size;
    using ObjectHistogramType::resize;
    using ObjectHistogramType::capacity;
    using ObjectHistogramType::empty;
    using ObjectHistogramType::reserve;

    using ObjectHistogramType::operator[];
    using ObjectHistogramType::at;
    using ObjectHistogramType::front;
    using ObjectHistogramType::back;

    using ObjectHistogramType::assign;
    using ObjectHistogramType::push_back;
    using ObjectHistogramType::pop_back;
    using ObjectHistogramType::insert;
    using ObjectHistogramType::erase;
    using ObjectHistogramType::swap;
    using ObjectHistogramType::clear;

    using ObjectHistogramType::get_allocator;

    /// Constructor
    FWDATA_API Histogram();

    /// Destructor
    FWDATA_API virtual ~Histogram();

    /// @brief get the container of ::fwData::Object
    FWDATA_API Histogram &getRefContainer();

    /// @brief get the container of ::fwData::Object
    FWDATA_API Histogram const &getRefContainer() const;

    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Histogram::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Histogram::csptr _source );
 
    FWDATA_API void addPixel( signed int _pixel );
    
    FWDATA_API void addPixel( float _pixel );

    FWDATA_API long getNbPixels( double min, double max );
   
    FWDATA_API bool isInRange( signed int _pixel );

    fwGettersSettersDocMacro(BinsWidth, binsWidth, int, bins width);
    fwGettersSettersDocMacro(MinValue, minValue, double, minimum value within the histogram);
    fwGettersSettersDocMacro(MaxValue, maxValue, double, maximum value within the histogram);

protected:

    /**
     * @brief The pixel width of the bins.
     */
    int m_binsWidth;

    /**
     * @brief The minimum pixel value within the histogram.
     */
    double m_minValue;
    
    /**
     * @brief The maximum pixel value within the histogram.
     */
    double m_maxValue;
};

}

#endif // _FWDATA_HISTOGRAM_HPP_

