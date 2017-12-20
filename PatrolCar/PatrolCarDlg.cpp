// PatrolCarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HDI_API.h"
#include "PatrolCar.h"
#include "PatrolCarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPatrolCarDlg 对话框
#define PI	3.14159265
const int moveDown=35;
CString m_strReceive;

CPatrolCarDlg::CPatrolCarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatrolCarDlg::IDD, pParent)
	, m_MileageInputFlag(false)
	,m_strCurrMileage("")
	,m_strTempMileage(_T(""))
	,m_iDirectionFlag(0)
	,m_iAddMileageFlag(0)
	, m_dbVoltage_1(0)
	, m_dbVoltage_2(0)
	, m_pOneSerial(NULL)
	, m_pTwoSerial(NULL)
	, m_dbSpeed(0)
	,m_iExitCount(0)
	, m_dbTotalMileage(0)
	, m_strInfo(_T(""))
	, m_iTempDirection(0)
	, m_iTempAddMileage(0)
	, m_pThreeSerial(NULL)
	,m_iThreeSerialMode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strReceive=L"";

}

void CPatrolCarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPatrolCarDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPatrolCarDlg 消息处理程序

BOOL CPatrolCarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	ShowCursor(FALSE);
	MoveWindow(0,0,480,272);//设置窗体大小为480*272

	int iFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int iFullHeight = GetSystemMetrics(SM_CYSCREEN); 
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, iFullWidth, iFullHeight, SWP_NOOWNERZORDER|SWP_SHOWWINDOW);

	//判断串口是否已经打开
	if (m_pOneSerial != NULL)
	{
		m_pOneSerial->ClosePort();

		delete m_pOneSerial;
		m_pOneSerial = NULL;
	}
	
	//新建串口通讯对象
	m_pOneSerial = new CCESeries();
	m_pOneSerial->m_OnSeriesRead = OnOneSerialRead; //

	//打开串口
	if(m_pOneSerial->OpenPort(this,1,9600,0,8,0))
	{
		//AfxMessageBox(L"串口打开成功");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"串口1打开失败");
	}

	m_pTwoSerial = new CCESeries();
	m_pTwoSerial->m_OnSeriesRead = OnTwoSerialRead; //

	//打开串口
	if(m_pTwoSerial->OpenPort(this,2,38400,0,8,0))
	{
		//AfxMessageBox(L"串口打开成功");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"串口2打开失败");
	}

	m_pThreeSerial = new CCESeries();
	m_pThreeSerial->m_OnSeriesRead = OnThreeSerialRead; 

	//打开串口
	if(m_pThreeSerial->OpenPort(this,3,9600,0,8,0))
	{
		//AfxMessageBox(L"串口打开成功");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"串口3打开失败");
	}

	ReadParam();
	
	m_BitmapMain.LoadBitmap(IDB_PIC_MAIN);//加载背景图片
	m_BitmapMileage.LoadBitmap(IDB_PIC_MILEAGE);//加载背景图片
	m_BitmapKey.LoadBitmap(IDB_PIC_KEY);//加载背景图片
	m_BitmapAdd.LoadBitmap(IDB_PIC_MILEAGEADD);//加载背景图片
	m_BitmapReduce.LoadBitmap(IDB_PIC_MILEAGEREDUCTION);//加载背景图片
	m_BitmapUp.LoadBitmap(IDB_PIC_UP);//加载背景图片
	m_BitmapDown.LoadBitmap(IDB_PIC_DOWN);//加载背景图片


	m_BitmapBattery_1.LoadBitmap(IDB_BATTERY_1);//加载背景图片
	m_BitmapBattery_2.LoadBitmap(IDB_BATTERY_2);//加载背景图片
	m_BitmapBattery_3.LoadBitmap(IDB_BATTERY_3);//加载背景图片
	m_BitmapBattery_4.LoadBitmap(IDB_BATTERY_4);//加载背景图片
	m_BitmapBattery_5.LoadBitmap(IDB_BATTERY_5);//加载背景图片
	m_BitmapBattery_6.LoadBitmap(IDB_BATTERY_6);//加载背景图片

	SetTimer(1,100, NULL);
	API_GPIO_Output(0,0);
	API_GPIO_Output(1,0);
	API_GPIO_Output(2,0);
	API_GPIO_Output(3,0);

	//系统退出
	m_RectExitShow.left=5;
	m_RectExitShow.bottom=62;
	m_RectExitShow.top=32;
	m_RectExitShow.right=100;

	//时间日期区域
	m_RectDateTimeShow.left=5;
	m_RectDateTimeShow.bottom=33;
	m_RectDateTimeShow.top=12;
	m_RectDateTimeShow.right=180;

	//表盘区域
	m_RectMeterShow.left=63;
	m_RectMeterShow.bottom=221;
	m_RectMeterShow.top=84;
	m_RectMeterShow.right=321;

	//输入里程区域
	m_RectMileageShow.left=190;
	m_RectMileageShow.bottom=33;
	m_RectMileageShow.top=12;
	m_RectMileageShow.right=289;

	//电池1区域
	m_RectBatteryShow_1.left=390;
	m_RectBatteryShow_1.bottom=117;
	m_RectBatteryShow_1.top=95;
	m_RectBatteryShow_1.right=448;

	//电池2区域
	m_RectBatteryShow_2.left=390;
	m_RectBatteryShow_2.bottom=219;
	m_RectBatteryShow_2.top=197;
	m_RectBatteryShow_2.right=448;

	//速度区域
	m_RectSpeedShow.left=49;
	m_RectSpeedShow.bottom=230;
	m_RectSpeedShow.top=210;
	m_RectSpeedShow.right=161;

	//总里程区域
	m_RectTotalMileageShow.left=206;
	m_RectTotalMileageShow.bottom=230;
	m_RectTotalMileageShow.top=210;
	m_RectTotalMileageShow.right=319;

	//信息区域
	m_RectInfoShow.left=5;
	m_RectInfoShow.bottom=290;////267;
	m_RectInfoShow.top=247;
	m_RectInfoShow.right=478;


	//输入里程按钮
	m_RectMileageButton.left=296;
	m_RectMileageButton.bottom=42;
	m_RectMileageButton.top=8;
	m_RectMileageButton.right=350;

	//上下行按钮
	m_RectDirectionButton.left=356;
	m_RectDirectionButton.bottom=42;
	m_RectDirectionButton.top=8;
	m_RectDirectionButton.right=410;

	//加减里程按钮
	m_RectAddMileageButton.left=415;
	m_RectAddMileageButton.bottom=42;
	m_RectAddMileageButton.top=8;
	m_RectAddMileageButton.right=470;

	//电池1按钮
	m_RectBatteryButton_1.left=411;
	m_RectBatteryButton_1.bottom=84;
	m_RectBatteryButton_1.top=53;
	m_RectBatteryButton_1.right=426;

	//电池2按钮
	m_RectBatteryButton_2.left=411;
	m_RectBatteryButton_2.bottom=189;
	m_RectBatteryButton_2.top=157;
	m_RectInfoShow.right=426;

	//KEY0
	m_RectKey0.left=13;
	m_RectKey0.bottom=moveDown+196;
	m_RectKey0.top=moveDown+162;
	m_RectKey0.right=130;

	//KEY1
	m_RectKey1.left=13;
	m_RectKey1.bottom=moveDown+147;
	m_RectKey1.top=moveDown+113;
	m_RectKey1.right=67;

	//KEY2
	m_RectKey2.left=76;
	m_RectKey2.bottom=moveDown+147;
	m_RectKey2.top=moveDown+113;
	m_RectKey2.right=130;

	//KEY3
	m_RectKey3.left=140;
	m_RectKey3.bottom=moveDown+147;
	m_RectKey3.top=moveDown+113;
	m_RectKey3.right=193;

	//KEY4
	m_RectKey4.left=13;
	m_RectKey4.bottom=moveDown+98;
	m_RectKey4.top=moveDown+64;
	m_RectKey4.right=67;

	//KEY5
	m_RectKey5.left=76;
	m_RectKey5.bottom=moveDown+98;
	m_RectKey5.top=moveDown+64;
	m_RectKey5.right=130;

	//KEY6
	m_RectKey6.left=140;
	m_RectKey6.bottom=moveDown+98;
	m_RectKey6.top=moveDown+64;
	m_RectKey6.right=193;

	//KEY7
	m_RectKey7.left=13;
	m_RectKey7.bottom=moveDown+49;
	m_RectKey7.top=moveDown+15;
	m_RectKey7.right=67;

	//KEY8
	m_RectKey8.left=76;
	m_RectKey8.bottom=moveDown+49;
	m_RectKey8.top=moveDown+15;
	m_RectKey8.right=130;

	//KEY9
	m_RectKey9.left=140;
	m_RectKey9.bottom=moveDown+49;
	m_RectKey9.top=moveDown+15;
	m_RectKey9.right=193;

	//KEYDecimal
	m_RectKeyDecimal.left=140;
	m_RectKeyDecimal.bottom=moveDown+196;
	m_RectKeyDecimal.top=moveDown+162;
	m_RectKeyDecimal.right=193;

	//KEYADD
	m_RectKeyAdd.left=203;
	m_RectKeyAdd.bottom=moveDown+147;
	m_RectKeyAdd.top=moveDown+113;
	m_RectKeyAdd.right=255;

	//KEYSubtract
	m_RectKeySubtract.left=203;
	m_RectKeySubtract.bottom=moveDown+196;
	m_RectKeySubtract.top=moveDown+162;
	m_RectKeySubtract.right=255;
	
	//KEYUp
	m_RectKeyUp.left=203;
	m_RectKeyUp.bottom=moveDown+49;
	m_RectKeyUp.top=moveDown+15;
	m_RectKeyUp.right=255;
	
	//KEYDwon
	m_RectKeyDwon.left=203;
	m_RectKeyDwon.bottom=moveDown+98;
	m_RectKeyDwon.top=moveDown+64;
	m_RectKeyDwon.right=255;
	
	//KEYBack
	m_RectKeyBack.left=279;
	m_RectKeyBack.bottom=moveDown+49;
	m_RectKeyBack.top=moveDown+15;
	m_RectKeyBack.right=332;

	//KEYClear
	m_RectKeyClear.left=279;
	m_RectKeyClear.bottom=moveDown+189;
	m_RectKeyClear.top=moveDown+64;
	m_RectKeyClear.right=332;

	//KEYOk
	m_RectKeyOk.left=279;
	m_RectKeyOk.bottom=moveDown+147;
	m_RectKeyOk.top=moveDown+113;
	m_RectKeyOk.right=332;

	//KEYReturn
	m_RectKeyReturn.left=279;
	m_RectKeyReturn.bottom=moveDown+196;
	m_RectKeyReturn.top=moveDown+162;
	m_RectKeyReturn.right=332;

	m_rgnExitShow.CreateRectRgn(m_RectExitShow.left,m_RectExitShow.top,m_RectExitShow.right,m_RectExitShow.bottom);

	m_rgnMileageButton.CreateRectRgn(m_RectMileageButton.left,m_RectMileageButton.top,m_RectMileageButton.right,m_RectMileageButton.bottom);
	m_rgnDirectionButton.CreateRectRgn(m_RectDirectionButton.left,m_RectDirectionButton.top,m_RectDirectionButton.right,m_RectDirectionButton.bottom);
	m_rgnAddMileageButton.CreateRectRgn(m_RectAddMileageButton.left,m_RectAddMileageButton.top,m_RectAddMileageButton.right,m_RectAddMileageButton.bottom);

	m_rgnKey0.CreateRectRgn(m_RectKey0.left,m_RectKey0.top,m_RectKey0.right,m_RectKey0.bottom);
	m_rgnKey1.CreateRectRgn(m_RectKey1.left,m_RectKey1.top,m_RectKey1.right,m_RectKey1.bottom);
	m_rgnKey2.CreateRectRgn(m_RectKey2.left,m_RectKey2.top,m_RectKey2.right,m_RectKey2.bottom);
	m_rgnKey3.CreateRectRgn(m_RectKey3.left,m_RectKey3.top,m_RectKey3.right,m_RectKey3.bottom);
	m_rgnKey4.CreateRectRgn(m_RectKey4.left,m_RectKey4.top,m_RectKey4.right,m_RectKey4.bottom);
	m_rgnKey5.CreateRectRgn(m_RectKey5.left,m_RectKey5.top,m_RectKey5.right,m_RectKey5.bottom);
	m_rgnKey6.CreateRectRgn(m_RectKey6.left,m_RectKey6.top,m_RectKey6.right,m_RectKey6.bottom);
	m_rgnKey7.CreateRectRgn(m_RectKey7.left,m_RectKey7.top,m_RectKey7.right,m_RectKey7.bottom);
	m_rgnKey8.CreateRectRgn(m_RectKey8.left,m_RectKey8.top,m_RectKey8.right,m_RectKey8.bottom);
	m_rgnKey9.CreateRectRgn(m_RectKey9.left,m_RectKey9.top,m_RectKey9.right,m_RectKey9.bottom);
	m_rgnKeyAdd.CreateRectRgn(m_RectKeyAdd.left,m_RectKeyAdd.top,m_RectKeyAdd.right,m_RectKeyAdd.bottom);
	m_rgnKeySubtract.CreateRectRgn(m_RectKeySubtract.left,m_RectKeySubtract.top,m_RectKeySubtract.right,m_RectKeySubtract.bottom);
	m_rgnKeyUp.CreateRectRgn(m_RectKeyUp.left,m_RectKeyUp.top,m_RectKeyUp.right,m_RectKeyUp.bottom);
	m_rgnKeyDwon.CreateRectRgn(m_RectKeyDwon.left,m_RectKeyDwon.top,m_RectKeyDwon.right,m_RectKeyDwon.bottom);
	m_rgnKeyOk.CreateRectRgn(m_RectKeyOk.left,m_RectKeyOk.top,m_RectKeyOk.right,m_RectKeyOk.bottom);
	m_rgnKeyBack.CreateRectRgn(m_RectKeyBack.left,m_RectKeyBack.top,m_RectKeyBack.right,m_RectKeyBack.bottom);
	m_rgnKeyClear.CreateRectRgn(m_RectKeyClear.left,m_RectKeyClear.top,m_RectKeyClear.right,m_RectKeyClear.bottom);
	m_rgnKeyDecimal.CreateRectRgn(m_RectKeyDecimal.left,m_RectKeyDecimal.top,m_RectKeyDecimal.right,m_RectKeyDecimal.bottom);
	m_rgnKeyReturn.CreateRectRgn(m_RectKeyReturn.left,m_RectKeyReturn.top,m_RectKeyReturn.right,m_RectKeyReturn.bottom);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CPatrolCarDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_PATROLCAR_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_PATROLCAR_DIALOG));
	}
}
#endif

