/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

namespace fwRuntime
{



/**
 * @brief	Implements a STL compliant predicate that tests if a given
 * 			object has a given identifier and is enabled.
 * @struct	IsEnableAndHasIdentifier
 *
 * @remark	Intended to be used on extensions, extension points and bundles
 *
 * @date	2007-2009
 * @author 	IRCAD (Research and Development Team).
 */
template< typename T >
struct IsEnableAndHasIdentifier
{
	/**
	 * @brief	Constructor.
	 */
	IsEnableAndHasIdentifier( const std::string & identifier )
	:	m_identifier( identifier )
	{}

	const bool operator() ( const ::boost::shared_ptr< T > p ) const
	{
		return p->getIdentifier() == m_identifier && p->isEnable();
	}

private:

	std::string m_identifier;
};




} // namespace fwRuntime
