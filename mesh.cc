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

                _trias.push_back( nodes[0] - 1 );
                _trias.push_back( nodes[2] - 1 );
                _trias.push_back( nodes[3] - 1 );
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
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
    // compute plane's normal
    vec3<T> v0v1 = v1 - v0;
    vec3<T> v0v2 = v2 - v0;

    // no need to normalize
    vec3<T> N = v0v1 * v0v2; // N
    N.normalize();

    const vec3<T> &orig = r.o;
    const vec3<T> &dir  = r.d;
    // Step 1: finding P

    constexpr float kEpsilon = 1e-8;
    // check if ray and plane are parallel ?
    T NdotRayDirection = N % dir;
    if ( fabs( NdotRayDirection ) < kEpsilon ) // almost 0
        return false; // they are parallel so they don't intersect !

    // compute d parameter using equation 2
    T d = -( N % v0 );

    // compute t (equation 3)
    float t = -( N % orig + d ) / NdotRayDirection;
    // check if the triangle is in behind the ray
    if ( t < 0 )
        return false; // the triangle is behind

    // compute the intersection point using equation 1
    vec3<T> P = orig + dir * t;

    // Step 2: inside-outside test
    vec3<T> C; // vector perpendicular to triangle's plane

    // edge 0
    vec3<T> edge0 = v1 - v0;
    vec3<T> vp0   = P - v0;
    C             = edge0 * vp0;
    if ( N % C < 0 )
        return false; // P is on the right side

    // edge 1
    vec3<T> edge1 = v2 - v1;
    vec3<T> vp1   = P - v1;
    C             = edge1 * vp1;
    if ( N % C < 0 )
        return false; // P is on the right side

    // edge 2
    vec3<T> edge2 = v0 - v2;
    vec3<T> vp2   = P - v2;
    C             = edge2 * vp2;
    if ( N % C < 0 )
        return false; // P is on the right side;

    h._normal = N;
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
