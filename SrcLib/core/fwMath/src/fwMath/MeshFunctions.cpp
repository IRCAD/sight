#include <list>

#include "fwMath/MeshFunctions.hpp"
#include "fwMath/VectorFunctions.hpp"


namespace fwMath
{

bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0, fwVec3d _vert1, fwVec3d _vert2, double &_t, double &_u, double &_v)
{
    const double Epsilon = 0.000001;

    fwVec3d edge1, edge2, tvec, pvec, qvec;

    /* find vectors for two edges sharing vert0 */
    edge1 = _vert1 - _vert0;
    edge2 = _vert2 - _vert0;

    /* begin calculating determinant - also used to calculate U parameter */
    pvec = ::fwMath::cross(_dir, edge2);

    /* if determinant is near zero, ray lies in plane of triangle */
    const double Det = ::fwMath::dot(edge1, pvec);

    if (Det > -Epsilon && Det < Epsilon) return false;
    const double Inv_det = 1.0 / Det;

    /* calculate distance from vert0 to ray origin */
    tvec = _orig - _vert0;

    /* calculate U parameter and test bounds */
    _u = Inv_det * fwMath::dot(tvec, pvec);
    if (_u < 0.0 || _u > 1.0) return false;

    /* prepare to test V parameter */
    qvec = ::fwMath::cross(tvec, edge1);

    /* calculate V parameter and test bounds */
    _v =  Inv_det * ::fwMath::dot(_dir, qvec);
    if (_v < 0.0 || _u + _v > 1.0) return false;

    /* calculate t, ray intersects triangle */
    _t = Inv_det * ::fwMath::dot(edge2, qvec);
    return true;
}

//------------------------------------------------------------------------------

bool IsInclosedVolume(const fwVertexPosition _vertex, const fwVertexIndex _vertexIndex, const fwVec3d &_P)
{

    const unsigned int X=0, Y=1, Z=2;
    const unsigned int ElementNbr  = _vertexIndex.size();
    if ( ElementNbr <= 0 )
        return false;

    // on regarde tous les triangles du maillage
    unsigned int IntersectionNbr = 0;
    for ( unsigned int i = 0 ; i < ElementNbr ; ++i )
    {
        //r�cup�ration des trois sommets du triangle
        const fwVec3d P1 = {_vertex[ _vertexIndex[i][0] ][0], _vertex[ _vertexIndex[i][0] ][1], _vertex[ _vertexIndex[i][0] ][2]};
        const fwVec3d P2 = {_vertex[ _vertexIndex[i][1] ][0], _vertex[ _vertexIndex[i][1] ][1], _vertex[ _vertexIndex[i][1] ][2]};
        const fwVec3d P3 = {_vertex[ _vertexIndex[i][2] ][0], _vertex[ _vertexIndex[i][2] ][1], _vertex[ _vertexIndex[i][2] ][2]};

        //on enl�ve les triangles s'ils sont situ�s au dessus du point
        //if (P1[Z] > _P[Z] && P2[Z] > _P[Z] && P3[Z] > _P[Z] ) break;
        OSLM_TRACE("Trg : " << i << " with Z = [" << P1[Z]  << "][" << P2[Z]  << "][" << P3[Z]  << "] compare with " << _P[Z] );
        bool trianglePotentiallyWellPositionned = true;
        if (P1[Z] > _P[Z] && P2[Z] > _P[Z] && P3[Z] > _P[Z] ) trianglePotentiallyWellPositionned = false;

        if ( trianglePotentiallyWellPositionned )
        {
            //on teste la pr�sence des vertex de part et d'autre des 3 axes.
            //Si P1[X] > P[X] alors il faut n�cessairement P2[X] < P[X] ou P3[X] < P[X], idem pour les 2 autres axes
            //En outre cela permet d'exclure les points qui sont situ�s sur les axes
            bool stop = false;
            for ( unsigned int axe = X ; axe <= Y && !stop ; ++axe )
            {
                const double Delta1 = P1[axe] - _P[axe];
                const double Delta2 = P2[axe] - _P[axe];
                const double Delta3 = P3[axe] - _P[axe];

                OSLM_TRACE("d1 : " << Delta1 << "d2 : " << Delta2 << "d3 : " << Delta3 );

                if ( Delta1 >= 0.f && Delta2 >= 0.f && Delta3 >= 0.f ) { stop = true; break;}
                if ( Delta1 < 0.f && Delta2 < 0.f && Delta3 < 0.f ) { stop = true; break;}

            }
            if ( !stop )
            {
                OSLM_TRACE("The face(" << i << ") is interesting to find a point in volume");

                fwVec3d Pintersection;
                fwVec3d orig = {_P[0], _P[1], _P[2]};

                fwVec3d dir = { 0.f, 0.f, 1.f};
                fwVec3d vert0 = { P1[0], P1[1], P1[2]};
                fwVec3d vert1 = { P2[0], P2[1], P2[2]};
                fwVec3d vert2 = { P3[0], P3[1], P3[2]};
                double t, u, v;
                if ( intersect_triangle (orig, dir, vert0, vert1, vert2, t, u, v) )
                {
                    //on ne garde que les points situ�s en dessous du point _P selon l'axe (Oz)
                    if (t < 0.f)
                    {
                        OSLM_TRACE(" t = " << t << " u = " << u << " v = " << v);
                        ++IntersectionNbr;
                    }
                }
            }
        }
    }
    OSLM_TRACE("Nb intersection : " << IntersectionNbr);
    if ( IntersectionNbr%2 == 1 )
        return true;
    else
        return false;
}


