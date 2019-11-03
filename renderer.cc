// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
color renderer<T>::tracepath( const scene<T> &world,
                              std::mt19937 &  rng,
                              const ray<T> &  r,
                              unsigned int    depth )
{
    if ( depth >= _renderParams.maxDepth() )
        return {0, 0, 0, 0};

    hit<T> h;
    if ( !world.intersect( r, h ) )
        return {0, 0, 0, 0};

    ray<T> newray{h._pos, h._normal};

    // DIFFUSE COMPONENT
    vec3<T> w = h._normal;
    vec3<T> u;

    auto x = std::abs(w[0]);
    auto y = std::abs(w[1]);
    auto z = std::abs(w[2]);

    if ( x > y && x > z )
        u = w * vec3<T>( 0, 1, 0 );
    else if ( y > x && y > z )
        u = w * vec3<T>( 0, 0, 1 );
    else
        u = w * vec3<T>( 1, 0, 0 );

    vec3<T> v = w * u;
    assert ( v.len2() != 0 );

    auto  range = ( rng.max() - rng.min() );
    float r1    = 2.0f * M_PI * ( 1.0f * ( rng() - rng.min() ) / range );
    float r2    = 1.0f * ( rng() - rng.min() ) / range;
    float r2s   = std::sqrt( r2 );
    newray.d    = u * std::cos( r1 ) * r2s + v * std::sin( r1 ) * r2s +
               w * std::sqrt( 1 - r2 );
    newray.d.normalize();

    color emit     = h._mat.diffuse() * h._mat.emission();
    color diffuse  = h._mat.diffuse();
    color incoming = tracepath( world, rng, newray, depth + 1 );

    // SPECULAR COMPONENT
    // newray.d = r.d - h._normal * 2 * (r.d % h._normal);
    // newray.d.normalize();
    // incoming =  incoming + tracepath(world, rng, newray, depth + 1);
    return emit + diffuse * incoming;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
void renderer<T>::render( const scene<T> &world )
{
    unsigned int height = _renderParams.height();
    unsigned int width  = _renderParams.width();
    unsigned int maxDepth = _renderParams.maxDepth();
    std::unique_ptr<FILE, decltype( &fclose )> image(
        fopen( "render.ppm", "w" ), &fclose );

    fprintf( image.get(), "P6\n%d %d\n255\n", width, height );
    float progPercent = 0.0f;
    for ( unsigned int jj = 0; jj < height; ++jj )
    {
        float curProg = ( 100.0f * jj / ( height - 1 ) );
        if ( curProg > progPercent )
        {
            progPercent += 10;
            std::cout << "[" << progPercent << "% ...]"
                      << "\n";
        }

        for ( unsigned int ii = 0; ii < width; ++ii )
        {
            color        pixcolor   = {0, 0, 0, 0};
            unsigned int numSamples = 512;
            // subpixel y
            for ( unsigned int spy = 0; spy < 2; ++spy )
            {
                // subpixel x
                for ( unsigned int spx = 0; spx < 2; ++spx )
                {
                    std::mt19937 mtrng( jj * width + ii );
                    float        dx =
                        1.0f * mtrng() / ( mtrng.max() - mtrng.min() ) - 0.5f;
                    float dy =
                        1.0f * mtrng() / ( mtrng.max() - mtrng.min() ) - 0.5f;
                    // build ray for pixel (ii, jj)
                    T u = 1.0f * ( ii + dx ) / ( width - 1 ) - 0.5f;
                    T v =
                        1.0f * ( width - 1 - ( jj + dy ) ) / ( width - 1 ) -
                        0.5f;
                    vec3<T> dir = _camera.direction( u, v );
                    dir.normalize();
                    ray<T> r = {_camera.position(), dir};
                    for ( unsigned int sample = 0; sample < numSamples;
                          ++sample )
                    {
                        pixcolor = pixcolor + tracepath( world, mtrng, r, 0 );
                    }
                }
            }

            pixcolor              = pixcolor / ( 4 * numSamples * maxDepth );
            vec3<unsigned char> c = pixcolor.touchar();
            fprintf( image.get(), "%c%c%c", c[0], c[1], c[2] );
        }
    }
}
