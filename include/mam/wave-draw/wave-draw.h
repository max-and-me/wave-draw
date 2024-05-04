// Copyright(c) 2024 Max And Me.

#pragma once

#if __cplusplus >= 202002L
#include <span>
#else
#include "gsl/span"
#endif
#include <cstdint>
#include <functional>

#include <vector>

namespace mam::wave_draw {

//------------------------------------------------------------------------
using CoordType = double;
struct DrawData
{
    CoordType x      = 0.;
    CoordType y      = 0.;
    CoordType width  = 0.;
    CoordType height = 0.;
};

using SampleType = float;
#if __cplusplus >= 202002L
using AudioBufferSpan = std::span<const SampleType>;
#else
using AudioBufferSpan = gsl::span<const SampleType>;
#endif
using DrawFunc = std::function<void(const DrawData&, size_t)>;
using Buckets  = std::vector<float>;

//------------------------------------------------------------------------
// Drawer
//------------------------------------------------------------------------
class Drawer
{
public:
    //--------------------------------------------------------------------
    Drawer(const AudioBufferSpan& audio_buffer, const double zoom_factor);

    auto setup_wave(const CoordType line_width, const CoordType spacing)
        -> Drawer&;

    auto setup_dimensions(const CoordType width, const CoordType height)
        -> Drawer&;

    auto draw(DrawFunc&& func) const -> void;

    //--------------------------------------------------------------------
private:
    CoordType line_width = 0.;
    CoordType spacing    = 0.;
    CoordType width      = 0.;
    CoordType height     = 0.;

    const AudioBufferSpan& audio_buffer;
    const double zoom_factor = 0.;
};

//------------------------------------------------------------------------
// Calculate zoom factor from a given view width
auto compute_zoom_factor(const AudioBufferSpan& audio_buffer,
                         const CoordType view_width,
                         const CoordType line_width,
                         const CoordType spacing) -> double;

// Calculate view width from a given zoom factor
auto compute_view_width(const AudioBufferSpan& audio_buffer,
                        const double zoom_factor,
                        const CoordType line_width,
                        const CoordType spacing) -> CoordType;
//------------------------------------------------------------------------
} // namespace mam::wave_draw