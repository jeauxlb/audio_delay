#include <AudioHacker.h>

#define DEBUG

unsigned int playbackBuf = 2048;
unsigned long int sampleRate;
unsigned int timer1Start;
volatile unsigned int timer1EndEven;
volatile unsigned int timer1EndOdd;
volatile long address = 0;
volatile byte addressChipNumber = 0;
unsigned int delaySeconds;
unsigned long int memorySize;

unsigned long lastDebugPrint = 0;
unsigned int readBuf[2];
unsigned int writeBuf;
boolean evenCycle = true;
volatile byte buffering = 1;

void setup() {
#ifdef DEBUG
	// connect to the serial port
	Serial.begin(115200);
#endif

	sampleRate = 30000;
	delaySeconds = 5;
	memorySize = (delaySeconds > (float)MAX_ADDR/20000) ? MAX_ADDR : delaySeconds * (long)20000;
	timer1Start = UINT16_MAX - (F_CPU / sampleRate);

	AudioHacker.begin();

#ifdef DEBUG
	Serial.print("memorySize = ");
	Serial.println(memorySize);
	Serial.print("sample rate = ");
	Serial.print(sampleRate);
	Serial.println();
#endif
}

void loop() {

#ifdef DEBUG
	if ((millis() - lastDebugPrint) >= 1000) {
		lastDebugPrint = millis();

		// Print the number of instruction cycles remaining at the end of the ISR.
		// The more work you try to do in the ISR, the lower this number will become.
		// If the number of cycles remaining reaches 0, then the ISR will take up
		// all the CPU time and the code in loop() will not run.

		Serial.print("even cycles remaining = ");
		Serial.print(UINT16_MAX - timer1EndEven);
		Serial.print("   odd cycles remaining = ");
		Serial.print(UINT16_MAX - timer1EndOdd);
		Serial.println();
		if (((UINT16_MAX - timer1EndEven) < 20) || (((UINT16_MAX - timer1EndOdd) < 20))) {
			Serial.println("WARNING: ISR execution time is too long. Reduce sample rate or reduce the amount of code in the ISR.");
		}
	}
#endif

}

ISR(TIMER1_OVF_vect) {
	TCNT1 = timer1Start;
	unsigned int signal;

	if(!buffering) {
		AudioHacker.writeDAC(playbackBuf);
	}

	// Read ADC
	signal = AudioHacker.readADC();
	
	if (evenCycle) {
		// we only write to memory on odd cycles, so buffer the sampled signal.
		writeBuf = signal;
		AudioHacker.readSRAMPacked(addressChipNumber, address, readBuf);
		playbackBuf = readBuf[0];
	} else {
		// Write two samples to SRAM
		AudioHacker.writeSRAMPacked(addressChipNumber, address, writeBuf, signal);

		address += 3;
		if (address > memorySize) {
			if (addressChipNumber == 0) {
				// proceed to the second SRAM chip
				address = 0;
				addressChipNumber = 1;
			} else {
				address = 0; // loop around to beginning of memory
				addressChipNumber = 0;
				buffering = 0;
			}
		}
		playbackBuf = readBuf[1];
	}

#ifdef DEBUG
	if (evenCycle) {
		timer1EndEven = TCNT1;
	} else {
		timer1EndOdd = TCNT1;
	}
#endif
	evenCycle = !evenCycle;
}


