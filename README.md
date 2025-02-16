# DY-Player component for ESPHome

Adds basic support for DY-Player audio modules.
Likely to work (untested) with DY-SV17F, DY-SV8F, DY-HV20T, DY-HV8F, DY-SV5W hardware.

:raised_hand: This is just a quick hack based on the source code of the [DF-Player component](https://esphome.io/components/dfplayer.html).
Basic functions (play, stop, volume, ...) work, all other things have been left untouched.
You are very welcome to finalize this.

## Usage

```
external_components:
  - source: github://hn/esphome-dyplayer
    components: [ dyplayer ]

uart:
  rx_pin: D5
  tx_pin: D6
  baud_rate: 9600

dyplayer:
```

## Documentation

See [DF-Player docs](https://esphome.io/components/dfplayer.html) and adapt as needed.
Some things may work, some certainly won't.

## Noise when nothing is being played

With the _apparently_ 10W/20W models (DY-HV20T, DY-HV8F) a quiet hiss is always audible when files are not playing.
These use an `AP2001D` amplifier (sometimes additionally labeled `DY2435` or `DY2244`),
which likely has no working `SD` pin to temporarily switch off (sleep) the chip.
In any case, this is not correctly connected to the `BUSY` pin of the audio chip on the PCB.
As no data sheet is available for the amplifier, none of this can be checked and the doubt can be raised
as to whether it really delivers as good a performance as described by the dealers.

The 5W models work with an `LTK5128D` amplifier, which is switched off correctly via the `SD` pin when not in use.

## Credits

- [Chris Snijder](https://github.com/SnijderC) for [detailed DY-Player documentation and code](https://github.com/SnijderC/dyplayer).
- [Guillermo Ruffino](https://github.com/glmnet) for the DF-Player component.
- [smoluks](https://github.com/smoluks/DY-SV17F) has [docs and schematics for DY-Players](https://github.com/smoluks/DY-SV17F).
- [Garry](https://garrysblog.com/) has done some [interesting research for the DF-Player](https://garrysblog.com/2022/06/12/mp3-dfplayer-notes-clones-noise-speakers-wrong-file-plays-and-no-library/).
