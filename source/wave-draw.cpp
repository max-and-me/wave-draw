// Copyright(c) 2024 Max And Me.

#pragma once

#include "mam/wave-draw/wave-draw.h"
#include "gsl/span"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace mam::wave_draw {

//------------------------------------------------------------------------
static auto compute_buckets(const AudioBufferSpan& audio_buffer,
                            double num_samples_per_bucket) -> Buckets
{
    const auto sample_per_bucket = std::lround(num_samples_per_bucket);

    Buckets buckets;

    bool bucket_full       = false;
    auto max_sample_value  = SampleType(0.);
    const auto num_samples = audio_buffer.size();
    for (size_t i = 0; i < num_samples; i++)
    {
        const auto abs_val = std::abs(audio_buffer[i]);
        max_sample_value   = std::max(max_sample_value, abs_val);
        bucket_full        = (i + 1) % sample_per_bucket == 0;
        if (bucket_full)
        {
            buckets.push_back(max_sample_value);
            max_sample_value = SampleType(0.);
        }
    }

    // If there is a max_sample_value left, just put it into another bucket.
    if (!bucket_full)
        buckets.push_back(max_sample_value);

    return buckets;
}

//------------------------------------------------------------------------
auto compute_zoom_factor(const AudioBufferSpan& audio_buffer,
                         const CoordType view_width,
                         const CoordType line_width,
                         const CoordType spacing) -> CoordType
{
    const auto res = static_cast<double>(audio_buffer.size()) /
                     (view_width / (line_width + spacing));

    return std::max(res, 1.); // at least 1 pixel per sample
}

//------------------------------------------------------------------------
auto compute_view_width(const AudioBufferSpan& audio_buffer,
                        const double zoom_factor,
                        const CoordType line_width,
                        const CoordType spacing) -> double
{
    const auto num_samples = static_cast<double>(audio_buffer.size());
    const double res = (num_samples / zoom_factor) * (line_width + spacing);
    return res;
}

//------------------------------------------------------------------------
// Drawer
//------------------------------------------------------------------------
Drawer::Drawer() {}

//------------------------------------------------------------------------
auto Drawer::init(const AudioBufferSpan& audio_buffer, const double zoom_factor)
    -> Drawer&
{
    buckets = compute_buckets(audio_buffer, zoom_factor);
    return *this;
}

//------------------------------------------------------------------------
auto Drawer::setup_dimensions(const CoordType width, const CoordType height)
    -> Drawer&
{
    this->width  = width;
    this->height = height;
    return *this;
}

//------------------------------------------------------------------------
auto Drawer::setup_wave(const CoordType line_width, const CoordType spacing)
    -> Drawer&
{
    this->line_width = line_width;
    this->spacing    = spacing;
    return *this;
}

//------------------------------------------------------------------------
auto Drawer::draw(DrawFunc&& func) const -> void
{
    const auto x_offset = line_width + spacing;
    const auto y_center = height * 0.5;
    for (size_t i = 0; i < buckets.size(); ++i)
    {
        // Line should be at least two pixels in height
        static const auto MIN_LINE_HEIGHT = 2.;
        const auto line_height =
            std::max(height * CoordType(buckets[i]), MIN_LINE_HEIGHT);
        const auto line_hight_half = line_height * 0.5;

        DrawData data;
        data.x      = CoordType(i) * x_offset;
        data.y      = y_center - line_hight_half;
        data.width  = line_width;
        data.height = line_height;

        func(data);
    }
}

//------------------------------------------------------------------------
} // namespace mam::wave_draw