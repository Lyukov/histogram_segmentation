#include <iostream>
#include "/home/dmitry/ImageProcessing/Task1/imageformats.hpp"
#include "/home/dmitry/ImageProcessing/Task1/imageio.cpp"
#include "/home/dmitry/ImageProcessing/Task1/imageio.hpp"
#include "/home/dmitry/ImageProcessing/Task1/pixelformats.hpp"
#include "histogram.hpp"

int main(int argc, char **argv) {
    ColorByteImage image = ImageIO::FileToColorByteImage(argv[1]);
    Histogram<3, unsigned char> hist;
    for(int j = 0; j < image.Height(); ++j) {
        for(int i = 0; i < image.Width(); ++i) {
            unsigned char _key[3] = {image(i, j).r, image(i, j).g, image(i, j).b};
            Key<3, unsigned char> key(&(_key[0]));
            hist.add(1., key);
        }
    }
    hist.sort();
    double count = 0;
    for(auto it = hist.begin(); it != hist.end(); ++it) {
        count += it->count;
    }
    for (size_t i = hist.size() - 1; i > 0 && i > hist.size() - 20; --i) {
        std::cout << hist[i].key << ' ' << hist[i].count << std::endl;
    }
    std::cout << "all: " << count << std::endl;
    std::cout << "hist size = " << hist.size() << std::endl;
    return 0;
}
