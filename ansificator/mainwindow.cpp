#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Core.h"

#include <QDebug>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    #ifdef QT_DEBUG
        ui->wFileIn->setText("tree.bmp\0");
    #else
        ui->wFileIn->setText("trees.png\0");
    #endif
    
    init();
    initUI();
    
    ui->wStatus->setText("");
    
    initImage();
    initGfxView();
    
    setStatusMsgNyan("Welcome.. start by loading a file, you may enter it or browse for it.");

    resetStatusRegs();
    
    mDlgSrcFileFilter = "Images (*.png *.jpg *.bmp *.gif *.xbm);;All files (*.*)";
    mDlgSaveImgFilter = "Portable Network Graphics (*.png);;\
JPEG Image (*.jpg);;\
Windows Bitmap (*.bmp);;\
X11 Bitmap (*.xbm);;\
All files (*.*)";

}

void MainWindow::init() {
    mTargetSizeX = 640;
    mTargetSizeY = 400;

    mStopFlag = false;
}

void MainWindow::initUI() {
    ui->centralWidget->setLayout( ui->mainLayout );
}

void MainWindow::resetStatusRegs() {
    mStatus.mBrowsedEntered = false;
    mStatus.mLoadedAPic = false;
    mStatus.mInConversion = false;
    mStatus.mConvertedSomething = false;
    
    mStatus.mRelativeBrightLevel = 0;
    mStatus.mContrastLevel = 0;

    mStatus.mGivenMessages.clear();
}

void MainWindow::initGfxView() {

	ui->wGraphicsView->setScene( &mScene );
	ui->wGraphicsView->scale(1, 1);
}

void MainWindow::initImage() {

    //create a blank (black) image.
    mqSrcImage = QImage::QImage(mTargetSizeX, mTargetSizeY, C_QIMGFORMAT);
    mqSrcImage.fill(0);
    mqWorkImage = mqSrcImage;
    
    renderWorkQImage();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *aEvent) {
    mStopFlag = true;
    
}

void MainWindow::resizeEvent(QResizeEvent *aEvent) {
    if (ui->wWindowScale->isChecked()) {
        QTransform vTransformBasic;
        ui->wGraphicsView->setTransform(vTransformBasic, false);
        scaleImageRelative();
    }
}

void MainWindow::renderWorkQImage() {
    QPixmap vqPixmap;
    
    QImage *vImage = (ui->wANSIenable->isChecked() == true) ? &mqWorkImage : &mqSrcImage;
   
    vqPixmap.convertFromImage(*vImage);

    mScene.clear();
    mScene.addPixmap( vqPixmap );
}

bool MainWindow::setStatusMsgNyan(QString aMessage, int aParameter, bool aRepeat) {
    //return( setStatusMsg(aMessage, aParameter, aRepeat) );
    ui->wStatus->setText(QString());
    return false;
}

bool MainWindow::setStatusMsg(QString aMessage, int aParameter, bool aRepeat) {
    QString vPostFix = "";
    if (aParameter >= 0) {
        vPostFix = " :3";
    }
    QString vCompleteMessage = aMessage + vPostFix;
    
    if ((aRepeat == true) || (mStatus.mGivenMessages.contains(vCompleteMessage, Qt::CaseInsensitive) == false)) {

        ui->wStatus->setText(vCompleteMessage);
        
        if (aRepeat == false) {
            mStatus.mGivenMessages.append(
                vCompleteMessage
                );
        }
        return true;
    } else {
        ui->wStatus->setText(QString());
        return false;
    }
}

void MainWindow::restoreConvertButtonText() {
    ui->wConvertButton->setText("Convert!");
}

//todo: delete
void MainWindow::on_wConvert_clicked() {}

