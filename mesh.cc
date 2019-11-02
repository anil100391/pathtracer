#include <fstream>
#include <string>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
mesh<T>::mesh( const std::string &filename ) noexcept
{
    std::ifstream file( filename );

    std::string line;
    char        ch;
    float       x, y, z;
    int         nodes[4];
    while ( getline( file, line ) )
    {
        if ( line[0] == 'v' )
        {
            if ( line[1] != 'n' )
            {
                if ( 4 ==
                     sscanf( line.c_str(), "%s %f %f %f", &ch, &x, &y, &z ) )
                {
                    _vertices.emplace_back( x, y, z );
                    _box.expand( _vertices.back() );
                }
            }
        }
        else if ( line[0] == 'f' )
        {
            if ( 5 == sscanf( line.c_str(),
                              "%s %d %d %d %d",
                              &ch,
                              &nodes[0],
                              &nodes[1],
                              &nodes[2],
                              &nodes[3] ) )
            {
                _trias.push_back( nodes[0] - 1 );
                _trias.push_back( nodes[1] - 1 );
                _trias.push_back( nodes[2] - 1 );
                auto randomcol = color::random();
                _mat.emplace_back( randomcol );

                _trias.push_back( nodes[0] - 1 );
                _trias.push_back( nodes[2] - 1 );
                _trias.push_back( nodes[3] - 1 );
                _mat.push_back( randomcol );
            }
            else if ( 4 == sscanf( line.c_str(),
                                   "%s %d %d %d",
                                   &ch,
                                   &nodes[0],
                                   &nodes[1],
                                   &nodes[2] ) )
            {
                _trias.push_back( nodes[0] - 1 );
                _trias.push_back( nodes[1] - 1 );
                _trias.push_back( nodes[2] - 1 );
                _mat.emplace_back( color::random() );
            }
        }
    }

    std::cout << "mesh stats: " << _vertices.size() << " vertices | "
              << _trias.size() / 3 << " triangles | bounding box " << _box
              << "\n";
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
bool mesh<T>::intersect( const ray<T> &r, hit<T> &h ) const noexcept
{
    if ( !intersect( _box, r ) )
        return false;

    hit<T> localHit;
    T      shortestDist = std::numeric_limits<T>::max();
    for ( unsigned int ii = 0; ii < _trias.size() / 3; ++ii )
    {
        const unsigned int *t  = &_trias[3 * ii];
        const vec3<T> &     v0 = _vertices[t[0]];
        const vec3<T> &     v1 = _vertices[t[1]];
        const vec3<T> &     v2 = _vertices[t[2]];
        if ( rayTriaIntersect( r, v0, v1, v2, localHit ) )
        {
            T dist = ( r.o - localHit._pos ).len2();
            if ( dist < shortestDist )
            {
                shortestDist = dist;
                h            = localHit;
                h._mat       = _mat[ii];
            }
        }
    }

    return ( shortestDist != std::numeric_limits<T>::max() );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
bool mesh<T>::intersect( const bbox<T> &box, const ray<T> &r )
{
    if ( box.contains( r.o ) )
        return true;

    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
    T tmin, tmax, tymin, tymax, tzmin, tzmax;

    vec3<T> invdir{1 / r.d[0], 1 / r.d[1], 1 / r.d[2]};
    int     sign[3] = {( invdir[0] < 0 ), ( invdir[1] < 0 ), ( invdir[2] < 0 )};
    vec3<T> bounds[2] = {box.min(), box.max()};
    tmin              = ( bounds[sign[0]][0] - r.o[0] ) * invdir[0];
    tmax              = ( bounds[1 - sign[0]][0] - r.o[0] ) * invdir[0];
    tymin             = ( bounds[sign[1]][1] - r.o[1] ) * invdir[1];
    tymax             = ( bounds[1 - sign[1]][1] - r.o[1] ) * invdir[1];

    if ( ( tmin > tymax ) || ( tymin > tmax ) )
        return false;

    if ( tymin > tmin )
        tmin = tymin;
    if ( tymax < tmax )
        tmax = tymax;

    tzmin = ( bounds[sign[2]][2] - r.o[2] ) * invdir[2];
    tzmax = ( bounds[1 - sign[2]][2] - r.o[2] ) * invdir[2];

    if ( ( tmin > tzmax ) || ( tzmin > tmax ) )
        return false;
    if ( tzmin > tmin )
        tmin = tzmin;
    if ( tzmax < tmax )
        tmax = tzmax;

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
bool mesh<T>::rayTriaIntersect( const ray<T> & r,
                                const vec3<T> &v0,
                                const vec3<T> &v1,
                                const vec3<T> &v2,
                                hit<T> &       h )
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
    auto v0v1 = v1 - v0; 
    auto v0v2 = v2 - v0; 
    const vec3<T> &dir = r.d;
    const vec3<T> &orig = r.o;
    auto pvec = dir * v0v2; 
    T det = v0v1 % pvec; 

    constexpr float kEpsilon = 1e-8;
    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < kEpsilon) return false; 

    bool backfacing = (det < kEpsilon);
    T invDet = 1.0 / det; 
 
    auto tvec = orig - v0; 
    T u = (tvec % pvec) * invDet; 
    if (u < 0 || u > 1) return false; 
 
    auto qvec = tvec * v0v1; 
    T v = (dir % qvec) * invDet; 
    if (v < 0 || u + v > 1) return false; 
 
    T t = (v0v2 % qvec) * invDet; 
    if ( t < kEpsilon )
        return false;
 
    auto N = v0v1 * v0v2;
    N.normalize();

    h._normal = N;
    if ( backfacing )
        h._normal = N * -1.0;
    h._pos    = r.o + r.d * t;
    return true; // this ray hits the triangle
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<typename T>
constexpr void mesh<T>::transform(const mat44<T> &mat) noexcept
{
    _box.reset();
    for ( auto& v : _vertices )
    {
        mat.Transform(v);
        _box.expand(v);
    }
}
