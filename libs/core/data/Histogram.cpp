/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/Histogram.hpp"

#include "data/Exception.hpp"
#include "data/Image.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::Histogram);

namespace sight::data
{

Histogram::Histogram(data::Object::Key) :
    m_binsWidth(1.f),
    m_minValue(0.f),
    m_maxValue(100.f)
{
}

//------------------------------------------------------------------------------

Histogram::~Histogram()
{
}

//------------------------------------------------------------------------------

void Histogram::shallowCopy(const Object::csptr& _source)
{
    Histogram::csptr other = Histogram::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);
    m_values    = other->m_values;
    m_minValue  = other->m_minValue;
    m_maxValue  = other->m_maxValue;
    m_binsWidth = other->m_binsWidth;
}

//------------------------------------------------------------------------------

void Histogram::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Histogram::csptr other = Histogram::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);

    m_minValue  = other->m_minValue;
    m_maxValue  = other->m_maxValue;
    m_binsWidth = other->m_binsWidth;

    m_values.clear();

    for(long value : other->m_values)
    {
        m_values.push_back(value);
    }
}

//------------------------------------------------------------------------------

void Histogram::addPixel(float _pixel)
{
    if(this->isInRange(_pixel))
    {
        size_t index = static_cast<size_t>((_pixel - m_minValue) / m_binsWidth);
        m_values[index]++;
    }
}

//------------------------------------------------------------------------------

void Histogram::initialize(float _min, float _max, float _binsWidth)
{
    SIGHT_ASSERT("The minimum value can't be greater than the maximum value", _min <= _max);

    m_minValue  = _min;
    m_maxValue  = _max;
    m_binsWidth = _binsWidth;

    m_values.clear();

    if(m_binsWidth != 0)
    {
        size_t newSize = static_cast<size_t>((m_maxValue - m_minValue) / m_binsWidth);
        m_values.resize(newSize + 1, 0);
    }
}

//------------------------------------------------------------------------------

long Histogram::getNbPixels(float _min, float _max)
{
    SIGHT_ASSERT("The minimum value can't be greater than the maximum value", _min < _max);

    size_t indexMin = 0;
    if(_min >= m_minValue)
    {
        indexMin = static_cast<size_t>((_min - m_minValue) / m_binsWidth);
    }

    size_t indexMax = m_values.size();
    if(_max <= m_maxValue)
    {
        indexMax = static_cast<size_t>((_max - m_minValue) / m_binsWidth);
    }

    long nbPixels = 0;

    while(indexMin < indexMax)
    {
        nbPixels += m_values.at(indexMin++);
    }

    return nbPixels;
}

//------------------------------------------------------------------------------

bool Histogram::isInRange(float _pixel)
{
    return _pixel >= m_minValue && _pixel <= m_maxValue;
}

} // namespace sight::data