void MainWindow::on_wLoadButton_clicked()
{

	QImage wFileImage;
	if (wFileImage.load(ui->wFileIn->text()) == true) {

        //"fixes" images with transparency.
        //but in the future, may implement a transformation loop
        //that changes transparency to white (more common) instead of black,
        //and/or implement an option between black/white.
        
        /*if ((wFileImage.format() == QImage::Format_ARGB32) ||
            (wFileImage.format() == QImage::Format_ARGB32_Premultiplied)) {
                wFileImage = wFileImage.convertToFormat(QImage::Format_RGB32, Qt::ColorOnly);
            }*/
        wFileImage = wFileImage.convertToFormat(C_QIMGFORMAT, Qt::ColorOnly);

        if (ui->actionStretch_loaded_image->isChecked() == true) {
            if ((wFileImage.width() != mTargetSizeX) ||
                (wFileImage.height() != mTargetSizeY)) {
                Qt::AspectRatioMode vAspectOption =
                    (ui->actionKeep_aspect_ratio_on_stretch->isChecked() == true) ?
                        Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;
                
                wFileImage = wFileImage.scaled(mTargetSizeX, mTargetSizeY,
                    vAspectOption, Qt::SmoothTransformation);
            }
        }
        
        if ((wFileImage.width() == mTargetSizeX) &&
            (wFileImage.height() == mTargetSizeY)) {
            
            mqSrcImage = wFileImage;
        } else {
            mqSrcImage.fill(0);
            
            int vStartInSrcY = 0;
            int vStartInSrcX = 0;
            int vStartInDstY = 0;
            int vStartInDstX = 0;
            int vLoadAmountX = 0;
            int vLoadAmountY = 0;
            
            if (wFileImage.width() > mTargetSizeX) {
                int vExcessiveX = wFileImage.width() - mTargetSizeX;
                vStartInSrcX = (vExcessiveX / 2);
                vStartInDstX = 0;
                vLoadAmountX = mTargetSizeX;
            } else {
                int vFillX = mTargetSizeX - wFileImage.width();
                vStartInSrcX = 0;
                vStartInDstX = (vFillX / 2);
                vLoadAmountX = wFileImage.width();
            }
            
            if (wFileImage.height() > mTargetSizeY) {
                int vExcessiveY = wFileImage.height() - mTargetSizeY;
                vStartInSrcY = (vExcessiveY / 2);
                vStartInDstY = 0;
                vLoadAmountY = mTargetSizeY;
            } else {
                int vFillY = mTargetSizeY - wFileImage.height();
                vStartInSrcY = 0;
                vStartInDstY = (vFillY / 2);
                vLoadAmountY = wFileImage.height();
            }
            
            int vEndInDstY = vStartInDstY + vLoadAmountY;
            int vEndInDstX = vStartInDstX + vLoadAmountX;
            
            int vCurrentSrcY = vStartInSrcY;
            for (int vCurrentDstY = vStartInDstY; vCurrentDstY < vEndInDstY; vCurrentDstY++) {
                int vCurrentSrcX = vStartInSrcX;
            
                QRgb *vSrcRgbPtr = (QRgb*) wFileImage.constScanLine(vCurrentSrcY);
                vSrcRgbPtr += vCurrentSrcX;
                
                QRgb *vDstRgbPtr = (QRgb*) mqSrcImage.scanLine(vCurrentDstY);
                vDstRgbPtr += vStartInDstX;
                
                for (int vCurrentDstX = vStartInDstX; vCurrentDstX < vEndInDstX; vCurrentDstX++) {

                    *vDstRgbPtr++ = *vSrcRgbPtr++;
              
                }
                vCurrentSrcY++;
            }
            
        }

        wFileImage = QImage();  //finished loading, clear the source file from memory now.
        mqWorkImage = mqSrcImage;
        renderWorkQImage();
        //mLoaded = true;
        
        QString vFileName = ui->wFileIn->text();
        if ((mStatus.mLoadedAPic == false) && (mStatus.mBrowsedEntered == false) &&
            (vFileName.compare("disk.jpg", Qt::CaseInsensitive) == 0)) {
            setStatusMsgNyan(
            ""
            );
        } else {
            setStatusMsgNyan("The program is ready to \"Convert!\" the image.");
        }
        
        mStatus.mLoadedAPic = true;
        
        mStatus.mContrastLevel = 0;
        mStatus.mRelativeBrightLevel = 0;
       
        /*if (vFileName.contains("super mario", Qt::CaseInsensitive) ||
            vFileName.contains("supermario", Qt::CaseInsensitive) ||
            vFileName.contains("super-mario", Qt::CaseInsensitive)) {
            setStatusMsgNyan("It's-a-me, Mario!", -1);
        }*/
        
       
	} else {
        //setStatusMsgNyan("That.. didn't work for some reason." );
        setStatusMsg("The file couldn't be loaded.", -1, true);
	}
	
	
}

