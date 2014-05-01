
#include "Core.h"

//#include <fstream>
#include <QImage>

using namespace std;

tCore Core;


tBitmapc::tBitmapc(int height, int width, int bpp) {
	alloc(height, width, bpp);
}

tBitmapc::tBitmapc() {
	buffer = 0;
}

tBitmapc::~tBitmapc() {
	if (buffer) {
		delete buffer;
	}
}

void tBitmapc::alloc(int height, int width, int bpp) {
	h = height;
	w = width;
	bpp = bpp;
	pixelbytes = (bpp / 8);
	buffer = new ui8[((height * width) * pixelbytes)];
}

void tBitmapc::copyTo(tBitmapc &to) {
	int bufferSz = ((h * w) * pixelbytes);
	to.buffer = new ui8[bufferSz];
	memcpy(to.buffer, buffer, bufferSz);
	to.w = w;
	to.h = h;
	to.pixelbytes = pixelbytes;
}

void tBitmapc::set(int y, int x, int value) {

	if (y >= h) {return;}
	if (x >= w) {return;}

	switch (pixelbytes) {
	case 3:
		{
		ui16 *base = (ui16*) (buffer + (((y * w) + x) * 3));
		*base = value & 0xFFFF;
		*((ui8*)base+2) = (ui8) (value >> 16) & 0xFF;
		break;
		}
	case 4:
		*(int*)(buffer + (((y * w) + x) * 4)) = value;
		break;
	}
}

int tBitmapc::get(int y, int x) {

	if (y >= h) {return 0;}
	if (x >= w) {return 0;}

	int value;
	switch (pixelbytes) {
	case 3:
		{
		ui16 *base = (ui16*) (buffer + (((y * w) + x) * 3));
		value = *base;
		value |= (*((ui8*)base+2) << 16);
		}
		break;
	case 4:
		value = *(int*)(buffer + (((y * w) + x) * 4));
		break;
	}
	return (value);
}

void tBitmapc::resetPointer() {
    resetPointer(0, 0);
}

void tBitmapc::resetPointer(int y, int x) {
	switch (pixelbytes) {
    case 4:
		pointer32 = (ui32*) (buffer + (((y * w) + x) * 4));
		break;
	}
}

int tBitmapc::get32() {
	return (*pointer32++);
}

void tBitmapc::set32(int value) {
	*pointer32++ = value;
}


//***********************************************************


tCore::tCore() {

	QImage vChart;
	vChart.load("chart.bmp", "BMP");
	
	for (int chr = 0; chr < 256; chr++) {
		int rowbase = (chr / 80);
		int row_pixel = rowbase * 16;
		int colbase = chr - (rowbase * 80);
		int col_pixel = colbase * 8;

		for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 8; x++) {

			if (vChart.pixel(col_pixel + x, row_pixel + y) & 0xFFFFFF) {
				letterRegistry[chr][y][x] = 1;
			} else {
				letterRegistry[chr][y][x] = 0;
			}
		}
		}

	}

	mColRegistry[0] = 0;
	mColRegistry[1] = 0x0000AA;
	mColRegistry[2] = 0x00AA00;
	mColRegistry[3] = 0x00AAAA;
	mColRegistry[4] = 0xAA0000;
	mColRegistry[5] = 0xAA00AA;
	mColRegistry[6] = 0xAA5500;
	mColRegistry[7] = 0xAAAAAA;
	mColRegistry[8] = 0x555555;
	mColRegistry[9] = 0x5555FF;
	mColRegistry[10] = 0x55FF55;
	mColRegistry[11] = 0x55FFFF;
	mColRegistry[12] = 0xFF5555;
	mColRegistry[13] = 0xFF55FF;
	mColRegistry[14] = 0xFFFF55;
	mColRegistry[15] = 0xFFFFFF;
	
	//Alternate palette for detection/comparison,
	//bright shades of colors have full contrast.
	//(more like Win 3.x in 16 color)
	//This is only used for conversion, other palette
	//is still used to display the image.
	mAlterPal[0] = 0;
	mAlterPal[1] = 0x0000AA;
	mAlterPal[2] = 0x00AA00;
	mAlterPal[3] = 0x00AAAA;
	mAlterPal[4] = 0xAA0000;
	mAlterPal[5] = 0xAA00AA;
	mAlterPal[6] = 0xAA5500;
	mAlterPal[7] = 0xAAAAAA;
	mAlterPal[8] = 0x555555;
	mAlterPal[9] = 0x0000FF;
	mAlterPal[10] = 0x00FF00;
	mAlterPal[11] = 0x00FFFF;
	mAlterPal[12] = 0xFF0000;
	mAlterPal[13] = 0xFF00FF;
	mAlterPal[14] = 0xFFFF00;
	mAlterPal[15] = 0xFFFFFF;
    
    setDetectionPaletteAlternate(true);
	
	resetCount();
	
	setCharacterOptions(false, false);
		
	mAlgorithmBits = C_COREBIT_2X2;

    mConfig.mCurrentFormat = eFormat_16f16b;
    
    mConfig.mLaziness = 0;
}

