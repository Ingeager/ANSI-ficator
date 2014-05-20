#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class MainWindow;
}

//#define C_QIMGFORMAT QImage::Format_RGB888
#define C_QIMGFORMAT QImage::Format_RGB32

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void closeEvent(QCloseEvent *aEvent);
    void resizeEvent(QResizeEvent *aEvent);
    
private:
    Ui::MainWindow *ui;
    
public:
	struct {
        bool mBrowsedEntered;
        bool mLoadedAPic;
        bool mInConversion;
        bool mConvertedSomething;
        int mContrastLevel;
        int mRelativeBrightLevel;
        
        QStringList mGivenMessages;
        
	} mStatus;
	
	QString mDlgSrcFileFilter;
	QString mDlgSaveImgFilter;

	QGraphicsScene mScene;

	QImage mqSrcImage;  //source image, loaded from file, effects may be applied to this
	QImage mqWorkImage; //the QImage which is displayed. On conversion, this is updated gradually
                        //with ANSI graphics. When images are loaded and effects are applied,
                        //this becomes a copy of mqSrcImage. This is also the QImage used when
                        //user wants to save the conversion to an image file.
	int mTargetSizeX;
	int mTargetSizeY;
	
	bool mStopFlag;
	
	void resetStatusRegs();
	void init();
	void initUI();
	void initGfxView();
	void initImage();
	void renderWorkQImage();
	bool setStatusMsg(QString aMessage, int aParameter = 0, bool aRepeat = false);
	bool setStatusMsgNyan(QString aMessage, int aParameter = 0, bool aRepeat = false);
	void restoreConvertButtonText();
	
	void scaleImageRelative();
	void reflectCoreBitsMenu();
	void reflectFormatBitsMenu();

private slots:
    void on_wConvert_clicked();
    void on_wLoadButton_clicked();
    void on_wConvertButton_clicked();
    void on_wWindowScale_toggled(bool checked);
    void on_wSaturPlus_clicked();
    void on_wDumpGFX_File_clicked();
    void on_wDumpImage_clicked();
    void on_wEffDarken_clicked();
    void on_wEffBrighten_clicked();
    void on_wEffMono_clicked();
    void on_wANSIenable_toggled(bool aChecked);
    void on_wBrowseButton_clicked();
    void on_wANSIenable_clicked();
    void on_wConvertButton_toggled(bool checked);
    void on_actionAlternate_detection_palette_toggled(bool aChecked);
    void on_action2x2_Mosaic_comparison_toggled(bool aChecked);
    void on_actionPer_pixel_comparison_toggled(bool arg1);
    void on_action2x2_Mosaic_comparison_triggered(bool checked);
    void on_actionPer_pixel_comparison_triggered(bool checked);
    void on_actionPer_character_comparison_triggered(bool checked);
    void on_actionDisable_Alphanum_triggered(bool aChecked);
    void on_actionDisable_Sym_triggered(bool aChecked);
    void on_action80_x_25_16_16_color_triggered();
    void on_action80_x_25_triggered();
    void on_action80_x_25_16_8_color_triggered();
    void on_actionExitt_triggered();
    void on_action80_x_25_black_text_white_bg_triggered();
    void on_action80_x_25_Grey_text_Black_bg_triggered();
    void on_wSaveTXT_clicked();
    void on_wSaveUTF8_clicked();
};

struct tInterthread{
    MainWindow *mDlg;
    int mStatus;
} ;

#endif // MAINWINDOW_H
