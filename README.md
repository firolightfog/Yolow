# Yolow

Missing a few simple module from my VCV Rack patches I decided to code a bit. But I would also like to share them with you. If you successfully use any of them in your project or you like it please let me know. Your [support](https://paypal.me/salix0210?country.x=HU&locale.x=hu_HU) would be also appreciated.  

<img width="430" alt="image" src="https://user-images.githubusercontent.com/34127628/176625955-11b6b1f9-8a63-46cd-a095-aa58d11fa532.png">

|Module|Short desc.|
| --- | --- |
|[ChSel2](#ChSel2)|Select upto 2 of your favourite channels from a polyphonic input|
|[ChSel6](#ChSel6)|Select upto 6 of your favourite channels from a polyphonic input|
|[ChSel8](#ChSel8)|Select upto 8 of your favourite channels from a polyphonic input|
|[PolyCopy10](#PolyCopy10)|Polyphonic merge with normalized input sockets in 2HP|
|[PolyCopy16](#PolyCopy16)|Polyphonic merge with normalized input sockets in 4HP|
|[PolyExp10](#PolyExp10)|Polyphonic split in 2HP|
|[Celesta](#Celesta)|three 8 step CV sequencers|
|[Celei](#Celesta)|single 8 step CV sequencer|
|[Sequin](#Sequin)|Queen of Sequencers|
|[PushMeSeq](#PushMeSeq)|variable trigger sequencer|
|[SeqP16](#SeqP16)|4 step sequential switch to choose from 16 channels of a polyphonic input|
|[SeqP5](#SeqP5)|4 step sequential switch to choose from 5 inputs|
|[MKnob](#MKnob)|Simple 4 channel fix voltage provider (4xmono & 1xpoly)|
|[RandVolt10](#RandVolt10)|Fix voltage provider (1V, 2V, 3V, etc) or random stepped voltage|
|[Slide6](#Slide6)|Simple 6 channel fix voltage provider with adjustable range (1xpoly)|
|[Slide16](#Slide16)|A 16 channel fix voltage provider (1xpoly); with simple 16 step sequencer (1xmono)|
|[NearMess](#NearMess)|An almost passive VCA and mixer|
|[TrigPrec1](#TrigPrec1)|Trigger sequence randomizer|
|[TrigPrec2](#TrigPrec2)|Trigger sequence randomizer|
|[TrigPrec6](#TrigPrec6)|Trigger sequence randomizer|
|[Enveloop](#Enveloop)|Looping envelope generator|
|[tXOr](#tXOr)|Clock divider with XOR output and CV inputs|
|[9lives](#9lives)|Clock multiplier, clock randomizer, trigger sequencer, CV sequencer|
|[tHex](#tHex):zzz:|*Deprecated.* A 16 step trigger sequencer with random options|

-----

### ChSel2
Select your favourite channel from a polyphonic input (see PIN). The upper section returns a single monophonic output as defined by the slider. The lower section works the same way but the input at the bottom allows you to modify the slider on the fly. This transforms the small 2HP module into a simple 16-step sequencer!

<img width="272" alt="image" src="https://user-images.githubusercontent.com/34127628/156900013-2fd81ae0-16ac-40fd-adf2-582d2e2ae03a.png">

2.1.6 update: if there's no PIN connected then fixed voltages are sent to the output. Unfortunately the knob labels don't properly reflect the behaviour.

### ChSel6
Select upto 6 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 6 in the context menu. Similarly to the suggestion [here](https://github.com/dbgrande/GrandeModular/issues/4).

### ChSel8
Select upto 8 of your favorite channels from a polyphonic input (see POLYIN). The result will be a polyphonic output (see OUT). The number of channels in the output can be set from 1 to 8 in the context menu. It's only 2HP!

2.1.6 update: if there's no PIN connected then fixed voltages are sent to the output. Unfortunately the knob labels don't properly reflect the behaviour. 

-----

### PolyCopy10
A simple merge module with ten normalized inputs. As suggested [here](https://github.com/dbgrande/GrandeModular/issues/3). 

### PolyCopy16
A simple merge module with sixteen normalized inputs.  

<img width="250" alt="image" src="https://user-images.githubusercontent.com/34127628/156899500-237c10e8-6976-4130-9836-cf5a6047b056.png">

### PolyExp10
A simple split module it returns the first ten channels of a polyphonic input. 
Optionally the returned channels can be set to `first 10` or `last 10` in the context menu.

-----

### Celesta
Celesta is a unquantized note sequencer based on [Andre_M's idea](https://community.vcvrack.com/t/ideas-for-any-interested-developers/17697?u=firolfo) in the VCV Community forum. Practically it's a module with three 8 step sequencers (A, B, C). Pattern length is adjustable. However sequencer B advances to the next step only when A has finished. Similary sequencer C takes the next step only if B has completed its pattern.

As an additional feature is that every step has a "precision" switch. Setting it to 
- `0` "mr perfect": the note is always played as expected
- `1` “missed notes”: occasionally plays another note from the same sequence
- `2` “sticky fingers”: occasionally keeps playing the previous note

On the left you find a “precision” knob to influence how often these anomalies happen.

2.1.7 update: reset behaviour was reshaped.

Since 2.1.8 Keyboard shortcuts are available:
- `i`: alternate values for all note knobs
- `r`: random values for all note knobs and step lengths

### Celei
Celei is the smaller sister of Celesta. Basically it's a simple unquantized note sequencer upto 8 steps with OCT (octave) and RNG (range) settings. A unique feature that you can get a clock signal once a selected step is active. (Setting it to step #1 would return a common EOC signal.) An external TRANSP (transpose) input is also accepted. Special thanks to Andre_M for all the contribution!

Since 2.1.8 Keyboard shortcuts are available:
- `1-8`: alternate value for the selected note knob
- `i`: alternate values for all note knobs
- `r`: random values for all note knobs and step lengths
- `u`: move knob values upwards
- `d`: move knob values downwards
- `c`/`v`: change between 'clock mode' and 'voltage mode'

<img width="500" alt="image" src="https://user-images.githubusercontent.com/34127628/177008993-9b384b4c-4ca0-4ae8-92a4-8ce2eed897db.png">

### Sequin
Queen of Sequencers. Slim, smooth, beautiful but noone really understands her. It delivers a CV sequence according to a clock but the pulse width (PW), notes can be modified by external LFOs. You can also enable or disable any clock pulse by an external CV (ENBL) or set the probability (PRB) for instant excitement.  

<img width="736" alt="image" src="https://user-images.githubusercontent.com/34127628/178427481-bc32b7a7-b5de-41bc-845c-4e7e6c22b7fb.png">

<img width="809" alt="image" src="https://user-images.githubusercontent.com/34127628/178427526-c4b0a55a-09ae-4452-90e1-8baa10a67ff5.png">

<img width="809" alt="image" src="https://user-images.githubusercontent.com/34127628/178427580-93929a80-1dbd-4be7-b3ec-12e1b89990e7.png">

<img width="809" alt="image" src="https://user-images.githubusercontent.com/34127628/178428799-2d8b313c-cc95-4329-a937-dd83713b903d.png">

<img width="808" alt="image" src="https://user-images.githubusercontent.com/34127628/178428839-246ea34c-fb59-4874-b48f-7c7f58ee6bc2.png">

### PushMeSeq
It is a variable trigger sequencer. It expects monophonic clock and reset input. Simple 'push and play' kind of sequencer with an additional knob on setting the mode. The available modes are:

- `0`: pick randomly from 48 steps (monophonic output)
- `1`: 1x48 steps (with monophonic output)
- `2`: 1x32 + 1x16 steps (with 2-channel polyphonic output)
- `3`: 3x16 steps (with 3-channel polyphonic output)
- `4`: 2x16 + 2x8 steps (with 4-channel polyphonic output)
- `5`: 1x16 + 4x8 steps (with 5-channel polyphonic output)
- `6`: 5x8+1x8 steps (with 6-channel polyphonic output)

Keyboard shortcuts are available:
- `1-6`: randomizes a certain zone
- `i`: inverts all buttons values

![image](https://user-images.githubusercontent.com/34127628/177716736-c3e5015c-f4cc-47f2-a232-451239b25a2f.png)

### SeqP5
A tricky 4-step sequencer that returns one of the 5 input sources according to the knob selections. To advance the step you need a clock (see CLK). Red LEDs indicate the active step, yellow LEDs indicate the currently selected input. Setting any of the knobs to zero will randomly forward one of the input sources.

### SeqP16
A tricky 4-step sequencer that returns one of the 16 channels of a polyphonic input (POLY IN). To advance the step you need a clock (see CLK). Red LEDs indicate the active step. Setting any of the knobs to zero will randomly select one of the input channels.

-----

### MKnob
A simple fix voltage provider module. The context menu offers
- Quantize: `nope` / `octave` / `notes`
- Range: `0V to 10V` or `-5V to 5V`
- Poly channels: limiting polyphonic output from 1 to 4 channels if needed
 
### RandVolt10
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

The 'poly channels' setting doesn't influence the sequence created by the JUMP input. But it does influence the number of steps available with the STEP input.

### NearMess
It is a very simple mixer that will combine two monophonic inputs (A and B) according to fixed ratios. This module is a tribute to my Takab [Nearness](https://www.modulargrid.net/e/takaab-nearness) Eurorack module that I dearly loved for it's simplicity and practicality.  

<img width="363" alt="image" src="https://user-images.githubusercontent.com/34127628/178104155-491bdf4e-58f1-40fb-af54-ab3bfbafdba2.png">

-----

### TrigPrec1
It's a simple trigger precision ruiner. Probability can be set by a knob or CV input. Probability allows to skip triggers from the input sequence (see IN socket) and/or add a few trigger from the clock (see CLK socket on the top). 

### TrigPrec2
It's a dual trigger precision ruiner. Each knob define the probability to skip triggers from the input sequence (see socket under the knob) and/or add a few trigger from the clock (see CLK on the top). The module follows the concept discussed [here](https://github.com/clone45/voxglitch/issues/74). 

### TrigPrec6
It's the bigger brother of TrigPrec6 with six inputs and outputs. Each knob define the probability to skip triggers from the input sequence (see socket left from the knob) and/or add a few trigger from the clock (see CLK on the top). 

-----

### Enveloop
This is an ADR (attack, decay, release) envelope generator with built in VCA. Separate knobs are available for A, D and R settings. The range can be set by a switch on the top, and the starting voltage for A is also definable. At the bottom there is a gate, a noise and an audio input allowing modification of the controll voltages. The module was inspired by the mighty [Arido](https://solcitomusica.blogspot.com/p/virtuales.html) VST of Solcitomusic. It was also motivating that I found only a few envelope generators with a built in VCA in the VCV Library. 

<img width="298" alt="image" src="https://user-images.githubusercontent.com/34127628/173998298-82dfa826-579d-4bc5-b02e-202e10c019b4.png">

### tXOr
This is a clock modification module. Each knob sets the division of the clock signal (see CLK). The output is the XOR (exclusive OR) signal of the modified clocks. The concept is similar to JWM's 1Pattern but the tXOr inputs allow dynamic modification of the knobs. 

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
This module is *deprecated*:zzz:. [Nibble Sequencer](https://library.vcvrack.com/CountModula/NibbleTriggerSequencer) of Count Modula is built on the [same concept](https://github.com/countmodula/VCVRackPlugins/issues/89) but provides better features.

<img width="85" alt="image" src="https://user-images.githubusercontent.com/34127628/156899830-e201945d-faca-43bf-bc53-e8606f2f13d3.png">