void tCore::setDetectionPaletteAlternate(bool aYesNo) {
    
    mConfig.mAlternateDetectPal = aYesNo;
    
    int *vPaletteRef = &mColRegistry[0];
	if (mConfig.mAlternateDetectPal == true) {
    	vPaletteRef = &mAlterPal[0];
    }
	for (int vColor = 0; vColor < 16; vColor++) {
        mColCompareReg_Blue[vColor] = vPaletteRef[vColor] & 0xFF;
        mColCompareReg_Green[vColor] = ((vPaletteRef[vColor] & 0xFF00) >> 8);
        mColCompareReg_Red[vColor] = ((vPaletteRef[vColor] & 0xFF0000) >> 16);
	}
}

//todo: this needs to be rewritten 
void tCore::setAlgBits(int aBits) {
    mAlgorithmBits = aBits;
}

void tCore::setCurrentFormat(int aFormatIndex) {
    mConfig.mCurrentFormat = aFormatIndex;
}

void tCore::setCharacterOptions(bool aDisableAlphanum, bool aDisableSymbols) {
    
    mConfig.mDisableAlphaNumeric = aDisableAlphanum;
    mConfig.mDisableSymbols = aDisableSymbols;
    
    //note: ideally this shouldn't be changed in the middle of a
    //conversion if "decideCell2" is being executed below in the stack.
    //But it's fine if it's in the interval between two characters.
    
    setupLetterTable();
}

void tCore::setupLetterTable() {
    
    int vLetterTableIndex = 0;
    
    for (int vLetterCount = 0; vLetterCount < 256; vLetterCount++) {
        bool vSkipLetter = false;
        if ((vLetterCount == 7) || ((vLetterCount >= 9) && (vLetterCount <= 13)) || ((vLetterCount >= 28) && (vLetterCount <= 31))) {
            vSkipLetter = true;
        }
        
        int vLetter = vLetterCount;
        
        //some hacky conversion-speed optimization..
        if ((vLetter >= 1) && (vLetter <= 3)) {
            vLetter = (vLetter - 1) + 176;
        } else if ((vLetter >= 176) && (vLetter <= 178)) {
                vLetter = (vLetter - 176) + 1;
            }

        if (vLetter == 4) {
            vLetter = 179;
        } else if (vLetter == 179) {
                vLetter = 4;
            }
            
        if ((vLetter >= 18) && (vLetter <= 21)) {
            vLetter = (vLetter - 18) + 220;
        } else if ((vLetter >= 220) && (vLetter <= 223)) {
                vLetter = (vLetter - 220) + 18;    
            }
        //..end of that stuff.
        
        if (mConfig.mDisableSymbols == true) {
            if ((vLetter >= 1) && (vLetter <= 6)) {vSkipLetter = true;}
            if (vLetter == 8) {vSkipLetter = true;}
            if ((vLetter >= 14) && (vLetter <= 15)) {vSkipLetter = true;}
            if ((vLetter >= 16) && (vLetter <= 17)) {vSkipLetter = true;}
            if ((vLetter >= 18) && (vLetter <= 21)) {vSkipLetter = true;}
            if ((vLetter >= 23) && (vLetter <= 27)) {vSkipLetter = true;}
            
            if ((vLetter >= 33) && (vLetter <= 47)) {vSkipLetter = true;}
            if ((vLetter >= 58) && (vLetter <= 64)) {vSkipLetter = true;}     
            if ((vLetter >= 91) && (vLetter <= 96)) {vSkipLetter = true;}   
            if ((vLetter >= 123) && (vLetter <= 126)) {vSkipLetter = true;}
            if (vLetter == 127) {vSkipLetter = true;}
            
            if ((vLetter >= 166) && (vLetter <= 168)) {vSkipLetter = true;}
            if ((vLetter >= 171) && (vLetter <= 175)) {vSkipLetter = true;}
            if ((vLetter >= 224) && (vLetter <= 253)) {vSkipLetter = true;}  
        }
        
        if (mConfig.mDisableAlphaNumeric == true) {
            if ((vLetter >= 48) && (vLetter <= 57)) {
                vSkipLetter = true;
            }
            if ((vLetter >= 65) && (vLetter <= 90)) {
                vSkipLetter = true;
            }
            if ((vLetter >= 97) && (vLetter <= 122)) {
                vSkipLetter = true;
            }
            if ((vLetter >= 128) && (vLetter <= 165)) {
                vSkipLetter = true;
            }
        }
        
        if (vSkipLetter == false) {
            mLetterTable[vLetterTableIndex] = vLetter;
            vLetterTableIndex++;
        }
    
    }
    
    mLetterTableSize = vLetterTableIndex;

}