bool isBorderlessSurface(const fwVertexIndex _vertexIndex)
{
	typedef std::pair< int, int  >  Edge; // always Edge.first < Edge.second !!
	std::map< Edge  , int > edgesHistogram;
	for ( fwVertexIndex::const_iterator iter=_vertexIndex.begin(); iter!= _vertexIndex.end(); ++iter )
	{
		assert (iter->size()>2 );
		int i1=  (*iter)[0];
		int i2 = (*iter)[1];
		int i3 = (*iter)[2];
		edgesHistogram[std::make_pair(std::min(i1,i2),std::max(i1,i2) )]++;
		edgesHistogram[std::make_pair(std::min(i1,i3),std::max(i1,i3) )]++;
		edgesHistogram[std::make_pair(std::min(i3,i2),std::max(i3,i2) )]++;
	}

	for ( std::map< Edge  , int >::const_iterator iter=edgesHistogram.begin(); iter!=edgesHistogram.end(); ++iter )
	{
		if (iter->second<2)
		{
			return false;
		}
	}
	return true;
}

// container of connected componnent
void findBorderEdges( const fwVertexIndex _vertexIndex , std::vector< std::vector<  std::pair< int, int  > > > &contours)
{
	typedef std::pair< int, int  >  Edge;
	typedef std::vector< Edge > Contour; // at Border
	typedef std::vector< Contour> Contours;

	std::map< Edge  , int > edgesHistogram;
	for ( fwVertexIndex::const_iterator iter=_vertexIndex.begin(); iter!= _vertexIndex.end(); ++iter )
	{
		assert (iter->size()>2 );
		int i1=  (*iter)[0];
		int i2 = (*iter)[1];
		int i3 = (*iter)[2];
		edgesHistogram[std::make_pair(std::min(i1,i2),std::max(i1,i2) )]++;
		edgesHistogram[std::make_pair(std::min(i1,i3),std::max(i1,i3) )]++;
		edgesHistogram[std::make_pair(std::min(i3,i2),std::max(i3,i2) )]++;
	}

	for ( std::map< Edge  , int >::const_iterator iter=edgesHistogram.begin(); iter!=edgesHistogram.end(); ++iter )
	{
		if (iter->second<2) // an orphan found
		{
			Contour contour;
			contour.reserve(1000);
			std::list< Edge > fifo;
			Edge orphan = iter->first;

			fifo.push_back(orphan);
			while( !fifo.empty() )
			{
				Edge current = fifo.front();
				contour.push_back( current );
				fifo.pop_front();
				edgesHistogram[current]=2; // to mark it processed;
				// search neighboor at border and insert in fifo
				for ( std::map< Edge  , int >::const_iterator iterL=edgesHistogram.begin(); iterL!=edgesHistogram.end(); ++iterL )
				{
					Edge candidate= iterL->first;
					if ( iterL->second < 2 ) // at border
					{
						if ( candidate.first == current.first ||  candidate.second == current.second || // neighboor
						     candidate.first == current.second ||  candidate.second == current.first
						   )
						{
							edgesHistogram[candidate]=2; // mark processed;
							fifo.push_back( candidate );
						}
					}
				}
			}
			// all neighboor processed
			contours.push_back( contour );
		}
	}
}

