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
		//récupération des trois sommets du triangle
		const fwVec3d P1 = {_vertex[ _vertexIndex[i][0] ][0], _vertex[ _vertexIndex[i][0] ][1], _vertex[ _vertexIndex[i][0] ][2]};
		const fwVec3d P2 = {_vertex[ _vertexIndex[i][1] ][0], _vertex[ _vertexIndex[i][1] ][1], _vertex[ _vertexIndex[i][1] ][2]};
		const fwVec3d P3 = {_vertex[ _vertexIndex[i][2] ][0], _vertex[ _vertexIndex[i][2] ][1], _vertex[ _vertexIndex[i][2] ][2]};

		//on enlève les triangles s'ils sont situés au dessus du point
		if (P1[Z] > _P[Z] && P2[Z] > _P[Z] && P3[Z] > _P[Z] ) break;

		//on teste la présence des vertex de part et d'autre des 3 axes.
		//Si P1[X] > P[X] alors il faut nécessairement P2[X] < P[X] ou P3[X] < P[X], idem pour les 2 autres axes
		//En outre cela permet d'exclure les points qui sont situés sur les axes
		bool stop = false;
		for ( unsigned int axe = X ; axe <= Y && !stop ; ++axe )
		{
			const double Delta1 = P1[axe] - _P[axe];

			if ( Delta1 == 0 ) { stop = true; break;}//cas limite qui ne nous interresse pas

			const double Delta2 = P2[axe] - _P[axe];
			const double Delta3 = P3[axe] - _P[axe];

			if ( Delta1 > 0 && Delta2 > 0 && Delta3 > 0 ) { stop = true; break;}
			if ( Delta1 < 0 && Delta2 < 0 && Delta3 < 0 ) { stop = true; break;}

		}
		if ( !stop )
		{
			//il reste les triangles éligibles
			//std::cout << " Le triangle " << i << " est éligible " << std::endl;

			fwVec3d Pintersection;
			//double orig[3]; orig[0] = _P[0]; orig[1] = _P[1]; orig[2] = _P[2];
			fwVec3d orig = {_P[0], _P[1], _P[2]};

			fwVec3d dir = { 0.f, 0.f, 1.f};
			fwVec3d vert0 = { P1[0], P1[1], P1[2]};
			fwVec3d vert1 = { P2[0], P2[1], P2[2]};
			fwVec3d vert2 = { P3[0], P3[1], P3[2]};
			double t;
			double u;
			double v;
			if ( intersect_triangle (orig, dir, vert0, vert1, vert2, t, u, v) )
			{
				//std::cout << " t = " << t << std::endl;
				//std::cout << " u = " << u << std::endl;
				//std::cout << " v = " << v << std::endl;
				//on ne garde que les points situés en dessous du point _P selon l'axe (Oz)
				if (t < 0.f)
				{
					++IntersectionNbr;
				}
			}
		}
	}
	if ( IntersectionNbr%2 == 1 )
		return true;
	else
		return false;
}


}