//void CPatrolCarDlg::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//	// TODO: 在此处添加消息处理程序代码
//
////	CDC*  hMemDC = new CDC();
////CBitmap*  hBitmap = new Cbitmap();
////hBitmap = LoadBitmap(IDB_PIC_DOWN)   // ID-BMPSOURCE 为资源ID
////hMemDC - > CreateCompatibleDC(pDC) 
////hMemDC - > SelectObject (hBitmap) 
////pDC - >BitBlt (0 ,0 ,480 ,272 ,hMemDC ,0 ,0 ,SRCCOPY) 
//
//CString str = L"36.00V";
//double SpeedVal=0.0;
//CRect rect;
//CBitmap   bBitmap  ; 
//CBitmap* pOldBitmap;
//CDC   hMemDC; //加载背景图片的内存DC
//CDC   hMemDC2;
//int nWidth=480;
//int nHeight=272;
////CDC memDC;//
////memDC.CreateCompatibleDC(pDC);//创建与目标DC相兼容的内存DC，
////memBitmap.CreateCompatibleBitmap(pDC,480, 272);//根据目标DC创建位图，为什么？看后面
////memDC.SelectObject(&memBitmap);//把位图选入内存DC
////CBrush brush;
////brush.CreateSolidBrush(RGB(255,0,0));//建立个红色的画刷给内存DC
////memDC.SelectObject(&brush)///选择这个刷子
////memDC.Rectangle(0, 0, 100, 100)//一个正方形
//////将这个DC的全部内容放入pDC，这样屏幕上才会有图像
////pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),&memDC, 0, 0,SRCCOPY);
//////结束了 画图不要忘记释放资源，DC是有限的
////memBitmap.DeleteObject();
////memDC.DeleteDC();
//
// //HDC memDC = CreateCompatibleDC(NULL);  
// //   // 创建画布  
//    //HBITMAP hbm = CreateBitmap(100, 100, 1, 32, NULL);  
//
////HBITMAP bmp = (HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(IDB_CLOSE), IMAGE_BITMAP, 0, 0, 0);
////bBitmap.LoadBitmap(IDB_BATTERY_1);//加载背景图片
//hMemDC2.CreateCompatibleDC(&dc);//创建兼容DC
//hMemDC.CreateCompatibleDC(&dc);//创建兼容DC
//pOldBitmap = hMemDC.SelectObject(&m_BitmapMain); //选入背景图片
//dc.BitBlt( 0,0 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);
//
//pOldBitmap = hMemDC.SelectObject(&m_BitmapMileage); //选入输入里程图片
//hMemDC2.BitBlt(m_RectMileageButton.left,m_RectMileageButton.top ,m_RectMileageButton.right,m_RectMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//输入里程
//
//if(m_iTempDirection==0)
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapUp); //选入背景图片
//else
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapDown); //选入背景图片
//hMemDC2.BitBlt(m_RectDirectionButton.left,m_RectDirectionButton.top ,m_RectDirectionButton.right,m_RectDirectionButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//上下行
//
//if(m_iTempAddMileage==0)
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapAdd); //选入背景图片
//else
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapReduce); //选入背景图片
//hMemDC2.BitBlt(m_RectAddMileageButton.left,m_RectAddMileageButton.top ,m_RectAddMileageButton.right,m_RectAddMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//加减里程
//
////m_dbVoltage_2=55.0;
//if(m_dbVoltage_1>=52.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //选入背景图片
//}else if(m_dbVoltage_1>=50.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //选入背景图片
//}else if(m_dbVoltage_1>=48.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //选入背景图片
//}else if(m_dbVoltage_1>=46.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //选入背景图片
//}else if(m_dbVoltage_1>=43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //选入背景图片
//}else if(m_dbVoltage_1<43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //选入背景图片
//}
//hMemDC2.BitBlt(m_RectBatteryButton_1.left,m_RectBatteryButton_1.top ,m_RectBatteryButton_1.right,m_RectBatteryButton_1.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//电池1
//
//if(m_dbVoltage_2>=52.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //选入背景图片
//}else if(m_dbVoltage_2>=50.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //选入背景图片
//}else if(m_dbVoltage_2>=48.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //选入背景图片
//}else if(m_dbVoltage_2>=46.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //选入背景图片
//}else if(m_dbVoltage_2>=43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //选入背景图片
//}else if(m_dbVoltage_2<43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //选入背景图片
//}
//hMemDC2.BitBlt(m_RectBatteryButton_2.left,m_RectBatteryButton_2.top ,m_RectBatteryButton_2.right,m_RectBatteryButton_2.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//电池2
//
//
//
////CRgn::PtInRegion(
//				 //CRgn ss;
//				 //ss.SetRectRgn
//	CFont font;
//	//font.CreateFont(15, 0, 0, 0, FW_BOLD,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
//	font.CreateFont(22, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
//	//font.CreatePointFont(6,L"宋体"); 
//	hMemDC2.SetBkMode(TRANSPARENT);
//	hMemDC2.SetTextColor(RGB(255,255,255));    //Static控件1的字体颜色-红色
//	hMemDC2.SelectObject(&font); 
//
//
////CRgn ss;
//				 //ss.SetRectRgn
//
//    //CString str = L"36.00V";
//	//日期时间区域
//	CTime time = CTime::GetCurrentTime();       
//	str.Format(L"%04d-%02d-%02d %02d:%02d:%02d",time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());  
//    hMemDC2.DrawText(str,m_RectDateTimeShow, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
//	//里程区域
//	//str.Format(L"2017-03-01 16:32:10");
//    hMemDC2.DrawText(m_strTempMileage,m_RectMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	//电池1区域
//	str.Format(L"%.2fV",m_dbVoltage_1);
//    hMemDC2.DrawText(str,m_RectBatteryShow_1, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	//电池2区域
//	str.Format(L"%.2fV",m_dbVoltage_2);
//    hMemDC2.DrawText(str,m_RectBatteryShow_2, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	////速度区域
//	SpeedVal=m_dbSpeed*m_Xishu[1];
//	//str.Format(L"%.2fKm/h",SpeedVal);
// //   dc.DrawText(str,m_RectSpeedShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	////总里程区域
//	//str.Format(L"%.2fKm",m_dbTotalMileage);
// //   dc.DrawText(str,m_RectTotalMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	//CRect rect;
//	//rect.left=m_RectTotalMileageShow.left-3; 
//	//rect.bottom=m_RectTotalMileageShow.bottom+3;
//	//rect.top =m_RectTotalMileageShow.top-3;
//	//rect.right=m_RectTotalMileageShow.right+3;
// //   dc.FillSolidRect(&rect,RGB(255,0,0));//红色填充
//	//信息区域
//	//str.Format(L"2017-03-01 16:32:10");
//    hMemDC2.DrawText(m_strInfo,m_RectInfoShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//
//    font.DeleteObject(); 
//
//	CPen RectPen(PS_SOLID,5,RGB(255,0,0));
//	//dc.SetBkMode(TRANSPARENT);
//	//dc.SetTextColor(RGB(255,0,0));    //Static控件1的字体颜色-红色    
//	hMemDC2.SelectObject(&RectPen); 
//
//	//SpeedVal=10;
//	hMemDC2.MoveTo(184,220);
//    //pControlDC->LineTo(25,151);
//	//pControlDC->LineTo(149+cos((360-(180-(-38)))*PI/180)*126,152+sin((360-(180-(-38)))*PI/180)*126);
//	
//	//pControlDC->LineTo(149+cos((360-(180-(218)))*PI/180)*126,152+sin((360-(180-(218)))*PI/180)*126);
//	//double Val=25.0;
//	//dc.LineTo(149+cos((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126,152+sin((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126);
//	hMemDC2.LineTo((int)(184+cos((360-(180-(20+SpeedVal*2.64/5*180/27)))*PI/180)*126),(int)(220+sin((360-(180-(20+SpeedVal*2.64/5*180/27)))*PI/180)*126));
//	
//	if(m_MileageInputFlag)
//	{
//		pOldBitmap = hMemDC.SelectObject(&m_BitmapKey); //选入背景图片
//		hMemDC2.BitBlt( 0,35 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);
//	}
//
//	//将内存中的图拷贝到屏幕上进行显示  
//	dc.BitBlt(0,0,nWidth,nHeight,&hMemDC2,0,0,SRCCOPY);  
//	 //绘图完成后的清理  
//	 bBitmap.DeleteObject();  
//	 hMemDC.DeleteDC();  
//	 hMemDC2.DeleteDC();  
//
//
//	//dc.LineTo(149,152);
//    //dc.LineTo(100,100);
//    //pControlDC->LineTo(100,0);
//    //pControlDC->LineTo(0,0);
//
//	//RectPen.DeleteObject();
//	//m_BitmapMain.LoadBitmap(IDB_PIC_MAIN);//加载背景图片
//	//m_BitmapKey.LoadBitmap(IDB_PIC_KEY);//加载背景图片
//	//m_BitmapAdd.LoadBitmap(IDB_PIC_MILEAGEADD);//加载背景图片
//	//m_BitmapReduce.LoadBitmap(IDB_PIC_MILEAGEREDUCTION);//加载背景图片
//	//m_BitmapUp.LoadBitmap(IDB_PIC_UP);//加载背景图片
//	//m_BitmapDown.LoadBitmap(IDB_PIC_DOWN);//加载背景图片
//
//
////PAINTSTRUCT ps;
////HDC hdc;
////HDC hdcmem;
////HBITMAP hBm;
////BITMAP bm;
//////hBm=::LoadBitmap(NULL,"D:\\Test.bmp");
////hBm=LoadBitmap(NULL,IDB_PIC_KEY);
//////hBm=::LoadBitmap("D:\\Test.bmp");
//////HINSTANCE hInst=GetModuleHandle(this);
////
//////hBm   =  (HBITMAP)LoadImage(NULL,"d:\\Test.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);  
////
//////hBm=::LoadBitmap(GetModuleHandle(this),MAKEINTRESOURCE(IDB_BITMAP1));
////
//////获取位图尺寸
////GetObject(hBm,sizeof(BITMAP),(LPVOID)&bm);
////
////hdc=GetDC(hwnd);
////hdcmem=CreateCompatibleDC(hdc);
////ReleaseDC(hwnd,hdc);
////
////hdc=BeginPaint(hwnd,&ps);
////SelectObject(hdcmem,hBm);  //将位图选入内存设备环境中
////BitBlt(hdc,180,90,bm.bmWidth,bm.bmHeight,hdcmem,0,0,SRCCOPY);
////EndPaint(hwnd,&ps);
////
////DeleteObject(&hBm);
////DeleteDC(hdcmem);
//
//
//
//	// 不为绘图消息调用 CDialog::OnPaint()
////	pWnd=GetDlgItem(IDC_PAINT_STATIC); 
////    pControlDC=pWnd->GetDC(); 
////
////    pWnd->Invalidate(); 
////    pWnd->UpdateWindow(); 
////
////    CRect rct,rect; 
////    pWnd->GetClientRect(rct); 
////
////	CBrush brush,*oldbrush;
////	/*if(RunFlag)
////		brush.CreateSolidBrush(RGB(0,255,0));
////	else
////		brush.CreateSolidBrush(RGB(255,0,0));*/
////
////	oldbrush=pControlDC->SelectObject(&brush);
////	pControlDC->Ellipse(10,10,40,40);
////	pControlDC->SelectObject(oldbrush);
////
////    CPen RectPen(PS_DASH,5,0x0000FF);
////    pControlDC->SelectObject(&RectPen); 
////
////    //-----------------------------------------------
////    //-- Draw
////    //-----------------------------------------------
////    //pControlDC->Rectangle(rct.left+10,rct.top+10,rct.right-10,rct.bottom-10); 
////
////    //pControlDC->MoveTo(148,151);
////    //pControlDC->LineTo(25,151);
////	//pControlDC->LineTo(149+cos((360-(180-(-38)))*PI/180)*126,152+sin((360-(180-(-38)))*PI/180)*126);
////	
////	//pControlDC->LineTo(149+cos((360-(180-(218)))*PI/180)*126,152+sin((360-(180-(218)))*PI/180)*126);
////	//double Val=25.0;
////	//pControlDC->LineTo(149+cos((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126,152+sin((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126);
////    pControlDC->LineTo(100,100);
////    //pControlDC->LineTo(100,0);
////    //pControlDC->LineTo(0,0);
////	
////	//CFont fontTitle;
////	//	fontTitle.CreatePointFont(200,L"微软雅黑");  
/////*
////	CRect clientRect;
////   GetClientRect(clientRect); // 获得客户区范围
////   
////   CRect rect;
////   rect.left = rect.top = 0;
////   rect.right = 200;
////   rect.bottom = clientRect.bottom;  // 限定宽度
////   CString str = "天哪。。对萨达";
////   CRect temp = rect;
////   int height = pDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // 获得文本高度
////
////   rect.DeflateRect(0,(rect.Height() - height) / 2); // 改变rect
////   pDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
////*/
////
////	CFont font;
////	font.CreatePointFont(200,L"微软雅黑"); 
////	pControlDC->SetBkMode(TRANSPARENT);
////	pControlDC->SetTextColor(RGB(255,0,0));    //Static控件1的字体颜色-红色
////	pControlDC->SelectObject(&font); 
////
////	rect.left=96;
////	rect.bottom=300;
////	rect.top=200;
////	rect.right=200;
////
////	//pControlDC->DrawText(L"36.00V",&rect, DT_CALCRECT | DT_CENTER | DT_SINGLELINE);
////	//fontTitle.DeleteObject();
////
////   //CRect temp = rect;
////   //int height = pControlDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // 获得文本高度
////   //rect.DeflateRect(0,(rect.Height() - height) / 2); // 改变rect
////
////    CString str = L"36.00V";
////    //str.Format(L"%.2fV",BatteryVal);
////    pControlDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
////    font.DeleteObject(); 
////    RectPen.DeleteObject();
////    pWnd->ReleaseDC(pControlDC); 
////
////
////
////	//pControlDC->MoveTo(120,195);
////	//pControlDC->LineTo(120,230);
////
////
////	//pControlDC->MoveTo(120,195);
////	//pControlDC->LineTo(120,230);
//
//}

void CPatrolCarDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码

//	CDC*  hMemDC = new CDC();
//CBitmap*  hBitmap = new Cbitmap();
//hBitmap = LoadBitmap(IDB_PIC_DOWN)   // ID-BMPSOURCE 为资源ID
//hMemDC - > CreateCompatibleDC(pDC) 
//hMemDC - > SelectObject (hBitmap) 
//pDC - >BitBlt (0 ,0 ,480 ,272 ,hMemDC ,0 ,0 ,SRCCOPY) 

CString str = L"36.00V";
double SpeedVal=0.0;
CRect rect;
CBitmap   bBitmap  ; 
CBitmap* pOldBitmap;
CDC   hMemDC; //加载背景图片的内存DC

//CDC memDC;//
//memDC.CreateCompatibleDC(pDC);//创建与目标DC相兼容的内存DC，
//memBitmap.CreateCompatibleBitmap(pDC,480, 272);//根据目标DC创建位图，为什么？看后面
//memDC.SelectObject(&memBitmap);//把位图选入内存DC
//CBrush brush;
//brush.CreateSolidBrush(RGB(255,0,0));//建立个红色的画刷给内存DC
//memDC.SelectObject(&brush)///选择这个刷子
//memDC.Rectangle(0, 0, 100, 100)//一个正方形
////将这个DC的全部内容放入pDC，这样屏幕上才会有图像
//pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),&memDC, 0, 0,SRCCOPY);
////结束了 画图不要忘记释放资源，DC是有限的
//memBitmap.DeleteObject();
//memDC.DeleteDC();

 //HDC memDC = CreateCompatibleDC(NULL);  
 //   // 创建画布  
    //HBITMAP hbm = CreateBitmap(100, 100, 1, 32, NULL);  