bool closeSurface(  fwVertexPosition &_vertex, fwVertexIndex &_vertexIndex )
{
	typedef std::pair< int, int  >  Edge;
	typedef std::vector< Edge > Contour; // at Border
	typedef std::vector< Contour> Contours;

	Contours contours;
	findBorderEdges( _vertexIndex , contours);
	// close each hole
	for ( Contours::iterator contour=contours.begin();  contour != contours.end(); ++contour )
	{
		int newVertexIndex = _vertex.size() ;
		// create gravity point & insert new triangle
		std::vector< float > massCenter(3,0);
		for ( Contour::iterator edge =contour->begin();  edge != contour->end(); ++edge )
		{
			for (int i=0; i<3; ++i )
			{
				massCenter[i]  += _vertex[edge->first][i];
				massCenter[i]  += _vertex[edge->second][i];
			}
			// create new Triangle
			std::vector< int > triangleIndex(3);
			triangleIndex[0] =  edge->first;
			triangleIndex[1] =  edge->second;
			triangleIndex[2] =  newVertexIndex;
			_vertexIndex.push_back( triangleIndex ); // TEST

		}
		for (int i=0; i<3; ++i )
		{
			massCenter[i] /= contour->size()*2;
		}
		_vertex.push_back( massCenter ); // normalize barycenter

	}
}

// slow version but contour edges are geometrically chained
//
//
//std::pair< int, int  >  normalize( std::pair< int, int  > p)
//{
//	return std::make_pair(std::min( p.first ,p.second ) ,std::max( p.first ,p.second ) );
//}

//void findBorderEdges( const fwVertexIndex _vertexIndex , std::vector< std::vector<  std::pair< int, int  > > > &contours)
//{
//	typedef std::pair< int, int  >  Edge;
//	typedef std::vector< Edge > Contour; // at Border
//	typedef std::vector< Contour> Contours;
//
//	std::map< Edge  , int > edgesHistogram;
//	for ( fwVertexIndex::const_iterator triangle=_vertexIndex.begin(); triangle!= _vertexIndex.end(); ++triangle )
//	{
//		assert (triangle->size()>2 );
//		for (int i=0; i < 3; ++i)
//		{
//			Edge edge( (*triangle)[i] , (*triangle)[(i+1)%3] );
//			edgesHistogram[ normalize(edge) ]++;
//		}
//	}
//
//	for ( fwVertexIndex::const_iterator triangle=_vertexIndex.begin(); triangle!= _vertexIndex.end(); ++triangle )
//	{
//		for (int i=0; i < 3; ++i)
//		{
//			Edge edge( (*triangle)[i] , (*triangle)[(i+1)%3] );
//
//			if (edgesHistogram[ normalize(edge) ]<2) // an orphan found
//			{
//				Contour contour;
//				contour.reserve(1000);
//				std::list< Edge > fifo;
//
//				fifo.push_back(edge);
//				while( !fifo.empty() )
//				{
//					Edge current = fifo.front();
//					contour.push_back( current );
//					fifo.pop_front();
//					edgesHistogram[ normalize(current) ]=2; // to mark it processed;
//					// search neighboor at border and insert in fifo
//					for ( fwVertexIndex::const_iterator triangleL=_vertexIndex.begin(); triangleL!= _vertexIndex.end(); ++triangleL )
//					{
//						for (int j=0; j < 3; ++j)
//						{
//							Edge candidate(  (*triangleL)[j], (*triangleL)[(j+1)%3] );
//							if ( edgesHistogram[ normalize(candidate) ] < 2 ) // at border
//							{
//
//								if ( candidate.first == current.second )// neighboor only one direction
//								{
//									edgesHistogram[normalize(candidate)]=2; // mark processed;
//									fifo.push_back( candidate );
//								}
//							}
//						}
//					}
//				}
//				// all neighboor processed
//				contours.push_back( contour );
//			}
//		} // vertex parse
//	} // triangle parse
//}




} // namespace fwMath
