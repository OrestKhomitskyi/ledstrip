# ledstrip

Arduino sketch that drives a WS2811 addressable LED strip via [FastLED](https://github.com/FastLED/FastLED), remote-controlled over Bluetooth serial. Supports several built-in lighting effects plus power, color, and brightness control.

## Hardware

- Arduino Uno/Nano (or compatible AVR board)
- WS2811 LED strip, wired to digital pin 7 (94 LEDs by default)
- HC-05/HC-06-style Bluetooth serial module, wired to pins 10 (RX) / 11 (TX) via `SoftwareSerial`

## Dependencies

- [FastLED](https://github.com/FastLED/FastLED) library (install via Arduino Library Manager)
- `SoftwareSerial` and `avr/wdt` (bundled with the Arduino AVR core)

## Setup

1. Wire the LED strip's data line to pin 7, and power/ground per the strip's specs (WS2811 strips are commonly 12V — don't power the strip from the Arduino).
2. Wire the Bluetooth module: TX → pin 11, RX → pin 10 (add a voltage divider on RX if your module is 5V logic).
3. Open [DigitalReadSerial.ino](DigitalReadSerial.ino) in the Arduino IDE and install the FastLED library.
4. Adjust `NUM_LEDS` (line 5) if your strip isn't 94 LEDs.
5. Upload to the board.

## Bluetooth command protocol

Commands are newline-terminated ASCII strings sent to the Bluetooth serial link at 9600 baud:

| Command | Format | Description |
|---|---|---|
| `3` | `3` | Toggle power on/off |
| `4` | `4,RRR,BBB,GGG` | Set color — 3-digit R at offset 2, B at offset 6, G at offset 10 |
| `7` | `7,BBB` | Set brightness, 3-digit value 0–255 |
| `8` | `8,X` | Select mode `X` (see below) |

### Modes

| `X` | Effect |
|---|---|
| `0` | Cylon bounce |
| `1` | Color wipe |
| `2` | Rainbow cycle |
| `3` | Meteor rain |
| `4` | Fade in/out (red → yellow → magenta) |
| `5` | Solid color |
| `6` | Hue cycle (basic mode) |

## Known limitations

This is a work-in-progress sketch, not a finished product:

- `mySerial.begin(9600)` is commented out in `setup()`, so the Bluetooth link isn't actually initialized — commands won't be received until this is uncommented.
- `loop()` currently calls `rainbowCycle()` directly rather than dispatching through `runMode()`/`cur_mode`, so the `8,X` mode-select command has no effect yet even once Bluetooth is enabled.

## License

No license file is currently included in this repository.
