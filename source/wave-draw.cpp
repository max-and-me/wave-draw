// Copyright(c) 2024 Max And Me.

#include "mam/wave-draw/wave-draw.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace mam::wave_draw {

//------------------------------------------------------------------------
// Maybe this can be set dynamically during runtime
static constexpr size_t MIN_STEP_COUNT = 1;

// Line should be at least two pixels in height
static constexpr double MIN_LINE_HEIGHT = 2.;

//------------------------------------------------------------------------
template <typename Func>
static auto compute_buckets(const AudioBufferSpan& audio_buffer,
                            double num_samples_per_bucket,
                            Func&& func) -> void
{
    const auto samples_per_bucket = static_cast<size_t>(num_samples_per_bucket);
    const auto num_samples        = audio_buffer.size();
    const auto num_buckets        = num_samples / samples_per_bucket;

    // The step count increases the bigger the buffer is. This preserves details
    // when zoomed in but saves performance when buffer is very big. And 720 is
    // a good value for now. The minimum step count is 1 of course.
    const auto step_count = std::max(MIN_STEP_COUNT, samples_per_bucket / 720);

    auto max_sample_value = SampleType(0.);

    for (auto bucket_index = 0; bucket_index < num_buckets; bucket_index++)
    {
        const auto start_sample = bucket_index * samples_per_bucket;
        const auto end_sample =
            std::min(start_sample + samples_per_bucket, num_samples);
        for (auto i = start_sample; i < end_sample; i += step_count)
        {
            const auto abs_val = std::abs(audio_buffer[i]);
            max_sample_value   = std::max(max_sample_value, abs_val);
        }

        func(bucket_index, max_sample_value);

        max_sample_value = SampleType(0.);
    }
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
Drawer::Drawer(const AudioBufferSpan& audio_buffer, const double zoom_factor)
: audio_buffer(audio_buffer)
, zoom_factor(zoom_factor)
{
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

    compute_buckets(audio_buffer, zoom_factor, [&](size_t i, double value) {
        const auto line_height =
            std::max(height * CoordType(value), MIN_LINE_HEIGHT);
        const auto line_hight_half = line_height * 0.5;

        DrawData data;
        data.x      = CoordType(i) * x_offset;
        data.y      = y_center - line_hight_half;
        data.width  = line_width;
        data.height = line_height;

        func(data, i);
    });
}

//------------------------------------------------------------------------
} // namespace mam::wave_draw