bool tCore::doit(tBitmapc &aBMC, bool (*aRefreshCallback)(tCoreResponseInfo*)) {

    mResponseCallback = aRefreshCallback;

    memset(cellmatrix, 0, sizeof(cellmatrix));
    
    bool wReturn;

	for (int vY = 0; vY < SCREEN_HEIGHT; vY++) {
	for (int vX = 0; vX < SCREEN_WIDTH; vX++) {
		int letter;
		int bgcol;
		int fgcol;
		bool vContinue = decideCell(aBMC, vY, vX,
            &letter,
            &bgcol,
            &fgcol);
		cellmatrix[vY][vX].letter = letter;
		cellmatrix[vY][vX].bgcol = bgcol;
		cellmatrix[vY][vX].fgcol = fgcol;
		
        mResponseInfo.mLastX = vX;
        mResponseInfo.mLastY = vY;
		mResponseInfo.mNewCell = true;
		
		if (vContinue == false) {
            wReturn = false;
            goto BRK;
		}
	}
	}

    wReturn = true;
BRK:
    if (mResponseCallback) {
        mResponseInfo.mCorePointer = (void*)this;
        mResponseCallback(&mResponseInfo);
        mResponseInfo.mNewCell = false;
    };

    return wReturn;
}

void tCore::resetCount() {
	mCount_y = mCount_x = 0;
}

/*int tCore::doitGradually(tBitmapc &bm) {
	int letter;
	int bgcol;
	int fgcol;
	decideCell(bm, count_y, count_x, &letter, &bgcol, &fgcol);
	cellmatrix[count_y][count_x].letter = letter;
	cellmatrix[count_y][count_x].bgcol = bgcol;
	cellmatrix[count_y][count_x].fgcol = fgcol;

	last_count_y = count_y;
	last_count_x = count_x;

	count_x++;
	if (count_x == SCREEN_WIDTH) {
		count_y++;
		count_x = 0;
		if (count_y == SCREEN_HEIGHT) {
			resetCount();
			return 0;
		}
	}
	return 1;
}*/

inline bool tCore::decideCell(tBitmapc &source, int y, int x, int *retLetter, int *retBGcol, int *retFGcol) {
	tBitmapc a(CELL_HEIGHT, CELL_WIDTH, 32);

	int basey = (y * CELL_HEIGHT);
	int basex = (x * CELL_WIDTH);
	
	a.resetPointer();
	for (int y2 = 0; y2 < CELL_HEIGHT; y2++) {
	for (int x2 = 0; x2 < CELL_WIDTH; x2++) {
		a.set32(source.get(basey + y2, basex + x2));
	}
	}
	return decideCell2(a, retLetter, retBGcol, retFGcol);
	
}

