//#pragma once
#include "ChaCha20.h"
#include <stdio.h>

// -------------------------------------------------------------
// --------------------------Functions--------------------------
// -------------------------------------------------------------

// SF: setCell
void setCell(uint32 cell[], uint32 constant[], const uint32 key[], uint32 blockNumber[], uint32 nonce[]) {

	// L: setConstant, i = 0 -> 3
	for (int i = 0; i < 4; i++)
		// C: Assign cell[i] = constant[i]
		cell[i] = constant[i];

	// L: setkey, i = 4 -> 11
	for (int i = 4; i < 12; i++)

		// C: Assign cell[i] = key[i - 4]
		cell[i] = key[i - 4];

	// L: setBlockNumber, i = 12 -> 13 
	for (int i = 12; i < 14; i++)

		// C: Assign cell[i] = blockNumber[i - 12]
		cell[i] = blockNumber[i - 12];

	// L: setBlockNumber, i = 14 -> 15 
	for (int i = 14; i < 16; i++)

		// C: Assign cell[i] = nonce[i - 12]
		cell[i] = nonce[i - 14];

}

// SF: roundFunction
void roundFunction(uint8 keyStream[], uint32 cell[]) {
	// C-DE: x-array for storing 16 cells KSG input
	uint32 x[16];

	//L: for storing 16 cells KSG input
	for (int i = 0; i < 16; i++)
		x[i] = cell[i];

	//Loop for mixing in Round function-------------
	for (int i = 0; i < ROUNDS; i += 2)
	{
		//Odd rounds------------------------------
		QR(x[0], x[4], x[8], x[12]);
		QR(x[1], x[5], x[9], x[13]);
		QR(x[2], x[6], x[10], x[14]);
		QR(x[3], x[7], x[11], x[15]);
		//Even rounds-------------------------
		QR(x[0], x[5], x[10], x[15]);
		QR(x[1], x[6], x[11], x[12]);
		QR(x[2], x[7], x[8], x[13]);
		QR(x[3], x[4], x[9], x[14]);
	}

	//Loop for adding KSG table input with output table of Round function
	for (int i = 0, b = 0; i < 16; i++)
	{ 
		x[i] += cell[i];
		
		// Store keystream
		keyStream[b++] = x[i] & 0xFF;
		keyStream[b++] = (x[i] >> 8) & 0xFF;
		keyStream[b++] = (x[i] >> 16) & 0xFF;
		keyStream[b++] = (x[i] >> 24) & 0xFF;
	}
}

void finalXor(uint8 keyStream[], uint8 cell[], uint8 textInput[], uint8 textOutput[]) {
	
	// C: roundFunction
	roundFunction(keyStream, (uint32*)cell);

	// L: textInput and textOutput
	int j = 0;
	while (textInput[j])
	{
		// L: keyStream
		for (int i = 0; i < 64 && textInput[j]; i++)
		{

			// C: xor cell8 and textInput8
			textOutput[j] = (keyStream[i] ^ textInput[j]);
			j++;
		}		
	}
}

void chaChaXor(uint8 keyStream[], uint32 cell[], uint8 textInput[], uint8 textOutput[])
{
	// C: call setCell
	setCell(cell, constant, chachaKey, blockNumber, nonce);

	finalXor(keyStream, (uint8*)cell, textInput, textOutput);
}
