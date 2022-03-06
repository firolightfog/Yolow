# Yolow

Missing a few simple module from my VCV Rack patches I decided to code a few. But I would like to share them with you. If you successfully use any of them in your project or you like it please let me know. Your [support](https://paypal.me/salix0210?country.x=HU&locale.x=hu_HU) would be also appreciated.  

<img width="465" alt="image" src="https://user-images.githubusercontent.com/34127628/156902010-fd1c258a-e3e5-47fb-8410-9f27ac50ae27.png">

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
Select your favourite channel from a polyphonic input (see PIN). The upper section returns a single monophonic output as defined by the slider. The lower section works the same way but the input at the bottom allows you to modify the slider on the fly. This transforms the small 2HP module into a simple 16-step sequencer!

<img width="272" alt="image" src="https://user-images.githubusercontent.com/34127628/156900013-2fd81ae0-16ac-40fd-adf2-582d2e2ae03a.png">

### ChSel6
Select upto 6 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 6 in the context menu.

### ChSel8
Select upto 8 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 8 in the context menu. It's only 2HP!

-----

### PolyCopy10
A simple merge module with ten normalized inputs.  

### PolyCopy16
A simple merge module with sixteen normalized inputs.  

<img width="250" alt="image" src="https://user-images.githubusercontent.com/34127628/156899500-237c10e8-6976-4130-9836-cf5a6047b056.png">

### PolyExp10
A simple split module it returns the first ten channels of a polyphonic input. 
Optionally the returned channels can be set to `first 10` or `last 10` in the context menu.

-----

### SeqP5
A tricky 4-step sequencer that return one of the 5 input sources selected by the four knobs. To advance the step you need a clock (see CLK). Red LEDs indicate the active step, yellow LEDs indicate the currently selected input. Setting any of the knobs to zero will randomly select one of the input sources.

### SeqP16
A tricky 4-step sequencer that return one of the 16 channels of a polyphonic input (POLY IN). To advance the step you need a clock (see CLK). Red LEDs indicate the active step. Setting any of the knobs to zero will randomly select one of the input channels.

-----

### MKnob
A simple fix voltage provider module. The context menu offers
- Quantize: `nope` / `octave` / `notes`
- Range: `0V to 10V` or `-5V to 5V`
- Poly channels: limiting polyphonic output from 1 to 4 channels if needed

### RandVolt1 Setting any of the knobs to zero will randomly select one of the input sources.0
- Without a clock input: it provides ten fix voltages (1V-10V).
- With a clock input: it provides random voltages. Practically a S&H unit with internal noise source.

Optionally the range can be set to `0V to 10V` or `-5V to 5V` in the context menu.

<img width="225" alt="image" src="https://user-images.githubusercontent.com/34127628/156899586-15b8dd43-4d7e-4e67-98d1-7b67e0b63bbd.png">

### Slide6
A module to provide a six channel polyphonic output with the fixed voltages set by the six sliders. The two knobs (see MIN & MAX) on the top defines the range of voltages. The context menu offers
- Quantize: `nope` / `octave` / `notes`
- Poly channels: limiting polyphonic output from 1 to 6 channels if needed

### Slide16
A module to provide a sixteen channel polyphonic output with the fixed voltages set by the six sliders. The two knobs (see MIN & MAX) on the top defines the range of voltages. 

The module can also be used as a 16-step sequencer and return the melody as a monophonic CV (see OUT at the bottom). To step from slider to slider you can use a clock (see STEP input) or alternatively you can use a 0V-10V CV (see JUMP). 

Some keyboard shortcuts are enabled and the context menu offers
- Quantize: `nope` / `octave` / `notes`
- Poly channels: limiting polyphonic output from 1 to 16 channels if needed

-----

### TrigPrec2
It's a dual trigger precision ruiner. Each knob define the probability to skip triggers from the input sequence (see socket under the knob) and/or add a few trigger from the clock (see CLK on the top). The module follows the concept discussed [here](https://github.com/clone45/voxglitch/issues/74). 

### TrigPrec6
It's the bigger brother of TrigPrec6 with six inputs and outputs. Each knob define the probability to skip triggers from the input sequence (see socket left from the knob) and/or add a few trigger from the clock (see CLK on the top). 

-----

### tXOr
This is a clock modification module. Each knob sets the division of the clock signal (see CLK). The output is the XOR (exclusive OR) signal of the modified clocks. The concept is similar to JWM's 1Pattern but the tXOr inputs allow dynamic modification of the knobs. *(The module panel is subject to re-design.)*

### 9lives
First of all you obviously need a clock (CLK). The module can be used as 

- a buffer: CLK signal is sent unchanged to THR (through).
- a simple clock divider: CLK input is multiplied by the MUL knob and sent to the relevant output (see MUL at the bottom).
- a fix voltage provider: CVY return the CV of yellow slider of the currently selected block (CVW is the white slider, CVG is the green slider)
- a complex clock modifier: LIFE returns the clock signal multipled by the yellow slider value. Please note that the white slider acts as a probabilty setting and may force the clock to be quiet. The green slider serves as a multiplcation probability by forcing a single pulse or a multiplied rhythm.
- a super complex clock modifier: a 0V-10V CV (see JUMP) allows you to dynamically select one of the blocks. Hence the yellow, white, green parameters are changing accordingly.
- a 16-step sequencer: a 0V-10V CV (see JUMP) allows you to dynamically select one of the blocks. Hence the CVY, CVW, CVG outputs will provide a changing stepped CV.

The active block is also indicated by the yellow LEDs on the top. Some keyboard shortcuts are enabled to select blocks and randomize parameters.

The concept of the module is based on Sha#Bang! Modules Stochastic Sequencer Grid but the implementation is greatly simplified (clock, layout, operation).

-----

### tHex 
This module is *deprecated*:zzz:. Nibble Sequencer of Count Modula is built on the [same concept](https://github.com/countmodula/VCVRackPlugins/issues/89) but provides better features.

<img width="85" alt="image" src="https://user-images.githubusercontent.com/34127628/156899830-e201945d-faca-43bf-bc53-e8606f2f13d3.png">

