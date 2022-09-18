
// nested array:
// Family Name
// Popular name Interval steps *Systematic name Chords

// Based on Oliver Prehn's lecture
// Source: https://newjazz.dk/Compendiums/scales_of_harmonies.pdf
// Explanation: https://youtu.be/Vq2xt2D3e3E

std:string cFamily[8]={"Error","Family 1","Family 2","Family 3","Family 4","Family 5","Family 6","Family 7"};
std:string cLabels[8][7]={"Error",
	{
		{"Major","WWHWWWH", "Ionian", "Ma7"},
		{"Dorian","WHWWWHW", "Dorian", "mi7"},
		{"Phrygian","HWWWHWW", "Phrygian", "mi7"},
		{"Lydian","WWWHWWH", "Lydian", "Ma7"},
		{"Mixolydian","WWHWWHW", "Mixolydian", "7"},
		{"Natural minor","WHWWHWW", "Aeolian", "mi7"},
		{"Locrian","HWWHWWW", "Locrian", "mi7b5"},
	},{
		{"Altered / Super Locrian","HWHWWWW", "Ionian #1", "7alt / mi7b5"},
		{"Ascending mel. minor","WHWWWWH", "Dorian #7", "miMa7"},
		{"Dorian b2","HWWWWHW", "Phrygian #6", "mi7"},
		{"Lydian Augmented","WWWWHWH", "Lydian #5", "Ma7#5"},
		{"Lydian dominant","WWWHWHW", "Mixolydian #4", "7"},
		{"Aeolian dominant","WWHWHWW", "Aeolian #3", "7"},
		{"Half diminished","WHWHWWW", "Locrian #2", "mi7b5"},
	},{
		{"Major #5 / Major Aug.","WWHWHHWH", "Ionian #5", "Ma7#5"},
		{"Dorian #4","WHWHHWHW", "Dorian #4", "mi7"},
		{"Phrygian dominant","HWHHWHWW", "Phrygian #3", "7"},
		{"Lydian #2","WHHWHWWH", "Lydian #2", "Ma7"},
		{"Altered dominant bb7","HWHWWHWH", "Mixolydian #1", "dim7"},
		{"Harmonic minor","WHWWHWHH", "Aeolian #7", "miMa7"},
		{"Locrian b6","HWWHWHHW", "Locrian #6", "mi7b5"},
	},{
		{"Harmonic Major","WWHWHWHH", "Ionian b6", "Ma7"},
		{"Dorian b5","WHWHWHHW", "Dorian b5", "mi7b5"},
		{"Phrygian b4","HWHWHHWW", "Phrygian b4", "mi7 / 7"},
		{"Lydian b3","WHWHHWWH", "Lydian b3", "miMa7"},
		{"Mixolydian b2","HWHHWWHW", "Mixolydian b2", "7"},
		{"Lydian augmented #2","WHHWWHWH", "Aeolian b1", "Ma7#5 / dim7"},
		{"Locrian bb7","HWWHWHWH", "Locrian b7", "dim7"},
	},{
		{"Diminished","WHWHWHWH", "Diminished", "dim7"},
		{"Dominant diminished","HWHWHWHW", "Inverted diminished", "7"},
		// repeated
		{"Diminished","WHWHWHWH", "Diminished", "dim7"},
		{"Dominant diminished","HWHWHWHW", "Inverted diminished", "7"},
		{"Diminished","WHWHWHWH", "Diminished", "dim7"},
		{"Dominant diminished","HWHWHWHW", "Inverted diminished", "7"},
		{"Diminished","WHWHWHWH", "Diminished", "dim7"},
	},{
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},
		// repeated
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},	
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},
		{"Whole tone","WWWWWW", "Whole tone", "7 #5/b5"},
	},{
		{"Augmented","WHHWHHWHH", "Augmented", "Ma7"},
		{"Inverted Augmented","HWHHWHHWH", "Inverted Augmented", "6#5"},
		// repeated
		{"Augmented","WHHWHHWHH", "Augmented", "Ma7"},
		{"Inverted Augmented","HWHHWHHWH", "Inverted Augmented", "6#5"},
		{"Augmented","WHHWHHWHH", "Augmented", "Ma7"},
		{"Inverted Augmented","HWHHWHHWH", "Inverted Augmented", "6#5"},
		{"Augmented","WHHWHHWHH", "Augmented", "Ma7"},
	}};
	
	