//HBITMAP bmp = (HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(IDB_CLOSE), IMAGE_BITMAP, 0, 0, 0);
//bBitmap.LoadBitmap(IDB_BATTERY_1);//加载背景图片
hMemDC.CreateCompatibleDC(&dc);//创建兼容DC
pOldBitmap = hMemDC.SelectObject(&m_BitmapMain); //选入背景图片
dc.BitBlt( 0,0 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);

pOldBitmap = hMemDC.SelectObject(&m_BitmapMileage); //选入输入里程图片
dc.BitBlt(m_RectMileageButton.left,m_RectMileageButton.top ,m_RectMileageButton.right,m_RectMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//输入里程

if(m_iTempDirection==0)
	pOldBitmap = hMemDC.SelectObject(&m_BitmapUp); //选入背景图片
else
	pOldBitmap = hMemDC.SelectObject(&m_BitmapDown); //选入背景图片
dc.BitBlt(m_RectDirectionButton.left,m_RectDirectionButton.top ,m_RectDirectionButton.right,m_RectDirectionButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//上下行

if(m_iTempAddMileage==0)
	pOldBitmap = hMemDC.SelectObject(&m_BitmapAdd); //选入背景图片
else
	pOldBitmap = hMemDC.SelectObject(&m_BitmapReduce); //选入背景图片
dc.BitBlt(m_RectAddMileageButton.left,m_RectAddMileageButton.top ,m_RectAddMileageButton.right,m_RectAddMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//加减里程

//m_dbVoltage_2=55.0;
if(m_dbVoltage_1>=52.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //选入背景图片
}else if(m_dbVoltage_1>=50.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //选入背景图片
}else if(m_dbVoltage_1>=48.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //选入背景图片
}else if(m_dbVoltage_1>=46.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //选入背景图片
}else if(m_dbVoltage_1>=43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //选入背景图片
}else if(m_dbVoltage_1<43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //选入背景图片
}
dc.BitBlt(m_RectBatteryButton_1.left,m_RectBatteryButton_1.top ,m_RectBatteryButton_1.right,m_RectBatteryButton_1.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//电池1

if(m_dbVoltage_2>=52.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //选入背景图片
}else if(m_dbVoltage_2>=50.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //选入背景图片
}else if(m_dbVoltage_2>=48.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //选入背景图片
}else if(m_dbVoltage_2>=46.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //选入背景图片
}else if(m_dbVoltage_2>=43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //选入背景图片
}else if(m_dbVoltage_2<43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //选入背景图片
}
dc.BitBlt(m_RectBatteryButton_2.left,m_RectBatteryButton_2.top ,m_RectBatteryButton_2.right,m_RectBatteryButton_2.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//电池2



//CRgn::PtInRegion(
				 //CRgn ss;
				 //ss.SetRectRgn
	CFont font;
	//font.CreateFont(15, 0, 0, 0, FW_BOLD,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	font.CreateFont(22, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	//font.CreatePointFont(6,L"宋体"); 
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));    //Static控件1的字体颜色-红色
	dc.SelectObject(&font); 


//CRgn ss;
				 //ss.SetRectRgn

    //CString str = L"36.00V";
	//日期时间区域
	CTime time = CTime::GetCurrentTime();       
	str.Format(L"%04d-%02d-%02d %02d:%02d:%02d",time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());  
    dc.DrawText(str,m_RectDateTimeShow, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
	//里程区域
	//str.Format(L"2017-03-01 16:32:10");
    dc.DrawText(m_strTempMileage,m_RectMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	//电池1区域
	str.Format(L"%.2fV",m_dbVoltage_1);
    dc.DrawText(str,m_RectBatteryShow_1, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	//电池2区域
	str.Format(L"%.2fV",m_dbVoltage_2);
    dc.DrawText(str,m_RectBatteryShow_2, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	////速度区域
	SpeedVal=m_dbSpeed*m_Xishu[1];
	//str.Format(L"%.2fKm/h",SpeedVal);
 //   dc.DrawText(str,m_RectSpeedShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	////总里程区域
	//str.Format(L"%.2fKm",m_dbTotalMileage);
 //   dc.DrawText(str,m_RectTotalMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	//CRect rect;
	//rect.left=m_RectTotalMileageShow.left-3; 
	//rect.bottom=m_RectTotalMileageShow.bottom+3;
	//rect.top =m_RectTotalMileageShow.top-3;
	//rect.right=m_RectTotalMileageShow.right+3;
 //   dc.FillSolidRect(&rect,RGB(255,0,0));//红色填充
	font.DeleteObject(); 

	//font.CreateFont(15, 0, 0, 0, FW_BOLD,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	font.CreateFont(20, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	//font.CreatePointFont(6,L"宋体"); 
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));    //Static控件1的字体颜色-红色
	dc.SelectObject(&font); 

	//信息区域
	//str.Format(L"2017-03-01 16:32:10.333 T:9999999-9999999 A:9999999-9999999 F:200.345");
	//str.Format(L"333 T:9999999-9999999 A:9999999-9999999 F:200.345");
	//DT_CALCRECT | DT_WORDBREAK  |  DT_EDITCONTROL|  DT_NOPREFIX | DT_EXTERNALLEADING
	//dc.DrawText(str,m_RectInfoShow,  DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK );
    dc.DrawText(m_strInfo,m_RectInfoShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);

    font.DeleteObject(); 

	CPen RectPen(PS_SOLID,5,RGB(255,0,0));
	//dc.SetBkMode(TRANSPARENT);
	//dc.SetTextColor(RGB(255,0,0));    //Static控件1的字体颜色-红色    
	dc.SelectObject(&RectPen); 

	//SpeedVal=10;
	dc.MoveTo(184,220);
    //pControlDC->LineTo(25,151);
	//pControlDC->LineTo(149+cos((360-(180-(-38)))*PI/180)*126,152+sin((360-(180-(-38)))*PI/180)*126);
	
	//pControlDC->LineTo(149+cos((360-(180-(218)))*PI/180)*126,152+sin((360-(180-(218)))*PI/180)*126);
	//double Val=25.0;
	//dc.LineTo(149+cos((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126,152+sin((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126);
	dc.LineTo((int)(184+cos((360-(180-(20+SpeedVal*2.64/5*180/27)))*PI/180)*126),(int)(220+sin((360-(180-(20+SpeedVal*2.64/5*180/27)))*PI/180)*126));
	
	if(m_MileageInputFlag)
	{
		pOldBitmap = hMemDC.SelectObject(&m_BitmapKey); //选入背景图片
		dc.BitBlt( 0,35 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);
	}

	//dc.LineTo(149,152);
    //dc.LineTo(100,100);
    //pControlDC->LineTo(100,0);
    //pControlDC->LineTo(0,0);

	//RectPen.DeleteObject();
	//m_BitmapMain.LoadBitmap(IDB_PIC_MAIN);//加载背景图片
	//m_BitmapKey.LoadBitmap(IDB_PIC_KEY);//加载背景图片
	//m_BitmapAdd.LoadBitmap(IDB_PIC_MILEAGEADD);//加载背景图片
	//m_BitmapReduce.LoadBitmap(IDB_PIC_MILEAGEREDUCTION);//加载背景图片
	//m_BitmapUp.LoadBitmap(IDB_PIC_UP);//加载背景图片
	//m_BitmapDown.LoadBitmap(IDB_PIC_DOWN);//加载背景图片


//PAINTSTRUCT ps;
//HDC hdc;
//HDC hdcmem;
//HBITMAP hBm;
//BITMAP bm;
////hBm=::LoadBitmap(NULL,"D:\\Test.bmp");
//hBm=LoadBitmap(NULL,IDB_PIC_KEY);
////hBm=::LoadBitmap("D:\\Test.bmp");
////HINSTANCE hInst=GetModuleHandle(this);
//
////hBm   =  (HBITMAP)LoadImage(NULL,"d:\\Test.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);  
//
////hBm=::LoadBitmap(GetModuleHandle(this),MAKEINTRESOURCE(IDB_BITMAP1));
//
////获取位图尺寸
//GetObject(hBm,sizeof(BITMAP),(LPVOID)&bm);
//
//hdc=GetDC(hwnd);
//hdcmem=CreateCompatibleDC(hdc);
//ReleaseDC(hwnd,hdc);
//
//hdc=BeginPaint(hwnd,&ps);
//SelectObject(hdcmem,hBm);  //将位图选入内存设备环境中
//BitBlt(hdc,180,90,bm.bmWidth,bm.bmHeight,hdcmem,0,0,SRCCOPY);
//EndPaint(hwnd,&ps);
//
//DeleteObject(&hBm);
//DeleteDC(hdcmem);



	// 不为绘图消息调用 CDialog::OnPaint()
//	pWnd=GetDlgItem(IDC_PAINT_STATIC); 
//    pControlDC=pWnd->GetDC(); 
//
//    pWnd->Invalidate(); 
//    pWnd->UpdateWindow(); 
//
//    CRect rct,rect; 
//    pWnd->GetClientRect(rct); 
//
//	CBrush brush,*oldbrush;
//	/*if(RunFlag)
//		brush.CreateSolidBrush(RGB(0,255,0));
//	else
//		brush.CreateSolidBrush(RGB(255,0,0));*/
//
//	oldbrush=pControlDC->SelectObject(&brush);
//	pControlDC->Ellipse(10,10,40,40);
//	pControlDC->SelectObject(oldbrush);
//
//    CPen RectPen(PS_DASH,5,0x0000FF);
//    pControlDC->SelectObject(&RectPen); 
//
//    //-----------------------------------------------
//    //-- Draw
//    //-----------------------------------------------
//    //pControlDC->Rectangle(rct.left+10,rct.top+10,rct.right-10,rct.bottom-10); 
//
//    //pControlDC->MoveTo(148,151);
//    //pControlDC->LineTo(25,151);
//	//pControlDC->LineTo(149+cos((360-(180-(-38)))*PI/180)*126,152+sin((360-(180-(-38)))*PI/180)*126);
//	
//	//pControlDC->LineTo(149+cos((360-(180-(218)))*PI/180)*126,152+sin((360-(180-(218)))*PI/180)*126);
//	//double Val=25.0;
//	//pControlDC->LineTo(149+cos((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126,152+sin((360-(180-(-38+SpeedVal*7.12/5*180/27)))*PI/180)*126);
//    pControlDC->LineTo(100,100);
//    //pControlDC->LineTo(100,0);
//    //pControlDC->LineTo(0,0);
//	
//	//CFont fontTitle;
//	//	fontTitle.CreatePointFont(200,L"微软雅黑");  
///*
//	CRect clientRect;
//   GetClientRect(clientRect); // 获得客户区范围
//   
//   CRect rect;
//   rect.left = rect.top = 0;
//   rect.right = 200;
//   rect.bottom = clientRect.bottom;  // 限定宽度
//   CString str = "天哪。。对萨达";
//   CRect temp = rect;
//   int height = pDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // 获得文本高度
//
//   rect.DeflateRect(0,(rect.Height() - height) / 2); // 改变rect
//   pDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
//*/
//
//	CFont font;
//	font.CreatePointFont(200,L"微软雅黑"); 
//	pControlDC->SetBkMode(TRANSPARENT);
//	pControlDC->SetTextColor(RGB(255,0,0));    //Static控件1的字体颜色-红色
//	pControlDC->SelectObject(&font); 
//
//	rect.left=96;
//	rect.bottom=300;
//	rect.top=200;
//	rect.right=200;
//
//	//pControlDC->DrawText(L"36.00V",&rect, DT_CALCRECT | DT_CENTER | DT_SINGLELINE);
//	//fontTitle.DeleteObject();
//
//   //CRect temp = rect;
//   //int height = pControlDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // 获得文本高度
//   //rect.DeflateRect(0,(rect.Height() - height) / 2); // 改变rect
//
//    CString str = L"36.00V";
//    //str.Format(L"%.2fV",BatteryVal);
//    pControlDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
//    font.DeleteObject(); 
//    RectPen.DeleteObject();
//    pWnd->ReleaseDC(pControlDC); 
//
//
//
//	//pControlDC->MoveTo(120,195);
//	//pControlDC->LineTo(120,230);
//
//
//	//pControlDC->MoveTo(120,195);
//	//pControlDC->LineTo(120,230);

}

//定义串口接收数据函数类型
void CALLBACK CPatrolCarDlg::OnOneSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	//CString strRecv=L"";
	//BYTE *pRecvBuf = NULL; //接收缓冲区
	////得到父对象指针
	//CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;

	BYTE OneRecv[1024]={0};
	static BYTE OneRecvBuf[1024]={0};
	static int OneRevPos=0;
	int j=0;
	//得到父对象指针
	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	//pRecvBuf = new BYTE[bufLen];
	CopyMemory(OneRecvBuf+OneRevPos,buf,bufLen);
	OneRevPos=OneRevPos+bufLen;

	for(int i=0;i<OneRevPos;i++)
	{
		if(OneRecvBuf[i]=='#')
		{
			for(int k=i;k<OneRevPos;k++)
			{
				if(OneRecvBuf[k]=='$')
				{
					
					CopyMemory(OneRecv,OneRecvBuf+i+1,k-1);
					//strRecv=CString((char*)OneRecv);
					pThis->m_strInfo=CString((char*)OneRecv);
					pThis->InvalidateRect(&pThis->m_RectInfoShow);
					OneRevPos=0;
					break;
				}
			}
		}
	}

	//if(OneRecvBuf[0]=='#')
	//{
	//	for(int k=i;k<OneRevPos;k++)
	//	{
	//		if(OneRecvBuf[k]=='$')
	//		{
	//			
	//			CopyMemory(OneRecv,OneRecvBuf+1,k-1);
	//			//strRecv=CString((char*)OneRecv);
	//			pThis->m_strInfo=CString((char*)OneRecv);
	//			pThis->InvalidateRect(&pThis->m_RectInfoShow);
	//			OneRevPos=0;
	//			break;
	//		}
	//	}
	//}

	////将接收的缓冲区拷贝到pRecvBuf种
	//strRecv=CString((char*)buf);
	
	//m_strReceive+=strRecv;
	////m_strReceive+=CString((char*)buf);
	//int iSos=m_strReceive.Find('#');
	//if(iSos>0)
	//{
	//	int iPos=m_strReceive.Find('$');
	//	if(iPos>0)
	//	{
	//		pThis->m_strInfo=m_strReceive.Mid(1,iPos-1);
	//		pThis->InvalidateRect(&pThis->m_RectInfoShow);
	//		m_strReceive=L"";
	//	}
	//}
	//else
	//{
	//	m_strReceive=L"";
	//}

}

void CALLBACK CPatrolCarDlg::OnTwoSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	BYTE TwoRecvBuf[34]={0}; //接收缓冲区
	BOOL TwoRevFlag=false;

	CString strRecv = L"";
	CString strSend=L"";

	//得到父对象指针
	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
	
	if(bufLen<3)return;
	for(int i=0,j=0;i<(int)bufLen-1;i++)
	{
		if(j>32)
			break;
		if(!TwoRevFlag)
		{
			if((buf[i]=='Q'||buf[i]=='U'))
			{
				if(buf[i+1]!=13)
				{
					TwoRecvBuf[j++]=buf[i];
					TwoRevFlag=true;
				}
			}
		}
		else
		{
			TwoRecvBuf[j++]=buf[i];
		}
	}
	if(TwoRevFlag)
	{
		strRecv=(char*)TwoRecvBuf; 
		strSend.Format(L"#%s$",strRecv);
		pThis->SendData(pThis->m_pOneSerial,strSend);
	}
//	if((buf[0]=='Q'||buf[0]=='U'))
//	{
//		if(buf[1]==13)
//		{
//	m_TwoRevLen=0;
//		memset(m_TwoRecvBuf,0,1000);
//		}
//		else
//		{
//	memcpy(m_TwoRecvBuf+m_TwoRevLen,buf,bufLen);
//	if(m_TwoRevLen>2048)m_TwoRevLen=0;
//	m_TwoRevLen+=bufLen;
//		}
//	}
//	else
//	{
//m_TwoRevLen=0;
//memset(m_TwoRecvBuf,0,1000);
//	}


	//memcpy(m_TwoRecvBuf+m_TwoRevLen,buf,bufLen);
	//if(m_TwoRevLen>4095)m_TwoRevLen=0;
	//m_TwoRevLen+=bufLen;

	//for(int i=m_TwoRevLen;i>-1;i--)
	//{

	//	if((m_TwoRecvBuf[i]=='Q'||m_TwoRecvBuf[i]=='U'))
	//	{
	//		if(m_TwoRecvBuf[i+1]==13)
	//		{
	//		memcpy(m_TwoRecvBuf,m_TwoRecvBuf+i+4,m_TwoRevLen);
	//		m_TwoRevLen=m_TwoRevLen-i-4;
	//		if(m_TwoRevLen<0)m_TwoRevLen=0;
	//		}
	//		else
	//		{
	//			memcpy(m_TwoRecvBuf,m_TwoRecvBuf+i,m_TwoRevLen);
	//			m_TwoRecvBuf[33]=0;
	//			m_TwoRevLen=m_TwoRevLen-i;
	//			strRecv=(char*)m_TwoRecvBuf; 
	//			strSend.Format(L"#%s$",strRecv);
	//			pThis->SendData(pThis->m_pOneSerial,strSend);
	//			TRACE(strRecv+_T("\n"));
	//		}
	//	}
	//}
	//for(int i=0;i<m_TwoRevLen;i++)
	//{
	//	if(m_TwoRecvBuf[i]>47&&m_TwoRecvBuf[i]<123)
	//	{
	//		if((m_TwoRecvBuf[i]=='Q'||m_TwoRecvBuf[i]=='U'))
	//		{
	//			if(m_TwoRecvBuf[i+1]==13)
	//			{
	//			memcpy(m_TwoRecvBuf,m_TwoRecvBuf+i+4,m_TwoRevLen);
	//			m_TwoRevLen=m_TwoRevLen-i-4;
	//			if(m_TwoRevLen<0)m_TwoRevLen=0;
	//			}
	//			else if(m_TwoRecvBuf[i+1]!=13)
	//			{
	//				memcpy(m_TwoRecvBuf,m_TwoRecvBuf+i,m_TwoRevLen);
	//				if(m_TwoRecvBuf[33]==13)
	//				{
	//					m_TwoRecvBuf[33]=0;
	//					m_TwoRevLen=m_TwoRevLen-i;
	//					strRecv=(char*)m_TwoRecvBuf; 
	//					strSend.Format(L"#%s$",strRecv);
	//					//pThis->SendData(pThis->m_pOneSerial,strSend);
	//					//TRACE(strRecv+_T("\n"));
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}
	/*for(int i=0;i<(int)bufLen;i++)
	{
		if(buf[i]>47&&buf[i]<123)
		{
			if((buf[i]=='Q'||buf[i]=='U')&&(buf[i+1]!=13||buf[i+1]!=10))
			{
				memcpy(m_TwoRecvBuf+m_TwoRevLen,buf+i,bufLen-i);
				m_TwoRevLen+=bufLen-i;
				if (m_TwoRevLen>2048 )
				{
					m_TwoRevLen=0;
				}
			}
			break;
		}
	}*/

	//将接收的缓冲区拷贝到pRecvBuf种
	//strRecv=CString((char*)buf);
	//m_strTwoRecv+=strRecv;
	//strSend.Format(L"%s_%d@",strRecv,bufLen);

	//TRACE(strSend);
	//TRACE(_T("串口打开成功"));
	//if(m_strTwoRecv.GetAt(2)=='Q'||m_strTwoRecv.GetAt(2)=='U')
	//{
	//	if(bufLen>3)
	//	{
	//		if(m_strTwoRecv.GetLength()>1)
	//		{
	//			strSend.Format(L"#%s$",m_strTwoRecv);
	//			pThis->SendData(pThis->m_pOneSerial,strSend);
	//		}
	//	}
	//	else
	//	{
	//		m_strTwoRecv=L"";
	//	}
	//}
	//else
	//{
	//	m_strTwoRecv=L"";
	//}
}

void CALLBACK CPatrolCarDlg::OnThreeSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	//BYTE *pRecvBuf = NULL; //接收缓冲区
	static BYTE ThreeRecvBuf[1024]={0};
	static int ThreeRevPos=0;
	int j=0;
	int iTemp=0;
	int revint16[4];
	//得到父对象指针
	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
	//将接收的缓冲区拷贝到pRecvBuf种
	//pRecvBuf = new BYTE[bufLen];
	CopyMemory(ThreeRecvBuf+ThreeRevPos,buf,bufLen);
	switch(pThis->m_iThreeSerialMode)
	{
	case 0:
		if(ThreeRecvBuf[0]==1)
		{	
			if(ThreeRevPos==0&&bufLen<13)
			{
				ThreeRevPos=ThreeRevPos+bufLen;
			}
			else
			{
				ThreeRevPos=0;
				for(int i=3,j=0;i<11;j++,i+=2)
					revint16[j] =ThreeRecvBuf[i] * 256+ ThreeRecvBuf[i + 1];

				//pThis->m_dbVoltage_1=revint16[0]/1000.0;
				//pThis->m_dbSpeed =revint16[1]/1000.0;
				//pThis->m_dbVoltage_2 = revint16[2] / 1000.0;
				iTemp=(int)(revint16[0]/100.0);
				pThis->m_dbVoltage_1=iTemp/10.0;
				iTemp=(int)(revint16[1]/100.0);
				//pThis->m_dbSpeed =iTemp/10.0;
				iTemp=(int)(revint16[2]/100.0);
				pThis->m_dbVoltage_2 =iTemp/10.0;

				pThis->m_iThreeSerialMode=1;
			}
		}
		break;
	case 1:
		//02 03 00 22 00 01 24 33 读取通道1频率
		//02 03 02 00 00 FC 44返回值
		if(ThreeRecvBuf[0]==2)
		{	
			if(ThreeRevPos==0&&bufLen<7)
			{
				ThreeRevPos=ThreeRevPos+bufLen;
			}
			else
			{
				ThreeRevPos=0;
				if(ThreeRecvBuf[1]==3&&ThreeRecvBuf[2]==2)
				{
					//iTemp=(int)(ThreeRecvBuf[4] * 256+ ThreeRecvBuf[3]);
					iTemp=(int)(ThreeRecvBuf[4]);
					pThis->m_dbSpeed =iTemp;
				}
				pThis->m_iThreeSerialMode=0;
			}
		}
		break;
	case 2:
		//02 03 00 28 00 04 C4 32 读取两个通道计数器值
		//02 03 08 00 00 00 00 00 00 00 00 9A 93
		if(ThreeRecvBuf[0]==2)
		{	
			if(ThreeRevPos==0&&bufLen<13)
			{
				ThreeRevPos=ThreeRevPos+bufLen;
			}
			else
			{
				ThreeRevPos=0;
				if(ThreeRecvBuf[1]==3&&ThreeRecvBuf[2]==8)
				{
					iTemp=(int)(ThreeRecvBuf[6] * 256+ ThreeRecvBuf[4]);
					//pThis->m_dbSpeed =iTemp/10.0;
					iTemp=(int)(ThreeRecvBuf[10] * 256+ ThreeRecvBuf[8]);
					//pThis->m_dbSpeed =iTemp/10.0;
				}
			}
		}
		//pThis->m_iThreeSerialMode=0;
		break;
	 default:
		break;

	}
}

