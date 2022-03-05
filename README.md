# Yolow
Modules for VCV Rack

![image](https://user-images.githubusercontent.com/34127628/156898284-8f956e4f-52ce-463b-87d8-10a2267cce81.png)

<!--
|Module|Short desc.|
| --- | --- |
|ChSel2.cpp|Select upto 2 of your favourite channels from a polyphonic input|
|ChSel6.cpp|Select upto 6 of your favourite channels from a polyphonic input|
|ChSel8.cpp|Select upto 8 of your favourite channels from a polyphonic input|
|PolyCopy10.cpp|Polyphonic merge with normalized input sockets in 2HP|
|PolyCopy16.cpp|Polyphonic merge with normalized input sockets in 4HP|
|PolyExp10.cpp|Polyphonic split in 2HP|
|SeqP16.cpp|4 step sequential switch to choose from 16 channels of a polyphonic input|
|SeqP5.cpp|4 step sequential switch to choose from 5 inputs|
|MKnob.cpp|Simple 4 channel fix voltage provider (4xmono & 1xpoly)|
|Slide16.cpp|A 16 channel fix voltage provider (1xpoly); with simple 16 step sequencer (1xmono)|
|Slide6.cpp|Simple 6 channel fix voltage provider with adjustable range (1xpoly)|
|TrigPrec2.cpp|Trigger sequence randomizer|
|TrigPrec6.cpp|Trigger sequence randomizer|
|tXOr.cpp|Clock divider with XOR output and CV inputs|
|9lives.cpp|Clock multiplier, clock randomizer, trigger sequencer, CV sequencer|
|RandVolt10.cpp|Fix voltage provider (1V, 2V, 3V, etc) or random stepped voltage|
|tHex.cpp :zzz:|[*deprecated*] A 16 step trigger sequencer with random options|
-->

|Module|Short desc.|
| --- | --- |
|[ChSel2](#ChSel2)|Select upto 2 of your favourite channels from a polyphonic input|
|[ChSel6](#ChSel6)|Select upto 6 of your favourite channels from a polyphonic input|
|[ChSel8](#ChSel8)|Select upto 8 of your favourite channels from a polyphonic input|
|[PolyCopy10](#PolyCopy10)|Polyphonic merge with normalized input sockets in 2HP|
|[PolyCopy16](#PolyCopy16)|Polyphonic merge with normalized input sockets in 4HP|
|[PolyExp10](#PolyExp10)|Polyphonic split in 2HP|
|[SeqP16](#SeqP16)|4 step sequential switch to choose from 16 channels of a polyphonic input|
|[SeqP5](#SeqP5)|4 step sequential switch to choose from 5 inputs|
|[MKnob](#MKnob)|Simple 4 channel fix voltage provider (4xmono & 1xpoly)|
|[RandVolt10](#RandVolt10)|Fix voltage provider (1V, 2V, 3V, etc) or random stepped voltage|
|[Slide6](#Slide6)|Simple 6 channel fix voltage provider with adjustable range (1xpoly)|
|[Slide16](#Slide16)|A 16 channel fix voltage provider (1xpoly); with simple 16 step sequencer (1xmono)|
|[TrigPrec2](#TrigPrec2)|Trigger sequence randomizer|
|[TrigPrec6](#TrigPrec6)|Trigger sequence randomizer|
|[tXOr](#tXOr)|Clock divider with XOR output and CV inputs|
|[9lives](#9lives)|Clock multiplier, clock randomizer, trigger sequencer, CV sequencer|
|[tHex](#tHex):zzz:|*Deprecated.* A 16 step trigger sequencer with random options|

-----

### ChSel2
### ChSel6
### ChSel8

-----

### PolyCopy10
### PolyCopy16
### PolyExp10

-----

### SeqP16
### SeqP5

-----

### MKnob
### RandVolt10
- Without a clock input: it provides ten fix voltages (1V-10V).
- With a clock input: it provides random voltages. Practically a S&H unit with internal noise source.
- Optionally the range can be set to `0V to 10V` or `-5V to 5V` in the context menu.
### Slide6
### Slide16

-----

### TrigPrec2
### TrigPrec6

-----

### tXOr
### 9lives

-----

### tHex 
This module is *deprecated*:zzz:. Nibble Sequencer of Count Modula is built on the [same concept](https://github.com/countmodula/VCVRackPlugins/issues/89) but provides better features.
