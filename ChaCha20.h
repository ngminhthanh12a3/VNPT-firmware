#ifndef _ChaCha20_
#define _ChaCha20_

//#include <assert.h>
//#include <stddef.h>
//#include <stdint.h>
#include <string.h>

#ifdef __cplusplus 
extern "C" {
#endif

// -----------------
// -----Include-----
// -----------------

// -----INC: Private-----
#include "HeaderPublic.h"

// --------------------------------------------
// -------------------Define-------------------
// --------------------------------------------

// D: Number of Rounds for Round Function
#define ROUNDS 20

// D: --------Quarter Round function-----
#define QR(a, b, c, d) (			\
	a += b,  d ^= a,  d = ROTL(d,16),	\
	c += d,  b ^= c,  b = ROTL(b,12),	\
	a += b,  d ^= a,  d = ROTL(d, 8),	\
	c += d,  b ^= c,  b = ROTL(b, 7))

// D: --------------Rotate left function--------------
#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))

// D: keySize, constantSize, nonceSize,	blockSize, keyStreamSize, cellSize
#define keySize 8
#define constantSize 4
#define nonceSize 2
#define blockNumberSize 2
#define keyStreamSize 64
#define cellSize 16

// --------------------------------------------------------------------------------
// -------------------------------ChaChaC.cpp Declare------------------------------
// --------------------------------------------------------------------------------

// ---------------------------------C-DE: constant--------------------------------
static uint32 constant[constantSize] = { 0xCDD73CF5, 0xC1ADC3FD, 0x1A3CCFE5, 0x20434F4E };

// ------------------------------C-DE: key-----------------------------
static const uint32 chachaKey[keySize] = { 0x564E5054, 0x20506861, 0x73652033, 0x20536563,
						0x72657420, 0x6B657920, 0x62792054, 0x68616E68 };

// ----------------------C-DE: blockNumber---------------------
static uint32 blockNumber[blockNumberSize] = { 0x247E48ED, 0x0537E91E };

// -------------------C-DE: nonce------------------
static uint32 nonce[nonceSize] = { 0xC26D2B4F, 0x8AF1CC40 };

// ----C-DE: cell-----
static uint32 cell[cellSize];

// C-DE: keyStream
static uint8 keyStream[keyStreamSize] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// -------------------------------------------------------------
// --------------------------Functions--------------------------
// -------------------------------------------------------------

void chaChaXor(uint8 keyStream[], uint32 cell[], uint8 textInput[], uint8 textOutput[]);

#ifdef __cplusplus 
}
#endif 

#endif // !_ChaCha20_