//// 串口接收数据处理函数
//LONG CPatrolCarDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
//{
//	//CString strOldRecv = L"";
//	//CString strRecv = L"";
//	//串口接收到的BUF
//	CHAR *pBuf = (CHAR*)wParam;
//	//Wendu=*pBuf;
//	//串口接收到的BUF长度
//	DWORD dwBufLen = lParam;
//	
//	CHAR cVal[10];
//
//	//接收框
//	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
//	//ASSERT(pEdtRecvMsg != NULL);
//
//	//得到接收框中的历史文本
//	//pEdtRecvMsg->GetWindowTextW(strOldRecv);
//	//>+07.537+07.538-00.000-00.000
//
//	for(int i=0;i<4;i++)
//	{
//		memset( cVal, 0, 10 );
//		memcpy(cVal,pBuf+2+7*i,6);
//		m_RecvData[i]=atof(cVal);
//	}
//	
//	BatteryVal=m_RecvData[0]*m_Xishu[0];//15.0/3.0;
//	SpeedVal=m_RecvData[1]*m_Xishu[1];//25.0/10.0;
//	
//	OnPaint();
//	//B2+(B3-B2)/(A3-A2)*(x-A2)
//
//	//if(m_OnceVal==-99.9)
//	//{
//	//	m_OnceVal=t;
//	//}
//	//else if(m_SecondaryVal==-99.9)
//	//{
//	//	m_SecondaryVal=t;
//	//}
//	
//	//将新接收到的文本添加到接收框中
//	//strOldRecv = strOldRecv + strRecv;
//	
//	//pEdtRecvMsg->SetWindowTextW(strRecv);
//
//	//释放内存
//	delete[] pBuf;
//	pBuf = NULL;
//
//	//m_ShowResult.Format(_T("%s_%f"),strRecv,t);
//	//m_ShowResult.Format(_T("%.3f"),t);
//	//UpdateData(false);
//
//
//	return 0;
//}
// 
//
////定义串口接收数据函数类型
//void CALLBACK CPatrolCarDlg::OnTwoSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
//{
//	BYTE *pRecvBuf = NULL; //接收缓冲区
//	//得到父对象指针
//	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
//	//将接收的缓冲区拷贝到pRecvBuf种
//	pRecvBuf = new BYTE[bufLen];
//	CopyMemory(pRecvBuf,buf,bufLen);
//
//	//发送异步消息，表示收到串口数据，消息处理完，应释放内存
//	pThis->PostMessage(WM_RECV_TSERIAL_DATA,WPARAM(pRecvBuf),bufLen);
//
//}
//
//// 串口接收数据处理函数
//LONG CPatrolCarDlg::OnRecvTSerialData(WPARAM wParam,LPARAM lParam)
//{
//	//CString strOldRecv = L"";
//	CString strRecv = L"";
//	//串口接收到的BUF
//	CHAR *pBuf = (CHAR*)wParam;
//	//Wendu=*pBuf;
//	//串口接收到的BUF长度
//	DWORD dwBufLen = lParam;
//	
//	CHAR cVal[10];
//	strRecv = CString(pBuf);
//	if( strRecv.Compare(L"RUN=TRUE")==0)
//	{
//		strRecv="";
//		RunFlag=true;
//		m_iFailCount=0;
//		OnPaint();
//	}
//	else if(m_iFailCount>2)
//	{
//		strRecv="";
//		RunFlag=false;
//		OnPaint();
//	}
//	//接收框
//	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
//	//ASSERT(pEdtRecvMsg != NULL);
//
//	//得到接收框中的历史文本
//	//pEdtRecvMsg->GetWindowTextW(strOldRecv);
//	//>+07.537+07.538-00.000-00.000
//
//	//for(int i=0;i<4;i++)
//	//{
//	//	memset( cVal, 0, 10 );
//	//	memcpy(cVal,pBuf+2+7*i,6);
//	//	m_RecvData[i]=atof(cVal);
//	//}
//	
//	//BatteryVal=m_RecvData[0]*m_Xishu[0];//15.0/3.0;
//	//SpeedVal=m_RecvData[1]*m_Xishu[1];//25.0/10.0;
//	
//	//OnPaint();
//	//B2+(B3-B2)/(A3-A2)*(x-A2)
//
//	//if(m_OnceVal==-99.9)
//	//{
//	//	m_OnceVal=t;
//	//}
//	//else if(m_SecondaryVal==-99.9)
//	//{
//	//	m_SecondaryVal=t;
//	//}
//	
//	//将新接收到的文本添加到接收框中
//	//strOldRecv = strOldRecv + strRecv;
//	
//	//pEdtRecvMsg->SetWindowTextW(strRecv);
//
//	//释放内存
//	delete[] pBuf;
//	pBuf = NULL;
//
//	//m_ShowResult.Format(_T("%s_%f"),strRecv,t);
//	//m_ShowResult.Format(_T("%.3f"),t);
//	//UpdateData(false);
//
//
//	return 0;
//}

void CPatrolCarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	CString strTemp=L"";
	CString strSend=L"";
	double dbCurrMileage=0.0f;
	if(m_rgnMileageButton.PtInRegion(point))
	{
		m_iTempDirection=m_iDirectionFlag;
		m_iTempAddMileage=m_iAddMileageFlag;
		m_strTempMileage=m_strCurrMileage;
		m_MileageInputFlag=true;
		rect.left=0;
		rect.bottom=moveDown+215;
		rect.top=moveDown;
		rect.right=345;
		InvalidateRect(&rect);
	}else if(m_rgnExitShow.PtInRegion(point))
	{
		m_iExitCount=(m_iExitCount>20?0:m_iExitCount+1);
		if(m_iExitCount==5)
		{
			CDialog::OnOK();
		}
	}else if(m_rgnDirectionButton.PtInRegion(point))
	{
		//m_iDirectionFlag=!m_iDirectionFlag;
		//InvalidateRect(&m_RectDirectionButton);
	}else if(m_rgnAddMileageButton.PtInRegion(point))
	{
		//m_iAddMileageFlag=!m_iAddMileageFlag;
		//InvalidateRect(&m_RectAddMileageButton);
	}else if(m_MileageInputFlag)
	{
		if(m_rgnKeyReturn.PtInRegion(point))
		{
			m_iTempDirection=m_iDirectionFlag;
			m_iTempAddMileage=m_iAddMileageFlag;
			m_strTempMileage=m_strCurrMileage;
			m_MileageInputFlag=false;
			InvalidateRect(&m_RectMileageShow);
			rect.left=0;
			rect.bottom=moveDown+215;
			rect.top=moveDown;
			rect.right=345;
			InvalidateRect(&rect);
			InvalidateRect(&m_RectDirectionButton);
			InvalidateRect(&m_RectAddMileageButton);
		}else if(m_rgnKeyOk.PtInRegion(point))
		{
			m_strCurrMileage=m_strTempMileage;
			m_iDirectionFlag=m_iTempDirection;
			m_iAddMileageFlag=m_iTempAddMileage;
			m_MileageInputFlag=false;
			InvalidateRect(&m_RectMileageShow);
			rect.left=0;
			rect.bottom=moveDown+215;
			rect.top=moveDown;
			rect.right=345;
			InvalidateRect(&rect);
			InvalidateRect(&m_RectDirectionButton);
			InvalidateRect(&m_RectAddMileageButton);
			wchar_t   *stopstring;
			dbCurrMileage=wcstod(m_strCurrMileage,&stopstring);
			//dbCurrMileage= _ttof(m_strCurrMileage.GetBuffer(m_strCurrMileage.GetLength()));
			strTemp.Format(L"%.6f",dbCurrMileage);
			strTemp.MakeReverse();
			strTemp.Delete(6,1);
			strSend.Format(L"#%d%d%s$",m_iAddMileageFlag,m_iDirectionFlag,strTemp);
			SendData(m_pOneSerial,strSend);
		}else if(m_rgnKeyBack.PtInRegion(point))
		{
			m_strTempMileage.Delete(m_strTempMileage.GetLength()-1,1);
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKeyClear.PtInRegion(point))
		{
			m_strTempMileage="";
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKeyDecimal.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('.');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKeyAdd.PtInRegion(point))
		{
			m_iTempAddMileage=0;
			InvalidateRect(&m_RectAddMileageButton);
		}else if(m_rgnKeySubtract.PtInRegion(point))
		{
			m_iTempAddMileage=1;
			InvalidateRect(&m_RectAddMileageButton);
		}else if(m_rgnKeyUp.PtInRegion(point))
		{
			m_iTempDirection=0;
			InvalidateRect(&m_RectDirectionButton);
		}else if(m_rgnKeyDwon.PtInRegion(point))
		{
			m_iTempDirection=1;
			InvalidateRect(&m_RectDirectionButton);
		}else if(m_rgnKey0.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('0');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey1.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('1');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey2.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('2');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey3.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('3');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey4.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('4');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey5.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('5');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey6.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('6');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey7.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('7');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey8.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('8');
			InvalidateRect(&m_RectMileageShow);
		}else if(m_rgnKey9.PtInRegion(point))
		{
			m_strTempMileage.AppendChar('9');
			InvalidateRect(&m_RectMileageShow);
		}
	}
	CDialog::OnLButtonUp(nFlags, point);
}

void CPatrolCarDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_rgnMileageButton.DeleteObject();
	m_rgnDirectionButton.DeleteObject();
	m_rgnAddMileageButton.DeleteObject();
	KillTimer(1);
	//关闭串口
	//
	if (m_pOneSerial != NULL)
	{
		//关闭串口
		m_pOneSerial->ClosePort();

		//释放串口对象
		delete m_pOneSerial;
		m_pOneSerial = NULL;
		//AfxMessageBox(L"串口关闭成功");
	}

	if (m_pTwoSerial != NULL)
	{
		//关闭串口
		m_pTwoSerial->ClosePort();

		//释放串口对象
		delete m_pTwoSerial;
		m_pTwoSerial = NULL;
		//AfxMessageBox(L"串口关闭成功");
	}
	if (m_pThreeSerial != NULL)
	{
		//关闭串口
		m_pThreeSerial->ClosePort();

		//释放串口对象
		delete m_pThreeSerial;
		m_pThreeSerial = NULL;
		//AfxMessageBox(L"串口关闭成功");
	}


}

void CPatrolCarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	static int iRefresh=0;
	static int iRTime=0;
	static int iLevelFlag=0;
	//CString strData=L"#1$";
	byte senddatas[]={0x23,0x31,0x24};
	byte sendbytes[] =   {0x01,0x03,0x02,0x58,0x00,0x04,0xC4,0x62};
    byte sendfrequency[]={0x02,0x03,0x00,0x22,0x00,0x01,0x24,0x33};
	byte sendzero[]={0x02,0x10,0x00,0x24,0x00,0x03,0x06,0x00,0x01,0x00,0x01,0x00,0x01,0x0E,0x3C};
	byte sendcount[]=    {0x02,0x03,0x00,0x28,0x00,0x04,0xC4,0x32};
	if(nIDEvent==1)
	{
		iRTime=(iRTime>9?0:iRTime+1);
		if(iRTime==9)
		{
			m_iExitCount=0;
			InvalidateRect(&m_RectDateTimeShow);
		}

		iRefresh=(iRefresh>2?0:iRefresh+1);
		if(iRefresh==2)
		{
			//m_iExitCount=0;
			//InvalidateRect(&m_RectDateTimeShow);

			if(m_dbOldVoltage_1!=m_dbVoltage_1)
			{
				m_dbOldVoltage_1=m_dbVoltage_1;
				InvalidateRect(&m_RectBatteryShow_1);
				InvalidateRect(&m_RectBatteryButton_1);
			}
			if(m_dbOldVoltage_2!=m_dbVoltage_2)
			{
				m_dbOldVoltage_2=m_dbVoltage_2;
				InvalidateRect(&m_RectBatteryShow_2);
				InvalidateRect(&m_RectBatteryButton_2);
			}
			if(!m_MileageInputFlag)
			{
				if(m_dbOldSpeed!=m_dbSpeed)
				{
					m_dbOldSpeed=m_dbSpeed;
					rect.left=0;
					rect.bottom=moveDown+215;
					rect.top=moveDown;
					rect.right=345;
					InvalidateRect(&rect);
				}
			}
			//Invalidate();
			//m_iThreeSerialMode=(m_iThreeSerialMode>1?0:m_iThreeSerialMode+1);
			switch(m_iThreeSerialMode)
			{
			case 0:
				m_pThreeSerial->WriteSyncPort(sendbytes,8);
				break;
			case 1:
				m_pThreeSerial->WriteSyncPort(sendfrequency,8);
				break;
			case 2:
				m_pThreeSerial->WriteSyncPort(sendcount,8);
				break;
			}
		}

		UINT8 Level=0;
		API_GPIO_Input(0,&Level);
		if(iLevelFlag==0&&Level==0)
		{
			iLevelFlag=1;
		}else if(iLevelFlag==1&&Level==1)
		{
			//SendData(m_pOneSerial,strData);
			m_pOneSerial->WriteSyncPort(senddatas,3);
			iLevelFlag=0;
		}
	}
	CDialog::OnTimer(nIDEvent);
}

