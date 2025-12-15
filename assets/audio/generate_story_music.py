"""
Story Music Generator for Operation Starfall
Generates 3 mood-based music tracks using synthesized audio
"""

import wave
import struct
import math
import os

# Audio settings
SAMPLE_RATE = 44100
DURATION_SECONDS = 30  # 30 second loops

def generate_sine_wave(frequency, duration, sample_rate, volume=0.3):
    """Generate a sine wave at given frequency"""
    samples = []
    num_samples = int(sample_rate * duration)
    for i in range(num_samples):
        t = i / sample_rate
        sample = volume * math.sin(2 * math.pi * frequency * t)
        samples.append(sample)
    return samples

def generate_chord(frequencies, duration, sample_rate, volume=0.2):
    """Generate a chord from multiple frequencies"""
    samples = [0.0] * int(sample_rate * duration)
    for freq in frequencies:
        wave = generate_sine_wave(freq, duration, sample_rate, volume / len(frequencies))
        for i in range(len(samples)):
            samples[i] += wave[i]
    return samples

def apply_envelope(samples, attack=0.1, decay=0.1, sustain_level=0.7, release=0.2, sample_rate=44100):
    """Apply ADSR envelope to samples"""
    total = len(samples)
    attack_samples = int(attack * sample_rate)
    decay_samples = int(decay * sample_rate)
    release_samples = int(release * sample_rate)
    
    for i in range(total):
        if i < attack_samples:
            # Attack
            envelope = i / attack_samples
        elif i < attack_samples + decay_samples:
            # Decay
            decay_progress = (i - attack_samples) / decay_samples
            envelope = 1.0 - (1.0 - sustain_level) * decay_progress
        elif i > total - release_samples:
            # Release
            release_progress = (i - (total - release_samples)) / release_samples
            envelope = sustain_level * (1.0 - release_progress)
        else:
            # Sustain
            envelope = sustain_level
        samples[i] *= envelope
    return samples

def save_wav(filename, samples, sample_rate):
    """Save samples to WAV file"""
    # Normalize
    max_val = max(abs(min(samples)), abs(max(samples)))
    if max_val > 0:
        samples = [s / max_val * 0.8 for s in samples]
    
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_rate)
        
        for sample in samples:
            packed = struct.pack('h', int(sample * 32767))
            wav_file.writeframesraw(packed)
    
    print(f"Created: {filename}")

def generate_epic_music():
    """Generate epic orchestral-style music for prologue/interludes"""
    print("Generating epic music...")
    samples = []
    
    # Epic chord progression: C major -> G major -> Am -> F major
    chords = [
        [261.63, 329.63, 392.00],  # C major
        [392.00, 493.88, 587.33],  # G major
        [220.00, 261.63, 329.63],  # A minor
        [349.23, 440.00, 523.25],  # F major
    ]
    
    chord_duration = DURATION_SECONDS / 4
    
    for chord in chords:
        chord_samples = generate_chord(chord, chord_duration, SAMPLE_RATE, 0.4)
        # Add low bass
        bass = generate_sine_wave(chord[0] / 2, chord_duration, SAMPLE_RATE, 0.25)
        for i in range(len(chord_samples)):
            chord_samples[i] += bass[i]
        chord_samples = apply_envelope(chord_samples, 0.3, 0.2, 0.6, 0.5, SAMPLE_RATE)
        samples.extend(chord_samples)
    
    return samples

def generate_sad_music():
    """Generate somber music for game over story"""
    print("Generating sad music...")
    samples = []
    
    # Minor key progression: Am -> Dm -> Em -> Am
    chords = [
        [220.00, 261.63, 329.63],  # A minor
        [293.66, 349.23, 440.00],  # D minor
        [329.63, 392.00, 493.88],  # E minor
        [220.00, 261.63, 329.63],  # A minor
    ]
    
    chord_duration = DURATION_SECONDS / 4
    
    for chord in chords:
        # Slower, lower volume for somber feel
        chord_samples = generate_chord(chord, chord_duration, SAMPLE_RATE, 0.3)
        # Add very low bass for weight
        bass = generate_sine_wave(chord[0] / 4, chord_duration, SAMPLE_RATE, 0.15)
        for i in range(len(chord_samples)):
            chord_samples[i] += bass[i]
        chord_samples = apply_envelope(chord_samples, 0.5, 0.3, 0.4, 1.0, SAMPLE_RATE)
        samples.extend(chord_samples)
    
    return samples

def generate_triumph_music():
    """Generate triumphant fanfare for victory story"""
    print("Generating triumph music...")
    samples = []
    
    # Major key fanfare: C -> G -> C -> F -> G -> C
    chords = [
        [261.63, 329.63, 392.00],  # C major
        [392.00, 493.88, 587.33],  # G major
        [523.25, 659.25, 783.99],  # C major (higher)
        [349.23, 440.00, 523.25],  # F major
        [392.00, 493.88, 587.33],  # G major
        [523.25, 659.25, 783.99],  # C major (high, triumphant)
    ]
    
    chord_duration = DURATION_SECONDS / 6
    
    for i, chord in enumerate(chords):
        # Bright, energetic
        chord_samples = generate_chord(chord, chord_duration, SAMPLE_RATE, 0.45)
        # Add punchy bass
        bass = generate_sine_wave(chord[0] / 2, chord_duration, SAMPLE_RATE, 0.3)
        for j in range(len(chord_samples)):
            chord_samples[j] += bass[j]
        
        # Faster attack for energy
        chord_samples = apply_envelope(chord_samples, 0.1, 0.1, 0.8, 0.3, SAMPLE_RATE)
        samples.extend(chord_samples)
    
    return samples

def main():
    output_dir = "assets/audio"
    os.makedirs(output_dir, exist_ok=True)
    
    print("=== Story Music Generator ===")
    print(f"Generating {DURATION_SECONDS} second tracks...")
    
    # Generate and save each track
    epic_samples = generate_epic_music()
    save_wav(os.path.join(output_dir, "story_epic.wav"), epic_samples, SAMPLE_RATE)
    
    sad_samples = generate_sad_music()
    save_wav(os.path.join(output_dir, "story_sad.wav"), sad_samples, SAMPLE_RATE)
    
    triumph_samples = generate_triumph_music()
    save_wav(os.path.join(output_dir, "story_triumph.wav"), triumph_samples, SAMPLE_RATE)
    
    print("\n=== All story music generated! ===")
    print("Files created in assets/audio/:")
    print("  - story_epic.wav (Prologue & Interludes)")
    print("  - story_sad.wav (Mission Failed)")
    print("  - story_triumph.wav (Victory)")

if __name__ == "__main__":
    main()
