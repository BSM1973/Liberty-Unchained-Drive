# UNCHAINED DRIVE — User manual
Liberty Music Software · v1.0.0 RC1 · Pre-commercial candidate

## Installation and quick start
Apple Silicon macOS: AU and VST3. Windows x64: VST3 and standalone application. Candidate installers require validation before distribution. Close the DAW, install the format, restart and rescan plugins. macOS locations: /Library/Audio/Plug-Ins/Components and /Library/Audio/Plug-Ins/VST3. Windows: standard system VST3 location. Do not delete project files when uninstalling.
Insert Unchained Drive on a track or bus. Start with moderate input, low Drive and 50% Mix. Adjust output gain; Auto Gain is a comparison aid, not a limiter.

## 1 — DRIVE
Ten circuits: Tube Heat (valves), Tape Fire (tape), Transistor Edge (asymmetry), Console Color, Fuzz Chaos, Diode Crush (clipping), Digital Destruction (quantization), Wavefolder X, Multiband Inferno and Hybrid Monster. Drive sets intensity; Input/Output adjust gains; Mix blends; Bias adjusts asymmetry; Tone, Color, Dynamics, Presence and Body shape the signal.
Circuit Parameters opens circuit-specific controls. Fuzz/Wavefolder Anti-Alias rounds discontinuities, changes timbre and may reduce spectral residual; legacy mode remains available. Hybrid offers A/B stages, separate drives, blend, series/parallel and reverse order. Auto Gain uses RMS or optional K-weighting; LINK shares correction in stereo/M-S, while Dual Mono remains independent.

## 2 — MULTIBAND INFERNO
Engage Inferno, choose three or five bands and drag crossover boundaries. Each band offers circuit, Drive, Mix, input, output, tone, color, dynamics, solo and mute. Linear phase uses a symmetric 513-tap FIR. Fixed additional delay is 256 samples plus maximum reported oversampling latency.

## 3 — PRECISION TONE LAB
PRE EQ shapes input with HP/LP, shelves and two parametric bands. POST EQ has four parametric bands with frequency, gain, Q and on/off. Drag the plotted nodes. The displayed curve models post EQ.

## 4 — DYNAMIC HEAT
Signed depth modulates drive from the envelope; Transient/Sustain responds to rapid changes. Adjust compression, attack, release, threshold and sensitivity. A second envelope feeds modulation.

## 5 — MOTION MATRIX
Five source/destination/depth assignments. Two LFOs, two envelopes, steps and random are available. Set LFO rate, shape, tempo sync and division. Destinations include Drive, Bias, Tone, Color, Mix, Output and five band drives. Sources are evaluated per block and main controls are smoothed.

## 6 — SIGNAL INTELLIGENCE
Delayed input/output waveform, 2048-point FFT, static transfer curve, peak/RMS, LUFS M/S/I, LRA and estimated 4x true peak. RESET clears integrated session statistics; rolling M/S windows continue. DELTA compares processed output to a filtered delayed reference; time/phase effects may remain.
THD TEST 1 kHz measures an isolated static curve at 48 kHz, amplitude 0.3 and selected realtime oversampling. No test tone reaches the DAW. Not available for Hybrid/Multiband. Metering is not EBU certified.

## 7 — THE HEAT LIBRARY
100 factory presets in ten categories, favorites, search (press Enter or Search), paged browsing and user presets. Name and Save; Import/Export uses .udpreset. INIT restores defaults. Factory presets change tone; adjust output by ear.

## Shared controls
A/B and copy, undo/redo, bypass and French/English language selection (French default). Right-click an assignable control, choose MIDI Learn and move a controller; clear mappings through the menu. The host must route MIDI CC to the effect. Mappings are saved in plugin state. Automate exposed parameters through your DAW.
Oversampling OFF, 2x, 4x, 8x, 16x; independent offline quality when the host reports offline processing. Higher factors cost CPU. Latency is shown in samples. Avoid topology/quality switching during critical recording.

## Limitations and support
Meters are aids, not certification. Listening, visual and actual DAW checks remain required before commercial distribution. Report OS, DAW/version, format, sample rate, buffer, preset and reproducible steps.
© Liberty Music Software — BREAK THE RULES. IGNITE YOUR SOUND.
