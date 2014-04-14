

#include "_ingetypes.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define CELL_HEIGHT 16
#define CELL_WIDTH 8
#define LETTER_NUMOF 256

struct tCoreResponseInfo {
    int mLastX;
    int mLastY;
    bool mNewCell;
    void *mCorePointer;
};

#define C_COREBIT_PERPIXEL  0
#define C_COREBIT_PERCHAR   1
#define C_COREBIT_2X2       2

#define C_COREBIT_16BGCOL   1


class tBitmapc {
public:
	int w;
	int h;
	int pixelbytes;
	ui8 *buffer;

	ui32 *pointer32;

	tBitmapc(int height, int width, int bpp);
	tBitmapc();
	~tBitmapc();

	void alloc(int height, int width, int bpp);
	void copyTo(tBitmapc &to);

	void set(int y, int x, int value);
	int get(int y, int x);

    void resetPointer();
	void resetPointer(int y, int x);
	int get32();
	void set32(int value);
} ;

class tCore {
public:
	tCore();
	
	void setDetectionPaletteAlternate(bool aYesNo);
	void setAlgBits(int aBits);
	void setFormatBits(bool a16bgColors);
	
	void setCharacterOptions(bool aDisableAlphanum, bool aDisableSymbols);
    void setupLetterTable();
    
    si32 loadBMP_bitmapc(ui8 *aBMPdata, tBitmapc &bitmap);

	bool doit(tBitmapc &aBMC, bool (*aRefreshCallback)(tCoreResponseInfo*));
	//int doitGradually(tBitmapc &bm);
	bool decideCell(tBitmapc &source, int y, int x, int *retLetter, int *retBGcol, int *retFGcol);
	bool decideCell2(tBitmapc &compareBuf2, int *retLetter, int *retBGcol, int *retFGcol);
	bool compare(tBitmapc &bma, ui8 *letterBMP, int &fcolR, int &fcolG, int &fcolB, int &bcolR, int &bcolG, int &bcolB, int &arBestDifference);
	void resetCount();
	
    bool (*mResponseCallback)(tCoreResponseInfo*);
   
    struct {
        bool mAlternateDetectPal;

        bool mDisableAlphaNumeric;
        bool mDisableSymbols;
        
        int mFormatBits;
        
        int mLaziness;
    } mConfig;
    
    int mAlgorithmBits;

    int mLetterTableSize;
    ui8 mLetterTable[256];

	int mColRegistry[16];
	int mAlterPal[16];
	int mColCompareReg_Red[16];
	int mColCompareReg_Green[16];
	int mColCompareReg_Blue[16];

	ui8 letterRegistry[LETTER_NUMOF][CELL_HEIGHT][CELL_WIDTH];

	struct {
		ui8 letter;
		ui8 fgcol;
		ui8 bgcol;
	} cellmatrix[SCREEN_HEIGHT][SCREEN_WIDTH];

	int mCount_x;
	int mCount_y;
	//int last_count_x;
	//int last_count_y;
	
	tCoreResponseInfo mResponseInfo;

};

extern tCore Core;