void MainWindow::on_wBrowseButton_clicked()
{
    QString vFileName = QFileDialog::getOpenFileName(
        this,
        "Choose any image file..",
        QString(),
        mDlgSrcFileFilter,
        0,
        0);
    
    
    if (vFileName.size() < 1) {
        
    } else {
        ui->wFileIn->setText( vFileName );
        setStatusMsgNyan( "Next, click \"Load!\".", -1 );
        
        if (ui->actionAutomatically_Load_after_Browse->isChecked() == true) {
            on_wLoadButton_clicked();
        }
    }
    mStatus.mBrowsedEntered = true;
    
}


MainWindow *gpResponseWnd = 0;
tCore *gpCorePointer = 0;

bool fResponse(tCoreResponseInfo *aCRI) {
    if (!gpResponseWnd) {return true;}
    gpCorePointer = (tCore*)aCRI->mCorePointer;
    
    if (aCRI->mNewCell == true) {
        aCRI->mNewCell = false;
        
        //a new cell is converted.
        //update the graphics.
        
        int vCell_Y = aCRI->mLastY;
        int vCell_X = aCRI->mLastX;
        
        int vLetter = gpCorePointer->cellmatrix[vCell_Y][vCell_X].letter;
        QRgb vFColRGB = gpCorePointer->mColRegistry[gpCorePointer->cellmatrix[vCell_Y][vCell_X].fgcol];
        QRgb vBColRGB = gpCorePointer->mColRegistry[gpCorePointer->cellmatrix[vCell_Y][vCell_X].bgcol];
        //vFColRGB = (vFColRGB & 0xFFFFFF) | 0xFF000000;
        //vBColRGB = (vBColRGB & 0xFFFFFF) | 0xFF000000;
    
        QImage &vpqDrawImage = gpResponseWnd->mqWorkImage;
    
        //todo: use bits() instead of setPixel().
        int vYbase = vCell_Y * CELL_HEIGHT;
        int vXbase = vCell_X * CELL_WIDTH;
        for (int vY = 0; vY < CELL_HEIGHT; vY++) {
            for (int vX = 0; vX < CELL_WIDTH; vX++) {
                if (gpCorePointer->letterRegistry[vLetter][vY][vX]) {
                    vpqDrawImage.setPixel(vXbase + vX, vYbase + vY, vFColRGB);
                } else {
                    vpqDrawImage.setPixel(vXbase + vX, vYbase + vY, vBColRGB);
                }
            }
        }

        gpResponseWnd->renderWorkQImage();
    }

    QEventLoop wEventLoop(gpResponseWnd);
    
    wEventLoop.processEvents();
    
    if (gpResponseWnd->mStopFlag) {
        gpResponseWnd->mStopFlag = false;
        return false;
    } else {
        return true;
    }
}

void MainWindow::on_wConvertButton_clicked()
{

    if (mStatus.mInConversion == true) {
        mStopFlag = true;
        return;
    }

        if (mStatus.mLoadedAPic == false) {
            setStatusMsgNyan("");
        }
    
	//if (mLoaded == true) {
		//CreateThread(0, 0, &ProcConvert, 0, 0, 0);
		
		//update options.. it's done either way when the menu options are toggled,
		//but it's done just to make sure that the settings in memory are consistent with
		//the initial menu settings.
		Core.setDetectionPaletteAlternate( ui->actionAlternate_detection_palette->isChecked() );
		Core.setCharacterOptions(
            ui->actionDisable_Alphanum->isChecked(),
            ui->actionDisable_Sym->isChecked()
        );
        
        /*Core.setCurrentFormat(
            ui->action80_x_25_16_16_color->isChecked()
        );*/
		
		mqWorkImage = mqSrcImage;
		renderWorkQImage();
		
        gpResponseWnd = this;
	
		ui->wConvertButton->setText("Stop!");
		
		mStatus.mInConversion = true;
		mStatus.mConvertedSomething = true;
		
        ui->wANSIenable->setChecked(true);
        
        setStatusMsgNyan("This should take less than a minute..");
		
		int vSrcWidth = mTargetSizeX;
		int vSrcHeight = mTargetSizeY;
        tBitmapc vBMC(vSrcHeight, vSrcWidth, 32);
        
        vBMC.resetPointer();
        for (int vPY = 0; vPY < vSrcHeight; vPY++) {
            for (int vPX = 0; vPX < vSrcWidth; vPX++) {
                
                QRgb vqRgb = mqSrcImage.pixel(vPX, vPY);
                vBMC.set32(
                    qBlue(vqRgb) |
                    (qGreen(vqRgb) << 8) |
                    (qRed(vqRgb) << 16));
                
            }
        }
        
        bool vCompleted = Core.doit(vBMC, &fResponse);
        
        if (vCompleted == true) {
            if ((mStatus.mContrastLevel == 0) && (mStatus.mRelativeBrightLevel == 0)) {
                if (!setStatusMsgNyan("Done! If you are unsatisfied, try \"Increase Contrast\", then convert again.", -1)) {
                    setStatusMsg("Done!", -1, true);
                };
            } else {
                setStatusMsg("Done!", -1, true);
            }
        }
        
        mStatus.mInConversion = false;
        restoreConvertButtonText();
        
	//)}
}

