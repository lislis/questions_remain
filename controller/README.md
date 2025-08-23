# Micro-controller code

The installation uses two Arduino Unos (or Nanos, I'll use the terms interchangeably) that communicate with each other over a shared serial channel.

Each Arduino takes on one big job, either reading touch from the canvas or playing back a sound file.

For a wiring diagram see the main documentation page.

The folder `uno_audio_simple` contains the code  to read the audio files from the SD card to the audio-jack.

The folder `uno_cap_mcp` contains the code to read capacitive touch, light up LEDs, and tell the other Arduino which file to play back.

The folder `_test` contains multiple test and experimentation sketches that have led to the final two skteches outlined above.
