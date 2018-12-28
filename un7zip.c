#include "LzmaDecode.h"

int un7zip(unsigned char *in, unsigned char *out, unsigned char *buf)
{
	unsigned int lzmaInternalSize;
	int lc, lp, pb;
	unsigned int outSize, outSizeProcessed;
	int ret;

	// 1) Read first byte of properties for LZMA compressed stream,
	// check that it has correct value and calculate three
	// LZMA property variables:
	unsigned char prop0 = in[0];
	if (prop0 >= ( 9 * 5 * 5))
	{
		return 1;
	}

	for (pb = 0; prop0 >= (9 * 5); pb++, prop0 -= (9 * 5));
	for (lp = 0; prop0 >= 9; lp++, prop0 -= 9);
	lc = prop0;

	// 2) Calculate required amount for LZMA lzmaInternalSize:
	lzmaInternalSize = (LZMA_BASE_SIZE + (LZMA_LIT_SIZE << (lc + lp))) * sizeof(CProb);

	// 3) Get decompressed data size:
	outSize = (in[8] << 24) | (in[7] << 16) | (in[6] << 8) | in[5];

	// 4) Decompress data:
	ret = LzmaDecode(buf, lzmaInternalSize, lc, lp, pb, in + 13, 0x2000000, out, outSize, &outSizeProcessed);

	*(unsigned int *)buf = outSizeProcessed;
	return ret;
}