inline bool tCore::decideCell2(tBitmapc &aCompareBMC2, int *apRetLetter, int *apRetBGcol, int *apRetFGcol) {


    if (mConfig.mCurrentFormat == eFormat_Mono_Win) { 
        *apRetFGcol = 0;
        *apRetBGcol = 15;
        return( decideCell_1bpp(aCompareBMC2, 0x000000, 0xFFFFFF, apRetLetter) );

    } else if (mConfig.mCurrentFormat == eFormat_Mono_DOS) {
        *apRetFGcol = 7;
        *apRetBGcol = 0;
        return( decideCell_1bpp(aCompareBMC2, 0xAAAAAA, 0x000000, apRetLetter) );

    } else {
    
        return( decideCell_4bpp(aCompareBMC2, apRetLetter, apRetBGcol, apRetFGcol) );
    }

}
 
bool tCore::decideCell_1bpp(tBitmapc &aCompareBMC2, int aInFGcol, int aInBGcol, int *apRetLetter) {
    
    int vBestLetter;
    int vBestDifference;
    
    vBestDifference = 0x300*CELL_HEIGHT*CELL_WIDTH;

	int bgcolR = (aInBGcol & 0xFF0000) >> 16;
	int bgcolG = (aInBGcol & 0xFF00) >> 8;
	int bgcolB = (aInBGcol & 0xFF);
	int fgcolR = (aInFGcol & 0xFF0000) >> 16;
	int fgcolG = (aInFGcol & 0xFF00) >> 8;
	int fgcolB = (aInFGcol & 0xFF);

    for (int vLetterTblIndex = 0; vLetterTblIndex < mLetterTableSize; vLetterTblIndex++) {
        int vLetter = mLetterTable[vLetterTblIndex];
    
        if (compare(aCompareBMC2,
            &letterRegistry[vLetter][0][0],
            fgcolR, fgcolG, fgcolB,
            bgcolR, bgcolG, bgcolB,
            vBestDifference)) {
                vBestLetter = vLetter;

                if (vBestDifference <= mConfig.mLaziness) {
                    bool vContinue;
                    if (mResponseCallback) {
                        mResponseInfo.mCorePointer = (void*)this;
                        vContinue = mResponseCallback(&mResponseInfo);
                        mResponseInfo.mNewCell = false;
                    };
                    *apRetLetter = vBestLetter;
                    return vContinue;
                }
        }
    }

    bool vContinue;
    if (mResponseCallback) {
        mResponseInfo.mCorePointer = (void*)this;
        vContinue = mResponseCallback(&mResponseInfo);
        mResponseInfo.mNewCell = false;
        if (vContinue == false) {
            *apRetLetter = vBestLetter;
            return false;
        };
    };
    
    *apRetLetter = vBestLetter;
    return true;
}

