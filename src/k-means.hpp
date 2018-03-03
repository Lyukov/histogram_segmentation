#pragma once

#include <chrono>
#include <cmath>
#include <random>
#include <set>
#include <vector>
#include "histogram.hpp"

namespace KMeans {

template <size_t N, typename T>
double distance(const Key<N, T>& k1, const Key<N, T>& k2) {
    double sum = 0.;
    for(size_t i = 0; i < N; ++i) {
        sum += double(k1[i] - k2[i]) * double(k1[i] - k2[i]);
    }
    return sum;
}

template <size_t N, typename T>
std::vector<Key<N, T> > InitClusterCenters(const size_t num_of_clusters,
                                           const Histogram<N, T>& hist) {
    // const int seed = 42;
    // std::mt19937 generator(seed);

    std::vector<Key<N, T> > result;
    for(size_t i = 0; i < num_of_clusters; ++i) {
        result.push_back(hist[i * 10].key);
    }
    return result;
}

template <size_t N, typename T>
Key<N, T> center_of_mass(const Histogram<N, T>& hist, const std::set<size_t>& cluster) {
    Key<N, double> result_d;
    for(size_t i = 0; i < N; ++i) {
        result_d[i] = 0.;
    }
    double sum = 0.;
    for(auto pixel = cluster.begin(); pixel != cluster.end(); ++pixel) {
        double w = hist[*pixel].count;
        for(size_t i = 0; i < N; ++i) {
            result_d[i] += double(hist[*pixel].key[i]) * w;
        }
        sum += w;
    }
    for(size_t i = 0; i < N; ++i) {
        result_d[i] /= sum;
    }
    Key<N, T> result;
    for(size_t i = 0; i < N; ++i) {
        result[i] = T(result_d[i]);
    }
    return result;
}

template <size_t N, typename T>
double KMeansIteration(const Histogram<N, T>& hist,
                         std::vector<Key<N, T> >& centers,
                         std::vector<std::set<size_t> >& clusters) {
    const size_t num_of_clusters = centers.size();
    clusters.clear();
    clusters = std::vector<std::set<size_t> >(num_of_clusters, std::set<size_t>());
    for(size_t color = 0; color < hist.size(); ++color) {
        double min_dist = distance(hist[color].key, centers[0]);
        size_t nearest_cntr = 0;

        for(size_t cntr = 1; cntr < num_of_clusters; ++cntr) {
            double dist = distance(hist[color].key, centers[cntr]);
            if(dist < min_dist) {
                min_dist = dist;
                nearest_cntr = cntr;
            }
        }

        clusters[nearest_cntr].insert(color);
    }

    double sum_shift_centers = 0.;
    for(size_t cntr = 0; cntr < num_of_clusters; ++cntr) {
        Key<N, T> new_center = center_of_mass(hist, clusters[cntr]);
        sum_shift_centers += distance(new_center, centers[cntr]);
        centers[cntr] = new_center;
    }
    return sum_shift_centers;
}
}