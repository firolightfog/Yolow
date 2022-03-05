# Yolow
Modules for VCV Rack

![image](https://user-images.githubusercontent.com/34127628/156898284-8f956e4f-52ce-463b-87d8-10a2267cce81.png)

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
Select your favourite channel from a polyphonic input (see PIN). The upper section returns a monophonic output according to the slider. The lower section works the same way but the input at the bottom allows you to modify the slider on the fly. This additional input can create a simple 16-step sequencer from the module. It's only 2HP!

<img width="272" alt="image" src="https://user-images.githubusercontent.com/34127628/156900013-2fd81ae0-16ac-40fd-adf2-582d2e2ae03a.png">

### ChSel6
Select upto 6 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 6 in the context menu.

### ChSel8
Select upto 8 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 8 in the context menu. It's only 2HP!

-----

### PolyCopy10
As simple merge module with ten normalized inputs.  

### PolyCopy16
As simple merge module with sixteen normalized inputs.  

<img width="250" alt="image" src="https://user-images.githubusercontent.com/34127628/156899500-237c10e8-6976-4130-9836-cf5a6047b056.png">

### PolyExp10
As simple split module it returns the first ten channels of a polyphonic input. 
Optionally the returned channels can be set to `first 10` or `last 10` in the context menu.

-----

### SeqP16
### SeqP5

-----

### MKnob
A simple fix voltage provider module. The context menu offers
- Quantize: nope / octave / notes
- Range: `0V to 10V` or `-5V to 5V`
- Poly channels: limiting polyphonic output from 1 to 4 channels if needed

### RandVolt10
- Without a clock input: it provides ten fix voltages (1V-10V).
- With a clock input: it provides random voltages. Practically a S&H unit with internal noise source.

Optionally the range can be set to `0V to 10V` or `-5V to 5V` in the context menu.

<img width="225" alt="image" src="https://user-images.githubusercontent.com/34127628/156899586-15b8dd43-4d7e-4e67-98d1-7b67e0b63bbd.png">

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
<img width="85" alt="image" src="https://user-images.githubusercontent.com/34127628/156899830-e201945d-faca-43bf-bc53-e8606f2f13d3.png">