void MainWindow::scaleImageRelative() {
    QRect vGeo = ui->wGraphicsView->geometry();
    
    float vNewScaleY = (float(vGeo.height()-2) / mTargetSizeY);
    float vNewScaleX = (float(vGeo.width()-2) / mTargetSizeX);
 
    ui->wGraphicsView->scale(vNewScaleX, vNewScaleY);
}

void MainWindow::on_wWindowScale_toggled(bool checked)
{
    if (checked) {
        scaleImageRelative();
    } else {
        /*QTransform vTransform = ui->wGraphicsView->transform();
        float vOldScaleX = vTransform.m11();
        float vOldScaleY = vTransform.m22();
        ui->wGraphicsView->scale(2-vOldScaleX, 2-vOldScaleY);*/
        
        QTransform vTransformBasic;
        ui->wGraphicsView->setTransform(vTransformBasic, false);

    }
}

//todo: remove
void MainWindow::on_wANSIenable_clicked() {}

void MainWindow::on_wSaturPlus_clicked()
{
    mStatus.mRelativeBrightLevel = 0;

    QRgb *vBitsPtr = (QRgb*) mqSrcImage.bits();
    
    #define REALTYPE float
    REALTYPE vMultiply = 1.200;
    REALTYPE vTempest;
    
    for (int vPixelsY = 0; vPixelsY < mTargetSizeY; vPixelsY++) {
    for (int vPixelsX = 0; vPixelsX < mTargetSizeX; vPixelsX++) {

        int vNewRed = (int) ( ((vTempest = (((REALTYPE(qRed(*vBitsPtr)) - 128) * vMultiply) + 128)) < 0) ?
            0 : (vTempest >= 256) ? 255 : vTempest);
        int vNewGreen = (int) ( ((vTempest = (((REALTYPE(qGreen(*vBitsPtr)) - 128) * vMultiply) + 128)) < 0) ?
            0 : (vTempest >= 256) ? 255 : vTempest);
        int vNewBlue = (int) ( ((vTempest = (((REALTYPE(qBlue(*vBitsPtr)) - 128) * vMultiply) + 128)) < 0) ?
            0 : (vTempest >= 256) ? 255 : vTempest);
        
        *vBitsPtr++ = qRgb(vNewRed, vNewGreen, vNewBlue);
    }
    }    
    #undef REALTYPE
    
    if (mStatus.mContrastLevel < 20) {
        mStatus.mContrastLevel++;
    }
    setStatusMsg("Increased Contrast (Level " +
        QString::number(mStatus.mContrastLevel) + " / 20).",
        -1, true);
    
    mqWorkImage = mqSrcImage;
    renderWorkQImage();
}


#define BRIGHTDARK_RATE 13

void MainWindow::on_wEffDarken_clicked()
{
    mStatus.mContrastLevel = 0;

    QRgb *vBitsPtr = (QRgb*) mqSrcImage.bits();

    for (int vPixelsY = 0; vPixelsY < mTargetSizeY; vPixelsY++) {
    for (int vPixelsX = 0; vPixelsX < mTargetSizeX; vPixelsX++) {
    
        *vBitsPtr = qRgb( 
            (qRed(*vBitsPtr) >= BRIGHTDARK_RATE) ? (qRed(*vBitsPtr) - BRIGHTDARK_RATE) : 0,
            (qGreen(*vBitsPtr) >= BRIGHTDARK_RATE) ? (qGreen(*vBitsPtr) - BRIGHTDARK_RATE) : 0,  
            (qBlue(*vBitsPtr) >= BRIGHTDARK_RATE) ? (qBlue(*vBitsPtr) - BRIGHTDARK_RATE) : 0  
            );
        vBitsPtr++;
    }
    }
    
    mqWorkImage = mqSrcImage;
    renderWorkQImage();
    
    if (mStatus.mRelativeBrightLevel > 0) {
        mStatus.mRelativeBrightLevel = 0;
    }
    if (mStatus.mRelativeBrightLevel > -20) {
        mStatus.mRelativeBrightLevel--;
    }
    setStatusMsg("Darkened the image (relative level " + 
        QString::number(mStatus.mRelativeBrightLevel) + " / -20).",
        -1, true);
}