bool tCore::decideCell_4bpp(tBitmapc &aCompareBMC2, int *apRetLetter, int *apRetBGcol, int *apRetFGcol) {

	//for this spot in the image, check for similarity to each
	//combination of character, bg color and front color.
    //choose the combination which ends up having the appearence
    //that is least different to the spot in the source bitmap,
    //based on comparing differences in R, G and B values between
    //the source and destination pixels.
    //the exact kind of comparison depends on the algoritm chosen
    //(in the menus).

    //if a combination ends up looking 100% identical to the spot
    //in the source bitmap, choose it and quit the routine,
    //disregarding other combinations.

	struct {
		int letter;
		int fgcol;
		int bgcol;
		int difference;
	} vBest;

	vBest.difference = 0x300*CELL_HEIGHT*CELL_WIDTH;

	int bgcolR, bgcolG, bgcolB;
	int fgcolR, fgcolG, fgcolB;

    int vBgColors = (mConfig.mCurrentFormat == eFormat_16f16b) ? 16 : 8;
    
	for (int bgcol = 0; bgcol < vBgColors; bgcol++) {
        bgcolB = mColCompareReg_Blue[bgcol];
        bgcolG = mColCompareReg_Green[bgcol];
        bgcolR = mColCompareReg_Red[bgcol];
        for (int fgcol = 0; fgcol < 16; fgcol++) {
            if (bgcol == fgcol) {goto skip_fgcol;}
            fgcolB = mColCompareReg_Blue[fgcol];
            fgcolG = mColCompareReg_Green[fgcol];
            fgcolR = mColCompareReg_Red[fgcol];

            for (int vLetterTblIndex = 0; vLetterTblIndex < mLetterTableSize; vLetterTblIndex++) {
                int vLetter = mLetterTable[vLetterTblIndex];

                /*vRate = compare(aCompareBMC2,
                    &letterRegistry[vLetter][0][0],
                    fgcolR, fgcolG, fgcolB,
                    bgcolR, bgcolG, bgcolB,
                    best.difference);
                if (vRate < best.difference) {*/
                
                if (compare(aCompareBMC2,
                    &letterRegistry[vLetter][0][0],
                    fgcolR, fgcolG, fgcolB,
                    bgcolR, bgcolG, bgcolB,
                    vBest.difference)) {
                        vBest.letter = vLetter;
                        vBest.fgcol = fgcol;
                        vBest.bgcol = bgcol;
                        
                        if (vBest.difference <= mConfig.mLaziness) {
                            bool vContinue;
                            if (mResponseCallback) {
                                mResponseInfo.mCorePointer = (void*)this;
                                vContinue = mResponseCallback(&mResponseInfo);
                                mResponseInfo.mNewCell = false;
                            };
                            *apRetLetter = vBest.letter;
                            *apRetBGcol = vBest.bgcol;
                            *apRetFGcol = vBest.fgcol;
                            return vContinue;
                        }
                       
                }
    
            //skip_letter:
                {}
            } //vLetterTblIndex
                
        skip_fgcol:
            {}
        } //fgcol
        bool vContinue;
        if (mResponseCallback) {
            mResponseInfo.mCorePointer = (void*)this;
            vContinue = mResponseCallback(&mResponseInfo);
            mResponseInfo.mNewCell = false;
            if (vContinue == false) {
                *apRetLetter = vBest.letter;
                *apRetBGcol = vBest.bgcol;
                *apRetFGcol = vBest.fgcol;
                return false;
            };
        };
        
	} //bgcol

    
    *apRetLetter = vBest.letter;
    *apRetBGcol = vBest.bgcol;
    *apRetFGcol = vBest.fgcol;
    return true;
}

