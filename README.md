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

## Credits

- [Chris Snijder](https://github.com/SnijderC) for [detailed DY-Player documentation and code](https://github.com/SnijderC/dyplayer).
- [Guillermo Ruffino](https://github.com/glmnet) for the DF-Player component.