void MainWindow::on_wEffBrighten_clicked()
{

    mStatus.mContrastLevel = 0;
    
    QRgb *vBitsPtr = (QRgb*) mqSrcImage.bits();
    
    for (int vPixelsY = 0; vPixelsY < mTargetSizeY; vPixelsY++) {
    for (int vPixelsX = 0; vPixelsX < mTargetSizeX; vPixelsX++) {
        
        *vBitsPtr = qRgb(
            (qRed(*vBitsPtr) <= (255 - BRIGHTDARK_RATE)) ? (qRed(*vBitsPtr) + BRIGHTDARK_RATE) : 255,
            (qGreen(*vBitsPtr) <= (255 - BRIGHTDARK_RATE)) ? (qGreen(*vBitsPtr) + BRIGHTDARK_RATE) : 255,  
            (qBlue(*vBitsPtr) <= (255 - BRIGHTDARK_RATE)) ? (qBlue(*vBitsPtr) + BRIGHTDARK_RATE) : 255  
            );
        vBitsPtr++;
    }
    }
    
    mqWorkImage = mqSrcImage;
    renderWorkQImage();

    if (mStatus.mRelativeBrightLevel < 0) {
        mStatus.mRelativeBrightLevel = 0;
    }
    if (mStatus.mRelativeBrightLevel < 20) {
        mStatus.mRelativeBrightLevel++;
    }
    setStatusMsg("Brightened the image (relative level " + 
        QString::number(mStatus.mRelativeBrightLevel) + " / 20).",
        -1, true);
}



void MainWindow::on_wEffMono_clicked()
{

    mStatus.mContrastLevel = 0;
    mStatus.mRelativeBrightLevel = 0;
    
    if (mStatus.mLoadedAPic == false) {
        mqSrcImage.fill(QColor(
                qrand() & 0xFF,
                qrand() & 0xFF,
                qrand() & 0xFF));
        setStatusMsgNyan("");
        
    } else {
    
        QRgb *vBitsPtr = (QRgb*) mqSrcImage.bits();
        
        int vReg = 0;
        for (int vPixelsY = 0; vPixelsY < mTargetSizeY; vPixelsY++) {
        for (int vPixelsX = 0; vPixelsX < mTargetSizeX; vPixelsX++) {
            
            /**vBitsPtr = qRgb(
                vReg = ((qRed(*vBitsPtr) + qGreen(*vBitsPtr) + qBlue(*vBitsPtr)) / 3),
                vReg,
                vReg
                );*/
            
            vReg = qGray(*vBitsPtr);
            *vBitsPtr++ = qRgb(
                vReg,
                vReg,
                vReg
                );
        }
        }
    }
    
    mqWorkImage = mqSrcImage;
    renderWorkQImage();   
    
    if (mStatus.mLoadedAPic == true) {
        setStatusMsgNyan("Hint: You can revert any changes to the image by just clicking \"Load\" again.");
    }
}


void MainWindow::on_wANSIenable_toggled(bool aChecked)
{
    if (aChecked == true) {
        if (mStatus.mConvertedSomething == false) {
            setStatusMsgNyan(
            "You'll have to start converting an image before this does anything.");
        }
    }
    
    renderWorkQImage();
}

void MainWindow::on_wDumpGFX_File_clicked()
{
    QDataStream vDataOut;
    
    ui16 dVGA_Mode = 2;
    ui16 dDisplayWidth = 80;
    ui16 dDisplayHeight = 25;
    ui16 dFlags = 0;
    ui16 dPaletteColors = 0;
    ui16 dCodePage = 437;

    ui16 dImageWidth = 80;
    ui16 dImageHeight = 25;
    
    ui8 dReserved[0xC] = {0};
    
    vDataOut << "GFX1";
    vDataOut << dVGA_Mode;
    vDataOut << dDisplayWidth;
    vDataOut << dDisplayHeight;
    vDataOut << dFlags;
    vDataOut << dPaletteColors;
    vDataOut << dCodePage;
    vDataOut << dImageWidth;
    vDataOut << dImageHeight;
    vDataOut << *dReserved;
    
    /*int vCharY = 0;
    int vCharX = 0;
        ui8 vCharacter = Core.cellmatrix[vCharY][vCharX].letter;
        ui8 vAttribute = (
            Core.cellmatrix[vCharY][vCharX].fgcol |
            (Core.cellmatrix[vCharY][vCharX].bgcol << 4)
            );  */   
}