bool tCore::compare(tBitmapc &aBma, ui8 *letterBMP, int &fcolR, int &fcolG, int &fcolB, int &bcolR, int &bcolG, int &bcolB, int &arBestDifference) {

    if (!(mAlgorithmBits & C_COREBIT_PERCHAR)) {

        //Main algorithm (per-pixel difference)

        si32 difference = 0;
        si32 a;
        int vPointCount;
        int vPixels = (CELL_WIDTH*CELL_HEIGHT);
    
        si32 vLetCmpBlue, vLetCmpGrn, vLetCmpRed;

        ui8 *vpSrcBitmap = aBma.buffer;

        
        if (!(mAlgorithmBits & C_COREBIT_2X2)) {
            for (vPointCount = 0; vPointCount < vPixels; vPointCount++) {
                if (*letterBMP++) {
                    difference += (
                        (((a = si32(vpSrcBitmap[0]) - fcolB) < 0) ? -a : a) +
                        (((a = si32(vpSrcBitmap[1]) - fcolG) < 0) ? -a : a) +
                        (((a = si32(vpSrcBitmap[2]) - fcolR) < 0) ? -a : a)
                    );
                    if (difference >= arBestDifference) {return false;}
                } else {
                    /*difference += ((a = si32(vpSrcBitmap[0]) - bcolB) < 0) ? -a : a;
                    difference += ((a = si32(vpSrcBitmap[1]) - bcolG) < 0) ? -a : a;
                    difference += ((a = si32(vpSrcBitmap[2]) - bcolR) < 0) ? -a : a;*/
                    difference += (
                        (((a = si32(vpSrcBitmap[0]) - bcolB) < 0) ? -a : a) +
                        (((a = si32(vpSrcBitmap[1]) - bcolG) < 0) ? -a : a) +
                        (((a = si32(vpSrcBitmap[2]) - bcolR) < 0) ? -a : a)
                    );
                    
                }
                
                vpSrcBitmap += 4;

/*                if (*letterBMP++) {
                    a = si32(*vpSrcBitmap++) - fcolB;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    a = si32(*vpSrcBitmap++) - fcolG;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    a = si32(*vpSrcBitmap++) - fcolR;
                    if (a < 0) {difference += -a;} else {difference += a;}
                } else {
                    a = si32(*vpSrcBitmap++) - bcolB;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    a = si32(*vpSrcBitmap++) - bcolG;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    a = si32(*vpSrcBitmap++) - bcolR;
                    if (a < 0) {difference += -a;} else {difference += a;}
                }
                if (difference >= arBestDifference) {return false;}
                vpSrcBitmap++;*/

            }
            
        } else {
            //Mosaic (2x2 pixel) comparison
            int vPointCount_Y;
            int vPointCount_X;
            for (vPointCount_Y = 0; vPointCount_Y < 8; vPointCount_Y++) {
                for (vPointCount_X = 0; vPointCount_X < 4; vPointCount_X++) {
                    if (letterBMP[0]) {
                        vLetCmpBlue = fcolB;
                        vLetCmpGrn = fcolG;
                        vLetCmpRed = fcolR;
                    } else {
                        vLetCmpBlue = bcolB;
                        vLetCmpGrn = bcolG;
                        vLetCmpRed = bcolR;
                    }
                    if (letterBMP[1]) {
                        vLetCmpBlue += fcolB;
                        vLetCmpGrn += fcolG;
                        vLetCmpRed += fcolR;
                    } else {
                        vLetCmpBlue += bcolB;
                        vLetCmpGrn += bcolG;
                        vLetCmpRed += bcolR;
                    }
                    if (letterBMP[8]) {
                        vLetCmpBlue += fcolB;
                        vLetCmpGrn += fcolG;
                        vLetCmpRed += fcolR;
                    } else {
                        vLetCmpBlue += bcolB;
                        vLetCmpGrn += bcolG;
                        vLetCmpRed += bcolR;
                    }
                    if (letterBMP[9]) {
                        vLetCmpBlue += fcolB;
                        vLetCmpGrn += fcolG;
                        vLetCmpRed += fcolR;
                    } else {
                        vLetCmpBlue += bcolB;
                        vLetCmpGrn += bcolG;
                        vLetCmpRed += bcolR;
                    }
                    
                    a = si32(vpSrcBitmap[0] + vpSrcBitmap[4] + vpSrcBitmap[32] + vpSrcBitmap[36]) - vLetCmpBlue;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    a = si32(vpSrcBitmap[1] + vpSrcBitmap[5] + vpSrcBitmap[33] + vpSrcBitmap[37]) - vLetCmpGrn;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    a = si32(vpSrcBitmap[2] + vpSrcBitmap[6] + vpSrcBitmap[34] + vpSrcBitmap[38]) - vLetCmpRed;
                    if (a < 0) {difference += -a;} else {difference += a;}
                    if (difference >= arBestDifference) {return false;}
                    
                    vpSrcBitmap += 8;
                    letterBMP += 2;
                }
                vpSrcBitmap += 32;
                letterBMP += 8;
            }
        }
        
        if (difference < arBestDifference) {
            arBestDifference = difference;
            return true;
            
        } else {
            return false;
        }
  
    } else {
    
        //Alternate algorithm. (Per-character difference)
        
        si32 difference = 0;
        int vPixels = (CELL_WIDTH*CELL_HEIGHT);
   
        si32 vBlueDifference = 0;
        si32 vRedDifference = 0;
        si32 vGreenDifference = 0;
   
        ui8 *vpSrcBitmap = aBma.buffer;

        for (int vPixelCount = 0; vPixelCount < vPixels; vPixelCount++) {
            if (*letterBMP++ ) {
                vBlueDifference += (si32(*vpSrcBitmap++) - fcolB);
                vGreenDifference += (si32(*vpSrcBitmap++) - fcolG);
                vRedDifference += (si32(*vpSrcBitmap++) - fcolR);
            } else {
                vBlueDifference += (si32(*vpSrcBitmap++) - bcolB);
                vGreenDifference += (si32(*vpSrcBitmap++) - bcolG);
                vRedDifference += (si32(*vpSrcBitmap++) - bcolR);
            }
            vpSrcBitmap++;
        }
        difference = ((vBlueDifference < 0) ? -vBlueDifference : vBlueDifference);
        difference += ((vGreenDifference < 0) ? -vGreenDifference : vGreenDifference);
        difference += ((vRedDifference < 0) ? -vRedDifference : vRedDifference);    
    
        if (difference < arBestDifference) {
            arBestDifference = difference;
            return true;
        } else {
            return false;
        }
    
    }
}
