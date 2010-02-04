/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

namespace fwTools
{

template< typename FWDATATYPE >
typename FWDATATYPE::sptr Object::getFieldSingleElement( const FieldID& id ) const
	throw()
{

	Field::csptr field = getField( id );
	if ( field && !field->children().empty() )
	{
		assert( field->children().size() == 1 );
		return FWDATATYPE::dynamicCast( field->children().front() );
	}
	else
	{
		return typename FWDATATYPE::sptr();
	}

}





template< typename FWDATATYPE >
typename FWDATATYPE::sptr Object::getFieldElement( const FieldID& id , unsigned int _index ) const
	throw()
{
	Field::csptr labeledObject =
		Field::dynamicCast( getField( id ) );

	if ( labeledObject )
	{
		const ChildContainer& children = labeledObject->children();
		if ( !children.empty() )
		{
			typename FWDATATYPE::sptr typedFieldElement =
				FWDATATYPE::dynamicCast( children.at( _index ) );
			if ( typedFieldElement ) return typedFieldElement;
		}
	}

	return typename FWDATATYPE::sptr();

}

} // namespace fwTools

