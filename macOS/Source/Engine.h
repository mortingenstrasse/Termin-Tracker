#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void tm_init(double rate);
void tm_cell(int track,int step,uint64_t notes);
void tm_cell_at(int pattern,int track,int step,uint64_t notes,int instrument,int velocity,int volume,int pan,int fx,int fxValue);
void tm_cell_at2(int pattern,int track,int step,uint64_t notes,int instrument,int velocity,int volume,int pan,int fx,int fxValue,int fx2,int fx2Value);
void tm_cell_at3(int pattern,int track,int step,uint64_t notes,int instrument,int velocity,int volume,int pan,int fx,int fxValue,int fx2,int fx2Value,int micro);
void tm_cell_at4(int pattern,int track,int step,uint64_t notes,int instrument,int velocity,int volume,int pan,int fx,int fxValue,int fx2,int fx2Value,int micro,int lengthQuarters);
void tm_set_eq(int bass,int midA,int midB,int high);
void tm_sample_octave(int slot,int octave);
void tm_dynamics(int source,int compOn,int sideOn,int threshold,int ratio,int attack,int release,int makeup,int duckThreshold,int duckDepth);
float tm_dynamics_gain_reduction(int which);
void tm_track_eq(int pattern,int track,int bass,int midA,int midB,int high);
void tm_step_eq(int pattern,int track,int step,int bass,int midA,int midB,int high);
void tm_track_sends(int pattern,int track,int reverb,int delay,int slicer);
void tm_master_sends(int reverb,int delay,int slicer);
void tm_mute(int track,int muted);
void tm_master(int volume);
void tm_track_volume(int track,int volume);
void tm_pattern_track_volume(int pattern,int track,int volume);
void tm_pattern_mute_mask(int pattern,int mask);
void tm_pattern_volume(int pattern,int channel,int volume);
float tm_meter_level(int meter);
int tm_meter_over(int meter);
int tm_playing_pattern(void);
void tm_preview(uint64_t notes,int instrument);
void tm_preview_region(int slot,int start,int end);
void tm_preview_stop(void);
int tm_preview_active(void);
void tm_midi_note(int note,int instrument,int velocity);
void tm_midi_panic(void);
uint32_t tm_record_position(void);
void tm_song_set(int index,int pattern,int length);
void tm_song_count(int count);
void tm_song_play(int on);
int tm_song_row(void);
void tm_start_step(int step);
void tm_select_pattern(int pattern,int length);
void tm_play(int on);
void tm_bpm(int bpm);
void tm_bpm_tenths(int bpm10);
void tm_omondeu(int slot,const int *values);
void tm_instrument_wave(int slot,int wave);
int tm_instrument_sample(int slot,const float *samples,int frames,double sampleRate,int root);
void tm_sample_config(int slot,int mode,int start,int end,const int *slices,int count,int position,int lengthMs,int shape,int direction,int spread);
void tm_sample_config_lfo(int slot,int mode,int start,int end,const int *slices,int count,int position,int lengthMs,int shape,int direction,int spread,int lfoMode,int lfoShape,int lfoSteps,int lfoDepth);
void tm_sample_config_scan(int slot,int mode,int start,int end,const int *slices,int count,int position,int lengthMs,int shape,int direction,int spread,int lfoMode,int lfoShape,int lfoSteps,int lfoDepth,int scanMode,const int *anchors);
int tm_sample_position(int slot);
void tm_capture_begin(void);int tm_capture_push(const float *left,const float *right,int frames);int tm_capture_count(void);int tm_capture_copy(float *out,int capacity);void tm_capture_peaks(float *low,float *high,int columns);
void tm_instrument_clear(int slot);
void tm_length(int length);
int tm_step(void);
void tm_render(float *left,float *right,int count);
void tm_fx_settings(int size,int decay,int predelay,int damping,int width,int reverbLevel,int division,int feedback,int delayDamping,int delayLevel,int pingpong);
void tm_export_tail(int on);
int tm_dsp_load(void);int tm_dsp_peak(void);int tm_dsp_overruns(void);int tm_dsp_frames(void);int tm_dsp_rate(void);void tm_dsp_reset(void);
#ifdef __cplusplus
}
#endif