int CPatrolCarDlg::SendData(CCESeries *m_pSerial,CString strData)
{
	//发送数据
	char * buf  =NULL;  //定义发送缓冲区
	DWORD dwBufLen = 0;   //定义发送缓冲区长度

	//串口如果没有打开，直接返回
	if (m_pOneSerial == NULL)
		return -1;
	
	//将待发送的字符串转换成单字节，进行发送
	buf = new char[strData.GetLength()*2+1];
	ZeroMemory(buf,strData.GetLength()*2+1);
	//转换成单字节进行发送	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strData.GetBuffer(strData.GetLength())
	   ,strData.GetLength(),buf,strData.GetLength()*2,NULL,NULL);

	//dwBufLen = strlen(buf) + 1;
	dwBufLen = strlen(buf);

	//发送字符串
	m_pSerial->WriteSyncPort((BYTE*)buf,dwBufLen);

	//释放内存
	delete[] buf;
	buf = NULL;

	return 0;
}


int CPatrolCarDlg::FileRead(CString filename,CString* content)
{
	int lRet;
	HANDLE hFile = INVALID_HANDLE_VALUE;	// 文件句柄 
	if(::GetFileAttributes(filename)==0xFFFFFFFF)
	{
	//::AfxMessageBox(_T("文件不存在"));
		return -5;
	}
	// 创建一个文件或打开一个文件
	hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 
					   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox(_T("打开文件失败!"));
		return -1;
	}

	DWORD filelen,actlen;
	char *pcharbuff;
	
	filelen = GetFileSize(hFile, NULL);							// 获取文件大小
	if (filelen == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("获取文件大小失败!"));
		return -2;	
	}

	BOOL ret = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);		// 移动文件指针到文件开头
	if (ret == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("将文件指针移至文件开头失败!"));
		return -3;	
	}

	pcharbuff = new char[filelen];
	ret = ReadFile(hFile, pcharbuff, filelen, &actlen, NULL);	// 从文件中读出数据 
	if (ret == TRUE)
	{
		LPTSTR pStr = content->GetBuffer(filelen);	
		// 将字节转化为 Unicode 字符串
		MultiByteToWideChar(CP_ACP, 0, pcharbuff, filelen, pStr, filelen);
		content->ReleaseBuffer();
		//AfxMessageBox(_T("读文件成功!"));	
		lRet=0;
	}
	else
	{
		//AfxMessageBox(_T("读文件失败!"));	
		lRet=-4;
	}

	if (pcharbuff != NULL) 
		delete[] pcharbuff;

	CloseHandle(hFile);
	return lRet;
}

void CPatrolCarDlg::Split(CString source, CStringArray& dest, CString division)
{
    dest.RemoveAll();
    int pos = 0;
    int pre_pos = 0;
    while( -1 != pos ){
        pre_pos = pos;
        pos = source.Find(division,(pos));
		if(pos<0)
			dest.Add(source.Mid(pre_pos));
		else
		{
			dest.Add(source.Mid(pre_pos,(pos-pre_pos)));
			pos++;
		}
		
    }
}

void CPatrolCarDlg::ReadParam(void)
{
	int ret=0;
	CString strTmp;
	ret=FileRead(_T("\\ResidentFlash2\\GUI\\xishu.txt"),&strTmp);
	if(ret<0)
	{
		for(int i=0;i<2;i++)
			m_Xishu[i]=1.0;
	}
	else
	{
		CStringArray dest;
		wchar_t   *stopstring;
		Split(strTmp,dest,_T(","));
		int Count = dest.GetSize();
		for(int i=0;i<2;i++)
			m_Xishu[i]=wcstod(dest[i],&stopstring);
	}
}
