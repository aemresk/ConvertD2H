/*
Date: 01/01/2017
Author:root
*/

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
//macros
#define GetHexValue_32(f) (GetHexValue((f), 32, 8)) 
#define GetDoubleValue_32(i) (GetDoubleValue((i), 32, 8))
#define GetHexValue_64(f) (GetHexValue((f), 64, 11))
#define GetDoubleValue_64(f) (GetDoubleValue((f), 64, 11))

uint64_t GetHexValue(long double f, unsigned bits, unsigned expBits)
{
	long double fNorm;
	int shift;
	long long sign, exp, significand;
	unsigned significandBits = bits - expBits - 1; // -1 for sign bit

	if (f == 0.0) return 0; // get this special case out of the way

	// check sign and begin normalization
	if (f < 0) { sign = 1; fNorm = -f; }
	else { sign = 0; fNorm = f; }

	// get the normalized form of f and track the exponent
	shift = 0;
	while(fNorm >= 2.0) { fNorm /= 2.0; shift++; }
	while(fNorm < 1.0) { fNorm *= 2.0; shift--; }
	fNorm = fNorm - 1.0;

	// calculate the binary form (non-float) of the significand data
	significand = fNorm * ((1LL<<significandBits) + 0.5f);

	// get the biased exponent
	exp = shift + ((1<<(expBits-1)) - 1); // shift + bias

	// return the final answer
	return (sign<<(bits-1)) | (exp<<(bits-expBits-1)) | significand;
}

long double GetDoubleValue(uint64_t i, unsigned bits, unsigned expBits)
{
	long double result;
	long long shift;
	unsigned bias;
	unsigned significandBits = bits - expBits - 1; // -1 for sign bit

	if (i == 0) return 0.0;

	// pull the significand
	result = (i&((1LL<<significandBits)-1)); // mask
	result /= (1LL<<significandBits); // convert back to float
	result += 1.0f; // add the one back on

	// deal with the exponent
	bias = (1<<(expBits-1)) - 1;
	shift = ((i>>significandBits)&((1LL<<expBits)-1)) - bias;
	while(shift > 0) { result *= 2.0; shift--; }
	while(shift < 0) { result /= 2.0; shift++; }

	// sign it
	result *= (i>>(bits-1))&1? -1.0: 1.0;

	return result;
}

int main(void)
{
	double d = 5.14159265359, d2;
	uint64_t di;

	di = GetHexValue_64(d);  // hex value 
	d2 = GetDoubleValue_64(di); // double value... converted hex value. 

	printf("double before : %.20lf\n", d); // it can change after point.  
	printf("double encoded: 0x%016" PRIx64 "\n", di);  // prix using Formatting.. 
	printf("double after  : %.20lf\n", d2); 
	return 0;
}