void MainWindow::on_wDumpImage_clicked()
{
    /*QFileDialog vFileDialog;
    vFileDialog.setFileMode(QFileDialog::AnyFile);
    vFileDialog.setNameFilter("Images (*.png *.jpg *.bmp)");
    vFileDialog.show();
    QString vFileName = vFileDialog.FileName;*/
    
    QString vFileName = QFileDialog::getSaveFileName(
        this,
        "Save as Image file..",
        QString(),
        mDlgSaveImgFilter,
        0,
        0);
    
    mqWorkImage.save(vFileName, 0, -1);
    
}

void MainWindow::on_actionAlternate_detection_palette_toggled(bool aChecked)
{
    Core.setDetectionPaletteAlternate( aChecked );    
}


//todo: remove
void MainWindow::on_wConvertButton_toggled(bool checked) {}

void MainWindow::reflectCoreBitsMenu() {
    ui->action2x2_Mosaic_comparison->setChecked(
        (Core.mAlgorithmBits & C_COREBIT_2X2) ? true : false);
    ui->actionPer_pixel_comparison->setChecked(
        (Core.mAlgorithmBits == C_COREBIT_PERPIXEL) ? true : false);
    ui->actionPer_character_comparison->setChecked(
        (Core.mAlgorithmBits & C_COREBIT_PERCHAR) ? true : false);
}

//todo: remove
void MainWindow::on_action2x2_Mosaic_comparison_toggled(bool aChecked) {}

//todo: remove
void MainWindow::on_actionPer_pixel_comparison_toggled(bool arg1) { }

void MainWindow::on_action2x2_Mosaic_comparison_triggered(bool checked)
{
    Core.setAlgBits( C_COREBIT_2X2 );
    reflectCoreBitsMenu();
}

void MainWindow::on_actionPer_pixel_comparison_triggered(bool checked)
{
    Core.setAlgBits( C_COREBIT_PERPIXEL );
    reflectCoreBitsMenu();
}

void MainWindow::on_actionPer_character_comparison_triggered(bool checked)
{
    Core.setAlgBits( C_COREBIT_PERCHAR );
    reflectCoreBitsMenu();
}

void MainWindow::on_actionDisable_Alphanum_triggered(bool aChecked)
{
    Core.setCharacterOptions(
        aChecked,
        ui->actionDisable_Sym->isChecked()
    );
}

void MainWindow::on_actionDisable_Sym_triggered(bool aChecked)
{
    Core.setCharacterOptions(
        ui->actionDisable_Alphanum->isChecked(),
        aChecked
    );
}

void MainWindow::reflectFormatBitsMenu() {
    ui->action80_x_25_16_16_color->setChecked(
        (Core.mConfig.mCurrentFormat == tCore::eFormat_16f16b) ? true : false);
    ui->action80_x_25_16_8_color->setChecked(
        (Core.mConfig.mCurrentFormat == tCore::eFormat_16f8b) ? true : false);
    ui->action80_x_25_black_text_white_bg->setChecked(
        (Core.mConfig.mCurrentFormat == tCore::eFormat_Mono_Win) ? true : false);
}

void MainWindow::on_action80_x_25_16_16_color_triggered()
{
    Core.setCurrentFormat( tCore::eFormat_16f16b );
    reflectFormatBitsMenu();
}

void MainWindow::on_action80_x_25_16_8_color_triggered()
{
    Core.setCurrentFormat( tCore::eFormat_16f8b );
    reflectFormatBitsMenu();   
}

void MainWindow::on_action80_x_25_black_text_white_bg_triggered()
{
    Core.setCurrentFormat(tCore::eFormat_Mono_Win);
    reflectFormatBitsMenu();
}

//todo: remove
void MainWindow::on_action80_x_25_triggered() { }

void MainWindow::on_actionExitt_triggered()
{
    mStopFlag = true;
    close();
}

