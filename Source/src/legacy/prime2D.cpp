#include <stdint.h>
#include <math.h>
#include <stdlib.h>

//Debugging
const uint16_t num3x5[16] = {0x7B6F, 0x749A, 0x73E7, 0x79A7, 0x49ED, 0x79CF, 0x7BCF, 0x24A7, 0x7AAF, 0x79EF,    0x5BEF, 0x7BC9, 0x724F, 0x7BE4, 0x72CF, 0x12CF}; //Used for the printInt functions
uint8_t* primeVRAM;
uint32_t primeRESX;
uint32_t primeRESY;

void setPrintInt(uint8_t* vram, uint32_t resX, uint32_t resY) {
	primeVRAM = vram;
	primeRESX = resX;
	primeRESY = resY;
}

#define bytesPerPixel 3
void plotPixel(uint32_t z, uint8_t color) {
	if (z > primeRESX * primeRESY) { // Out of bounds
	 return;	
	}
	uint8_t* fill = primeVRAM + (z * bytesPerPixel);
	for (uint8_t i = 0; i < bytesPerPixel; i++) {
		*fill = color;
		fill++;
	}
}

void printInt(int32_t in, uint8_t len, uint8_t base, uint32_t xC, uint32_t yC) { //integer to print, length, base 2-16 //Auto adjusts for signage
	if (primeVRAM == NULL) {
		return;
	}
    uint32_t off = xC + (primeRESX * yC);
    uint32_t v = off;
    len <<= 2; //len * 4
    uint32_t x2 = 5 + len;
    uint32_t jump = (primeRESX - 3) - x2;
    v += primeRESX;
    for (uint8_t dY = 1; dY < 6; dY++) {
        for (uint32_t dX = 0; dX < x2; dX += 4) {
            plotPixel(v,0x00);
            v += 4;
        }
        v += jump;
    }
    v = off;
    for (uint32_t dX = 0; dX < x2; dX++) {
        plotPixel(v,0x00);
        v++;
    }
    v = off + (primeRESX * 6); // 320 * 6
    for (uint32_t dX = 0; dX < x2; dX++) {
        plotPixel(v,0x00);
        v++;
    }

	v = off + (primeRESX) + 1;
	for (uint8_t dY = 0; dY < 5; dY++) {
		for (uint8_t dX = 0; dX < 3; dX++) {
			plotPixel(v,0x00);
			v++;
		}
		v += (primeRESX - 3);
	}
	
    uint32_t fill = (primeRESX * 3) + 1 + off; //Fills three pixels //1 + (3 * 320)
	if (in < 0) { plotPixel(fill,0xFF); } else { plotPixel(fill,0x00); } fill++; //If Negative
	if (in < 0) { plotPixel(fill,0xFF); } else { plotPixel(fill,0x00); } fill++; 
	if (in < 0) { plotPixel(fill,0xFF); } else { plotPixel(fill,0x00); }
	
    uint32_t j = abs(in);
    uint32_t powr = 1;
    for (uint32_t k = 0; k < len; k += 4) {
        uint16_t bitImage = num3x5[(j / powr) % base];
        uint32_t fill = off + (primeRESX + 1) + len - k;
        for (uint8_t y = 0; y < 5; y++) {
            for (uint8_t x = 0; x < 3; x++) {
				if (bitImage & 1) { plotPixel(fill,0xFF); } else { plotPixel(fill,0x00); } //White if true
                bitImage >>= 1;
                fill++;
            }
            fill += (primeRESX - 3); //320 - 3
        }
        powr *= base;
    }
}
void printUInt(uint32_t in, uint8_t len, uint8_t base, uint32_t xC, uint32_t yC) { //integer to print, length, base 2-16
	if (primeVRAM == NULL) {
		return;
	}
    uint32_t off = xC + (primeRESX * yC);
    uint32_t v = off;
    len <<= 2; //len * 4
    uint32_t x2 = 1 + len;
    uint32_t jump = (primeRESX - 3) - x2;
    v += primeRESX;
    for (uint8_t dY = 1; dY < 6; dY++) {
        for (uint32_t dX = 0; dX < x2; dX += 4) {
            plotPixel(v,0x00);
            v += 4;
        }
        v += jump;
    }
    v = off;
    for (uint32_t dX = 0; dX < x2; dX++) {
        plotPixel(v,0x00);
        v++;
    }
    v = off + (primeRESX * 6); // 320 * 6
    for (uint32_t dX = 0; dX < x2; dX++) {
		plotPixel(v,0x00);
        v++;
    }
    uint32_t powr = 1;
    for (uint32_t k = 0; k < len; k += 4) {
        uint16_t bitImage = num3x5[(in / powr) % base];
        uint32_t fill = off + (primeRESX - 3) + len - k;
        for (uint8_t y = 0; y < 5; y++) {
            for (uint8_t x = 0; x < 3; x++) {
                if (bitImage & 1) { plotPixel(fill,0xFF); } else { plotPixel(fill,0x00); } //White if true
                bitImage >>= 1;
                fill++;
            }
            fill += (primeRESX - 3); //320 - 3
        }
        powr *= base;
    }
}
#define fpINC(x) nextafter(x,x-copysign(1.0,x))
#define fpDEC(x) nextafter(x,x+copysign(1.0,x))
void printFloat(double in, uint8_t len, uint8_t dec, uint8_t base, uint32_t xC, uint32_t yC) {
	if (base == 10) {
		if (in >= pow(10.0,(double)(len))) {
			in = 99999999.99999999;
		} else if (in <= -pow(10.0,(double)(len))) {
			in = -99999999.99999999;
		}
	}
	if (len == 0) {
		xC += 5;
	} else {
		uint32_t integerPart = (uint32_t)floor(abs(in));
		printUInt(integerPart,len,base,xC,yC);
	}
	uint32_t decimalPart = (uint32_t)((abs(in) - floor(abs(in))) * pow(10.0,(double)dec));
	printUInt(decimalPart,dec,base,xC + (len * 4 + 2),yC);
	for (uint8_t i = 0; i < 7; i++) {
		plotPixel((xC + (len * 4 + 1)) + (primeRESX * (yC + (i))),0x00);
	}
	plotPixel((xC + (len * 4 + 1)) + (primeRESX * (yC + (5))),0xFF);
	if (in < 0.0) {
		if (len == 0) {
			xC -= 5;
		}
		for (uint32_t y = 0; y < 7; y++) {
			for (uint32_t x = 0; x < 5; x++) {
				plotPixel((xC + x) + (primeRESX * (yC + (y))),0x00);
			}
		}
		plotPixel((xC + 1) + (primeRESX * (yC + (3))),0xFF);
		plotPixel((xC + 2) + (primeRESX * (yC + (3))),0xFF);
		plotPixel((xC + 3) + (primeRESX * (yC + (3))),0xFF);
	}
};