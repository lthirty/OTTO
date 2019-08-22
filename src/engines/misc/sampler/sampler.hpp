#pragma once

#include "core/engine/engine.hpp"

#include "util/iterator.hpp"

#include <AudioFile.h>
#include <Gamma/Filter.h>
#include <Gamma/SamplePlayer.h>
#include "util/dsp/AR.hpp"

#include "engines/misc/sends/sends.hpp"

#include "core/audio/waveform.hpp"

namespace otto::engines {

  using namespace core;
  using namespace core::engine;
  using namespace props;

  struct Sequencer;

  struct Sampler : MiscEngine<Sampler> {
    static constexpr util::string_ref name = "Sampler";
    struct Props {
      Property<std::string> file = "";
      Property<float> volume = {1, limits(0, 4), step_size(0.01)};
      Property<float> filter = {10, limits(1, 20), step_size(0.3)};
      Property<float> speed = {1, limits(-10, 10), step_size(0.01)};
      Property<float> fadein = {0, limits(0, 1), step_size(0.01)};
      Property<float> fadeout = {0, limits(0, 1), step_size(0.01)};
      Property<int> startpoint = {0, limits(0,1), step_size(30)};
      Property<int> endpoint = {0, limits(-1,0), step_size(30)};

      audio::Waveform waveform;
      std::vector<std::string> filenames;
      std::vector<std::string>::iterator file_it = filenames.begin();
      gam::Array<float> samplecontainer;
      float samplerate;
      int num_samples = 1;

      /// The error message loading the last sample
      std::string error = "Sample not loaded yet";

      engines::Sends send;

      DECL_REFLECTION(Props, file, volume, filter, speed, fadein, fadeout, startpoint, endpoint);
    } props;

    Sampler();

    void restart();
    void finish();

    float progress() const noexcept;

    ui::Screen& envelope_screen()
    {
      return *_envelope_screen;
    }

    float operator()() noexcept;

    void process(audio::AudioBufferHandle audio, bool triggered);

  protected:
    friend struct SamplerMainScreen;
    friend struct SamplerEnvelopeScreen;

    void load_file(std::string);
    gam::SamplePlayer<> sample;
    AudioFile<float> samplefile;
    bool note_on = false;

    AR<> env_;

    gam::Biquad<> _lo_filter;
    gam::Biquad<> _hi_filter;

    // WaveformView stuff (for drawing)
    int wfv_num_of_points = 300;
    audio::WaveformView wfv = props.waveform.view(wfv_num_of_points, sample.min(), sample.max() - 1);
    float x_scale_factor = 1;
    void update_scaling(int start, int end);
    void update_wf();
    void draw_waveform(ui::vg::Canvas &ctx, int start, int end);

    std::unique_ptr<ui::Screen> _envelope_screen;
  };

} // namespace otto::engines
