# Wave Draw

## Introduction

This library takes a buffer of audio samples and draws a simplfied version of the resulting waveform. Something you can see in e.g. Spotify.

## Getting started

Fetch the library in your project:

```
FetchContent_Declare(
    wave-draw
    GIT_REPOSITORY "https://github.com/max-and-me/wave-draw.git"
    GIT_TAG main
)

FetchContent_MakeAvailable(wave-draw)
```

Link to it afterwards to the target in which it is needed:

```
target_link_libraries(MyTarget
    PRIVATE
        wave-draw
)
```

### Example using VSTGUI

```
void WaveformView::draw(VSTGUI::CDrawContext* pContext,
                        const VSTGUI::CRect& viewSize)
{
    constexpr auto spacing    = 1.;
    constexpr auto line_width = 2.;

    const auto zoom_factor = wave_draw::compute_zoom_factor(
        fn_get_audio_buffer(), viewSize.getWidth(), line_width, spacing);

    auto drawFunc = [&](const wave_draw::DrawData& data) {
        constexpr CCoord ROUND_CORNER_RADIUS = 1.;

        VSTGUI::SharedPointer<VSTGUI::CGraphicsPath> obj =
        VSTGUI::owned(context.createRoundRectGraphicsPath(create_rect(data),
                                                      ROUND_CORNER_RADIUS));
        auto* graphics_path = context.createRoundRectGraphicsPath(create_rect(data), ROUND_CORNER_RADIUS);
        context.drawGraphicsPath(graphics_path);
    };
    wave_draw::Drawer()
        .init(get_audio_buffer(), zoom_factor)
        .setup_wave(line_width, spacing)
        .setup_dimensions(viewSize.getWidth(), viewSize.getHeight())
        .draw(drawFunc);
}
```




