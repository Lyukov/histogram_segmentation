#include <string.h>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include "histogram.hpp"
#include "imageformats.hpp"
#include "imageio.hpp"
#include "k-means.hpp"
#include "pixelformats.hpp"

template <size_t N, typename T>
ColorByteImage draw_clusters( const ColorByteImage& image,
                              const Histogram<N, T>& hist,
                              const std::vector<std::set<size_t> >& clusters,
                              const ColorBytePixel* colors,
                              const size_t nonce = 0 )
{
    ColorByteImage result( image.Width(), image.Height() );

    const size_t hist_w = 256 * 3;
    const size_t hist_h = 256;

    ColorFloatImage hist_image( hist_w, hist_h );
    hist_image.for_each_pixel( []( ColorFloatPixel ) { return ColorFloatPixel( 0.f ); } );

    std::map<Key<N, T>, size_t> color_map;
    for( size_t cluster = 0; cluster < clusters.size(); ++cluster )
    {
        for( auto item = clusters[cluster].begin(); item != clusters[cluster].end(); ++item )
        {
            color_map.insert( std::pair<Key<N, T>, size_t>( hist[*item].key, cluster ) );

            const float k = 0.15f;
            float alpha = hist[*item].count / 256.f * k;
            alpha = ( alpha > 1.f ) ? 1.f : alpha;
            ColorFloatPixel color = alpha * ( ColorFloatPixel( 255.f ) + -1.f * colors[cluster] );
            hist_image( 256 * 0 + hist[*item].key[0], 255 - hist[*item].key[1] ) += color;
            hist_image( 256 * 1 + hist[*item].key[0], 255 - hist[*item].key[2] ) += color;
            hist_image( 256 * 2 + hist[*item].key[1], 255 - hist[*item].key[2] ) += color;
        }
    }
    for( size_t j = 0; j < hist_h; ++j )
    {
        for( size_t i = 0; i < hist_w; ++i )
        {
            hist_image( i, j ) = ColorFloatPixel( 255.f ) + -1.f * hist_image( i, j );
        }
    }

    std::string fname = std::string( "histf" ) + std::to_string( nonce ) + std::string( ".bmp" );
    ImageIO::ImageToFile( hist_image, fname.c_str() );

    for( int j = 0; j < image.Height(); ++j )
    {
        for( int i = 0; i < image.Width(); ++i )
        {
            unsigned char _key[3] = {image( i, j ).r, image( i, j ).g, image( i, j ).b};
            Key<3, unsigned char> key( &( _key[0] ) );
            size_t color_number = color_map[key];
            result( i, j ) = colors[color_number];
        }
    }
    return result;
}

int main( int argc, char** argv )
{
    if (argc < 2) {
        std::cout << "command: ./program_name path_to_image number_of_clusters" << std::endl;
        return -1;
    }
    ColorByteImage image = ImageIO::FileToColorByteImage( argv[1] );
    Histogram<3, unsigned char> hist;

    std::chrono::time_point<std::chrono::system_clock> start_time, end_time;
    start_time = std::chrono::system_clock::now();

    for( int j = 0; j < image.Height(); ++j )
    {
        for( int i = 0; i < image.Width(); ++i )
        {
            unsigned char _key[3] = {image( i, j ).r, image( i, j ).g, image( i, j ).b};
            Key<3, unsigned char> key( &( _key[0] ) );
            hist.add( 1., key );
        }
    }

    end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> creation_second = end_time - start_time;
    std::cout << std::endl
              << "Histogram creation time: " << creation_second.count() << "s" << std::endl;

    start_time = std::chrono::system_clock::now();

    hist.sort();

    end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> sort_second = end_time - start_time;
    std::cout << "Sort time: " << sort_second.count() << "s" << std::endl;

    start_time = std::chrono::system_clock::now();

    hist.rebuild_tree();

    end_time = std::chrono::system_clock::now();
    std::chrono::duration<double> rebuild_second = end_time - start_time;
    std::cout << "Rebuild tree time: " << rebuild_second.count() << "s" << std::endl;
    double count = 0;
    for( auto it = hist.begin(); it != hist.end(); ++it )
    {
        count += it->count;
    }
    std::cout << "all: " << count << std::endl;
    std::cout << "hist size = " << hist.size() << std::endl;

    const size_t hist_w = 256 * 3;
    const size_t hist_h = 256;

    GrayscaleFloatImage hist_image( hist_w, hist_h );
    hist_image.for_each_pixel( []( float ) { return 255.f; } );
    const float k = 0.15f;
    for( size_t i = 0; i < hist.size(); ++i )
    {
        hist_image( 256 * 0 + hist[i].key[0], 255 - hist[i].key[1] ) -= hist[i].count * k;
        hist_image( 256 * 1 + hist[i].key[0], 255 - hist[i].key[2] ) -= hist[i].count * k;
        hist_image( 256 * 2 + hist[i].key[1], 255 - hist[i].key[2] ) -= hist[i].count * k;
    }
    ImageIO::ImageToFile( hist_image, "histogram.bmp" );

    const int N_TEST = 9;
    ColorBytePixel colors[N_TEST] = {
        ColorBytePixel( 255, 0, 0 ),     ColorBytePixel( 0, 255, 0 ),
        ColorBytePixel( 0, 0, 255 ),     ColorBytePixel( 255, 255, 0 ),
        ColorBytePixel( 0, 255, 255 ),   ColorBytePixel( 255, 0, 255 ),
        ColorBytePixel( 255, 127, 127 ), ColorBytePixel( 127, 127, 255 ),
        ColorBytePixel( 127, 255, 127 )};

    std::vector<std::set<size_t> > clusters;

    if (argc < 3) {
        std::cout << "command: ./program_name path_to_image number_of_clusters" << std::endl;
        return -1;
    }
    const size_t number_of_clusters = atoi( argv[2] );
    auto centers = KMeans::InitClusterCenters( number_of_clusters, hist );
    double sum_shift = 0.;
    const double eps = 0.001;
    int i = 0;
    ColorByteImage im( image.Width(), image.Height() );
    do
    {
        ++i;
        start_time = std::chrono::system_clock::now();

        sum_shift = KMeans::KMeansIteration( hist, centers, clusters );

        end_time = std::chrono::system_clock::now();
        std::chrono::duration<double> clust_second = end_time - start_time;
        std::cout << "Iteration " << i << ": clustering time: " << clust_second.count() << "s"
                  << std::endl;

        start_time = std::chrono::system_clock::now();

        im = draw_clusters( image, hist, clusters, colors, i );
        std::string filename =
            std::string( "clustiter" ) + std::to_string( i ) + std::string( ".bmp" );
        ImageIO::ImageToFile( im, filename.c_str() );

        end_time = std::chrono::system_clock::now();
        std::chrono::duration<double> draw_second = end_time - start_time;
        std::cout << "Drawing time: " << draw_second.count() << "s" << std::endl;

        std::cout << "Shift: " << sum_shift << std::endl << std::endl;
    } while( sum_shift > eps );

    return 0;
}