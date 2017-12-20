// PatrolCarDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HDI_API.h"
#include "PatrolCar.h"
#include "PatrolCarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPatrolCarDlg �Ի���
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


// CPatrolCarDlg ��Ϣ�������

BOOL CPatrolCarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	ShowCursor(FALSE);
	MoveWindow(0,0,480,272);//���ô����СΪ480*272

	int iFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int iFullHeight = GetSystemMetrics(SM_CYSCREEN); 
	::SetWindowPos(this->m_hWnd, HWND_TOPMOST, 0, 0, iFullWidth, iFullHeight, SWP_NOOWNERZORDER|SWP_SHOWWINDOW);

	//�жϴ����Ƿ��Ѿ���
	if (m_pOneSerial != NULL)
	{
		m_pOneSerial->ClosePort();

		delete m_pOneSerial;
		m_pOneSerial = NULL;
	}
	
	//�½�����ͨѶ����
	m_pOneSerial = new CCESeries();
	m_pOneSerial->m_OnSeriesRead = OnOneSerialRead; //

	//�򿪴���
	if(m_pOneSerial->OpenPort(this,1,9600,0,8,0))
	{
		//AfxMessageBox(L"���ڴ򿪳ɹ�");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"����1��ʧ��");
	}

	m_pTwoSerial = new CCESeries();
	m_pTwoSerial->m_OnSeriesRead = OnTwoSerialRead; //

	//�򿪴���
	if(m_pTwoSerial->OpenPort(this,2,38400,0,8,0))
	{
		//AfxMessageBox(L"���ڴ򿪳ɹ�");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"����2��ʧ��");
	}

	m_pThreeSerial = new CCESeries();
	m_pThreeSerial->m_OnSeriesRead = OnThreeSerialRead; 

	//�򿪴���
	if(m_pThreeSerial->OpenPort(this,3,9600,0,8,0))
	{
		//AfxMessageBox(L"���ڴ򿪳ɹ�");
		//Sleep(200);
	}
	else
	{
		AfxMessageBox(L"����3��ʧ��");
	}

	ReadParam();
	
	m_BitmapMain.LoadBitmap(IDB_PIC_MAIN);//���ر���ͼƬ
	m_BitmapMileage.LoadBitmap(IDB_PIC_MILEAGE);//���ر���ͼƬ
	m_BitmapKey.LoadBitmap(IDB_PIC_KEY);//���ر���ͼƬ
	m_BitmapAdd.LoadBitmap(IDB_PIC_MILEAGEADD);//���ر���ͼƬ
	m_BitmapReduce.LoadBitmap(IDB_PIC_MILEAGEREDUCTION);//���ر���ͼƬ
	m_BitmapUp.LoadBitmap(IDB_PIC_UP);//���ر���ͼƬ
	m_BitmapDown.LoadBitmap(IDB_PIC_DOWN);//���ر���ͼƬ


	m_BitmapBattery_1.LoadBitmap(IDB_BATTERY_1);//���ر���ͼƬ
	m_BitmapBattery_2.LoadBitmap(IDB_BATTERY_2);//���ر���ͼƬ
	m_BitmapBattery_3.LoadBitmap(IDB_BATTERY_3);//���ر���ͼƬ
	m_BitmapBattery_4.LoadBitmap(IDB_BATTERY_4);//���ر���ͼƬ
	m_BitmapBattery_5.LoadBitmap(IDB_BATTERY_5);//���ر���ͼƬ
	m_BitmapBattery_6.LoadBitmap(IDB_BATTERY_6);//���ر���ͼƬ

	SetTimer(1,100, NULL);
	API_GPIO_Output(0,0);
	API_GPIO_Output(1,0);
	API_GPIO_Output(2,0);
	API_GPIO_Output(3,0);

	//ϵͳ�˳�
	m_RectExitShow.left=5;
	m_RectExitShow.bottom=62;
	m_RectExitShow.top=32;
	m_RectExitShow.right=100;

	//ʱ����������
	m_RectDateTimeShow.left=5;
	m_RectDateTimeShow.bottom=33;
	m_RectDateTimeShow.top=12;
	m_RectDateTimeShow.right=180;

	//��������
	m_RectMeterShow.left=63;
	m_RectMeterShow.bottom=221;
	m_RectMeterShow.top=84;
	m_RectMeterShow.right=321;

	//�����������
	m_RectMileageShow.left=190;
	m_RectMileageShow.bottom=33;
	m_RectMileageShow.top=12;
	m_RectMileageShow.right=289;

	//���1����
	m_RectBatteryShow_1.left=390;
	m_RectBatteryShow_1.bottom=117;
	m_RectBatteryShow_1.top=95;
	m_RectBatteryShow_1.right=448;

	//���2����
	m_RectBatteryShow_2.left=390;
	m_RectBatteryShow_2.bottom=219;
	m_RectBatteryShow_2.top=197;
	m_RectBatteryShow_2.right=448;

	//�ٶ�����
	m_RectSpeedShow.left=49;
	m_RectSpeedShow.bottom=230;
	m_RectSpeedShow.top=210;
	m_RectSpeedShow.right=161;

	//���������
	m_RectTotalMileageShow.left=206;
	m_RectTotalMileageShow.bottom=230;
	m_RectTotalMileageShow.top=210;
	m_RectTotalMileageShow.right=319;

	//��Ϣ����
	m_RectInfoShow.left=5;
	m_RectInfoShow.bottom=290;////267;
	m_RectInfoShow.top=247;
	m_RectInfoShow.right=478;


	//������̰�ť
	m_RectMileageButton.left=296;
	m_RectMileageButton.bottom=42;
	m_RectMileageButton.top=8;
	m_RectMileageButton.right=350;

	//�����а�ť
	m_RectDirectionButton.left=356;
	m_RectDirectionButton.bottom=42;
	m_RectDirectionButton.top=8;
	m_RectDirectionButton.right=410;

	//�Ӽ���̰�ť
	m_RectAddMileageButton.left=415;
	m_RectAddMileageButton.bottom=42;
	m_RectAddMileageButton.top=8;
	m_RectAddMileageButton.right=470;

	//���1��ť
	m_RectBatteryButton_1.left=411;
	m_RectBatteryButton_1.bottom=84;
	m_RectBatteryButton_1.top=53;
	m_RectBatteryButton_1.right=426;

	//���2��ť
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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
//	// TODO: �ڴ˴������Ϣ����������
//
////	CDC*  hMemDC = new CDC();
////CBitmap*  hBitmap = new Cbitmap();
////hBitmap = LoadBitmap(IDB_PIC_DOWN)   // ID-BMPSOURCE Ϊ��ԴID
////hMemDC - > CreateCompatibleDC(pDC) 
////hMemDC - > SelectObject (hBitmap) 
////pDC - >BitBlt (0 ,0 ,480 ,272 ,hMemDC ,0 ,0 ,SRCCOPY) 
//
//CString str = L"36.00V";
//double SpeedVal=0.0;
//CRect rect;
//CBitmap   bBitmap  ; 
//CBitmap* pOldBitmap;
//CDC   hMemDC; //���ر���ͼƬ���ڴ�DC
//CDC   hMemDC2;
//int nWidth=480;
//int nHeight=272;
////CDC memDC;//
////memDC.CreateCompatibleDC(pDC);//������Ŀ��DC����ݵ��ڴ�DC��
////memBitmap.CreateCompatibleBitmap(pDC,480, 272);//����Ŀ��DC����λͼ��Ϊʲô��������
////memDC.SelectObject(&memBitmap);//��λͼѡ���ڴ�DC
////CBrush brush;
////brush.CreateSolidBrush(RGB(255,0,0));//��������ɫ�Ļ�ˢ���ڴ�DC
////memDC.SelectObject(&brush)///ѡ�����ˢ��
////memDC.Rectangle(0, 0, 100, 100)//һ��������
//////�����DC��ȫ�����ݷ���pDC��������Ļ�ϲŻ���ͼ��
////pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),&memDC, 0, 0,SRCCOPY);
//////������ ��ͼ��Ҫ�����ͷ���Դ��DC�����޵�
////memBitmap.DeleteObject();
////memDC.DeleteDC();
//
// //HDC memDC = CreateCompatibleDC(NULL);  
// //   // ��������  
//    //HBITMAP hbm = CreateBitmap(100, 100, 1, 32, NULL);  
//
////HBITMAP bmp = (HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(IDB_CLOSE), IMAGE_BITMAP, 0, 0, 0);
////bBitmap.LoadBitmap(IDB_BATTERY_1);//���ر���ͼƬ
//hMemDC2.CreateCompatibleDC(&dc);//��������DC
//hMemDC.CreateCompatibleDC(&dc);//��������DC
//pOldBitmap = hMemDC.SelectObject(&m_BitmapMain); //ѡ�뱳��ͼƬ
//dc.BitBlt( 0,0 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);
//
//pOldBitmap = hMemDC.SelectObject(&m_BitmapMileage); //ѡ���������ͼƬ
//hMemDC2.BitBlt(m_RectMileageButton.left,m_RectMileageButton.top ,m_RectMileageButton.right,m_RectMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//�������
//
//if(m_iTempDirection==0)
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapUp); //ѡ�뱳��ͼƬ
//else
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapDown); //ѡ�뱳��ͼƬ
//hMemDC2.BitBlt(m_RectDirectionButton.left,m_RectDirectionButton.top ,m_RectDirectionButton.right,m_RectDirectionButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//������
//
//if(m_iTempAddMileage==0)
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapAdd); //ѡ�뱳��ͼƬ
//else
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapReduce); //ѡ�뱳��ͼƬ
//hMemDC2.BitBlt(m_RectAddMileageButton.left,m_RectAddMileageButton.top ,m_RectAddMileageButton.right,m_RectAddMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//�Ӽ����
//
////m_dbVoltage_2=55.0;
//if(m_dbVoltage_1>=52.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_1>=50.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_1>=48.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_1>=46.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_1>=43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_1<43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //ѡ�뱳��ͼƬ
//}
//hMemDC2.BitBlt(m_RectBatteryButton_1.left,m_RectBatteryButton_1.top ,m_RectBatteryButton_1.right,m_RectBatteryButton_1.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//���1
//
//if(m_dbVoltage_2>=52.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_2>=50.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_2>=48.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_2>=46.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_2>=43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //ѡ�뱳��ͼƬ
//}else if(m_dbVoltage_2<43.0)
//{
//	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //ѡ�뱳��ͼƬ
//}
//hMemDC2.BitBlt(m_RectBatteryButton_2.left,m_RectBatteryButton_2.top ,m_RectBatteryButton_2.right,m_RectBatteryButton_2.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//���2
//
//
//
////CRgn::PtInRegion(
//				 //CRgn ss;
//				 //ss.SetRectRgn
//	CFont font;
//	//font.CreateFont(15, 0, 0, 0, FW_BOLD,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
//	font.CreateFont(22, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
//	//font.CreatePointFont(6,L"����"); 
//	hMemDC2.SetBkMode(TRANSPARENT);
//	hMemDC2.SetTextColor(RGB(255,255,255));    //Static�ؼ�1��������ɫ-��ɫ
//	hMemDC2.SelectObject(&font); 
//
//
////CRgn ss;
//				 //ss.SetRectRgn
//
//    //CString str = L"36.00V";
//	//����ʱ������
//	CTime time = CTime::GetCurrentTime();       
//	str.Format(L"%04d-%02d-%02d %02d:%02d:%02d",time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());  
//    hMemDC2.DrawText(str,m_RectDateTimeShow, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
//	//�������
//	//str.Format(L"2017-03-01 16:32:10");
//    hMemDC2.DrawText(m_strTempMileage,m_RectMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	//���1����
//	str.Format(L"%.2fV",m_dbVoltage_1);
//    hMemDC2.DrawText(str,m_RectBatteryShow_1, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	//���2����
//	str.Format(L"%.2fV",m_dbVoltage_2);
//    hMemDC2.DrawText(str,m_RectBatteryShow_2, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	////�ٶ�����
//	SpeedVal=m_dbSpeed*m_Xishu[1];
//	//str.Format(L"%.2fKm/h",SpeedVal);
// //   dc.DrawText(str,m_RectSpeedShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	////���������
//	//str.Format(L"%.2fKm",m_dbTotalMileage);
// //   dc.DrawText(str,m_RectTotalMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//	//CRect rect;
//	//rect.left=m_RectTotalMileageShow.left-3; 
//	//rect.bottom=m_RectTotalMileageShow.bottom+3;
//	//rect.top =m_RectTotalMileageShow.top-3;
//	//rect.right=m_RectTotalMileageShow.right+3;
// //   dc.FillSolidRect(&rect,RGB(255,0,0));//��ɫ���
//	//��Ϣ����
//	//str.Format(L"2017-03-01 16:32:10");
//    hMemDC2.DrawText(m_strInfo,m_RectInfoShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
//
//    font.DeleteObject(); 
//
//	CPen RectPen(PS_SOLID,5,RGB(255,0,0));
//	//dc.SetBkMode(TRANSPARENT);
//	//dc.SetTextColor(RGB(255,0,0));    //Static�ؼ�1��������ɫ-��ɫ    
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
//		pOldBitmap = hMemDC.SelectObject(&m_BitmapKey); //ѡ�뱳��ͼƬ
//		hMemDC2.BitBlt( 0,35 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);
//	}
//
//	//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ  
//	dc.BitBlt(0,0,nWidth,nHeight,&hMemDC2,0,0,SRCCOPY);  
//	 //��ͼ��ɺ������  
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
//	//m_BitmapMain.LoadBitmap(IDB_PIC_MAIN);//���ر���ͼƬ
//	//m_BitmapKey.LoadBitmap(IDB_PIC_KEY);//���ر���ͼƬ
//	//m_BitmapAdd.LoadBitmap(IDB_PIC_MILEAGEADD);//���ر���ͼƬ
//	//m_BitmapReduce.LoadBitmap(IDB_PIC_MILEAGEREDUCTION);//���ر���ͼƬ
//	//m_BitmapUp.LoadBitmap(IDB_PIC_UP);//���ر���ͼƬ
//	//m_BitmapDown.LoadBitmap(IDB_PIC_DOWN);//���ر���ͼƬ
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
//////��ȡλͼ�ߴ�
////GetObject(hBm,sizeof(BITMAP),(LPVOID)&bm);
////
////hdc=GetDC(hwnd);
////hdcmem=CreateCompatibleDC(hdc);
////ReleaseDC(hwnd,hdc);
////
////hdc=BeginPaint(hwnd,&ps);
////SelectObject(hdcmem,hBm);  //��λͼѡ���ڴ��豸������
////BitBlt(hdc,180,90,bm.bmWidth,bm.bmHeight,hdcmem,0,0,SRCCOPY);
////EndPaint(hwnd,&ps);
////
////DeleteObject(&hBm);
////DeleteDC(hdcmem);
//
//
//
//	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
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
////	//	fontTitle.CreatePointFont(200,L"΢���ź�");  
/////*
////	CRect clientRect;
////   GetClientRect(clientRect); // ��ÿͻ�����Χ
////   
////   CRect rect;
////   rect.left = rect.top = 0;
////   rect.right = 200;
////   rect.bottom = clientRect.bottom;  // �޶����
////   CString str = "���ġ���������";
////   CRect temp = rect;
////   int height = pDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // ����ı��߶�
////
////   rect.DeflateRect(0,(rect.Height() - height) / 2); // �ı�rect
////   pDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
////*/
////
////	CFont font;
////	font.CreatePointFont(200,L"΢���ź�"); 
////	pControlDC->SetBkMode(TRANSPARENT);
////	pControlDC->SetTextColor(RGB(255,0,0));    //Static�ؼ�1��������ɫ-��ɫ
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
////   //int height = pControlDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // ����ı��߶�
////   //rect.DeflateRect(0,(rect.Height() - height) / 2); // �ı�rect
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
	// TODO: �ڴ˴������Ϣ����������

//	CDC*  hMemDC = new CDC();
//CBitmap*  hBitmap = new Cbitmap();
//hBitmap = LoadBitmap(IDB_PIC_DOWN)   // ID-BMPSOURCE Ϊ��ԴID
//hMemDC - > CreateCompatibleDC(pDC) 
//hMemDC - > SelectObject (hBitmap) 
//pDC - >BitBlt (0 ,0 ,480 ,272 ,hMemDC ,0 ,0 ,SRCCOPY) 

CString str = L"36.00V";
double SpeedVal=0.0;
CRect rect;
CBitmap   bBitmap  ; 
CBitmap* pOldBitmap;
CDC   hMemDC; //���ر���ͼƬ���ڴ�DC

//CDC memDC;//
//memDC.CreateCompatibleDC(pDC);//������Ŀ��DC����ݵ��ڴ�DC��
//memBitmap.CreateCompatibleBitmap(pDC,480, 272);//����Ŀ��DC����λͼ��Ϊʲô��������
//memDC.SelectObject(&memBitmap);//��λͼѡ���ڴ�DC
//CBrush brush;
//brush.CreateSolidBrush(RGB(255,0,0));//��������ɫ�Ļ�ˢ���ڴ�DC
//memDC.SelectObject(&brush)///ѡ�����ˢ��
//memDC.Rectangle(0, 0, 100, 100)//һ��������
////�����DC��ȫ�����ݷ���pDC��������Ļ�ϲŻ���ͼ��
//pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),&memDC, 0, 0,SRCCOPY);
////������ ��ͼ��Ҫ�����ͷ���Դ��DC�����޵�
//memBitmap.DeleteObject();
//memDC.DeleteDC();

 //HDC memDC = CreateCompatibleDC(NULL);  
 //   // ��������  
    //HBITMAP hbm = CreateBitmap(100, 100, 1, 32, NULL);  

//HBITMAP bmp = (HBITMAP)::LoadImage(hInst, MAKEINTRESOURCE(IDB_CLOSE), IMAGE_BITMAP, 0, 0, 0);
//bBitmap.LoadBitmap(IDB_BATTERY_1);//���ر���ͼƬ
hMemDC.CreateCompatibleDC(&dc);//��������DC
pOldBitmap = hMemDC.SelectObject(&m_BitmapMain); //ѡ�뱳��ͼƬ
dc.BitBlt( 0,0 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);

pOldBitmap = hMemDC.SelectObject(&m_BitmapMileage); //ѡ���������ͼƬ
dc.BitBlt(m_RectMileageButton.left,m_RectMileageButton.top ,m_RectMileageButton.right,m_RectMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//�������

if(m_iTempDirection==0)
	pOldBitmap = hMemDC.SelectObject(&m_BitmapUp); //ѡ�뱳��ͼƬ
else
	pOldBitmap = hMemDC.SelectObject(&m_BitmapDown); //ѡ�뱳��ͼƬ
dc.BitBlt(m_RectDirectionButton.left,m_RectDirectionButton.top ,m_RectDirectionButton.right,m_RectDirectionButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//������

if(m_iTempAddMileage==0)
	pOldBitmap = hMemDC.SelectObject(&m_BitmapAdd); //ѡ�뱳��ͼƬ
else
	pOldBitmap = hMemDC.SelectObject(&m_BitmapReduce); //ѡ�뱳��ͼƬ
dc.BitBlt(m_RectAddMileageButton.left,m_RectAddMileageButton.top ,m_RectAddMileageButton.right,m_RectAddMileageButton.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//�Ӽ����

//m_dbVoltage_2=55.0;
if(m_dbVoltage_1>=52.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_1>=50.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_1>=48.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_1>=46.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_1>=43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_1<43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //ѡ�뱳��ͼƬ
}
dc.BitBlt(m_RectBatteryButton_1.left,m_RectBatteryButton_1.top ,m_RectBatteryButton_1.right,m_RectBatteryButton_1.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//���1

if(m_dbVoltage_2>=52.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_1); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_2>=50.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_2); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_2>=48.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_3); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_2>=46.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_4); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_2>=43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_5); //ѡ�뱳��ͼƬ
}else if(m_dbVoltage_2<43.0)
{
	pOldBitmap = hMemDC.SelectObject(&m_BitmapBattery_6); //ѡ�뱳��ͼƬ
}
dc.BitBlt(m_RectBatteryButton_2.left,m_RectBatteryButton_2.top ,m_RectBatteryButton_2.right,m_RectBatteryButton_2.bottom,&hMemDC ,0 ,0 ,SRCCOPY);//���2



//CRgn::PtInRegion(
				 //CRgn ss;
				 //ss.SetRectRgn
	CFont font;
	//font.CreateFont(15, 0, 0, 0, FW_BOLD,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	font.CreateFont(22, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	//font.CreatePointFont(6,L"����"); 
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));    //Static�ؼ�1��������ɫ-��ɫ
	dc.SelectObject(&font); 


//CRgn ss;
				 //ss.SetRectRgn

    //CString str = L"36.00V";
	//����ʱ������
	CTime time = CTime::GetCurrentTime();       
	str.Format(L"%04d-%02d-%02d %02d:%02d:%02d",time.GetYear(),time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());  
    dc.DrawText(str,m_RectDateTimeShow, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
	//�������
	//str.Format(L"2017-03-01 16:32:10");
    dc.DrawText(m_strTempMileage,m_RectMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	//���1����
	str.Format(L"%.2fV",m_dbVoltage_1);
    dc.DrawText(str,m_RectBatteryShow_1, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	//���2����
	str.Format(L"%.2fV",m_dbVoltage_2);
    dc.DrawText(str,m_RectBatteryShow_2, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	////�ٶ�����
	SpeedVal=m_dbSpeed*m_Xishu[1];
	//str.Format(L"%.2fKm/h",SpeedVal);
 //   dc.DrawText(str,m_RectSpeedShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	////���������
	//str.Format(L"%.2fKm",m_dbTotalMileage);
 //   dc.DrawText(str,m_RectTotalMileageShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);
	//CRect rect;
	//rect.left=m_RectTotalMileageShow.left-3; 
	//rect.bottom=m_RectTotalMileageShow.bottom+3;
	//rect.top =m_RectTotalMileageShow.top-3;
	//rect.right=m_RectTotalMileageShow.right+3;
 //   dc.FillSolidRect(&rect,RGB(255,0,0));//��ɫ���
	font.DeleteObject(); 

	//font.CreateFont(15, 0, 0, 0, FW_BOLD,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	font.CreateFont(20, 0, 0, 0, FW_NORMAL,FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,   DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, L"Times New Roman");
	//font.CreatePointFont(6,L"����"); 
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255,255,255));    //Static�ؼ�1��������ɫ-��ɫ
	dc.SelectObject(&font); 

	//��Ϣ����
	//str.Format(L"2017-03-01 16:32:10.333 T:9999999-9999999 A:9999999-9999999 F:200.345");
	//str.Format(L"333 T:9999999-9999999 A:9999999-9999999 F:200.345");
	//DT_CALCRECT | DT_WORDBREAK  |  DT_EDITCONTROL|  DT_NOPREFIX | DT_EXTERNALLEADING
	//dc.DrawText(str,m_RectInfoShow,  DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK );
    dc.DrawText(m_strInfo,m_RectInfoShow, DT_LEFT | DT_EDITCONTROL | DT_WORDBREAK);

    font.DeleteObject(); 

	CPen RectPen(PS_SOLID,5,RGB(255,0,0));
	//dc.SetBkMode(TRANSPARENT);
	//dc.SetTextColor(RGB(255,0,0));    //Static�ؼ�1��������ɫ-��ɫ    
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
		pOldBitmap = hMemDC.SelectObject(&m_BitmapKey); //ѡ�뱳��ͼƬ
		dc.BitBlt( 0,35 ,480 ,272 ,&hMemDC ,0 ,0 ,SRCCOPY);
	}

	//dc.LineTo(149,152);
    //dc.LineTo(100,100);
    //pControlDC->LineTo(100,0);
    //pControlDC->LineTo(0,0);

	//RectPen.DeleteObject();
	//m_BitmapMain.LoadBitmap(IDB_PIC_MAIN);//���ر���ͼƬ
	//m_BitmapKey.LoadBitmap(IDB_PIC_KEY);//���ر���ͼƬ
	//m_BitmapAdd.LoadBitmap(IDB_PIC_MILEAGEADD);//���ر���ͼƬ
	//m_BitmapReduce.LoadBitmap(IDB_PIC_MILEAGEREDUCTION);//���ر���ͼƬ
	//m_BitmapUp.LoadBitmap(IDB_PIC_UP);//���ر���ͼƬ
	//m_BitmapDown.LoadBitmap(IDB_PIC_DOWN);//���ر���ͼƬ


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
////��ȡλͼ�ߴ�
//GetObject(hBm,sizeof(BITMAP),(LPVOID)&bm);
//
//hdc=GetDC(hwnd);
//hdcmem=CreateCompatibleDC(hdc);
//ReleaseDC(hwnd,hdc);
//
//hdc=BeginPaint(hwnd,&ps);
//SelectObject(hdcmem,hBm);  //��λͼѡ���ڴ��豸������
//BitBlt(hdc,180,90,bm.bmWidth,bm.bmHeight,hdcmem,0,0,SRCCOPY);
//EndPaint(hwnd,&ps);
//
//DeleteObject(&hBm);
//DeleteDC(hdcmem);



	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
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
//	//	fontTitle.CreatePointFont(200,L"΢���ź�");  
///*
//	CRect clientRect;
//   GetClientRect(clientRect); // ��ÿͻ�����Χ
//   
//   CRect rect;
//   rect.left = rect.top = 0;
//   rect.right = 200;
//   rect.bottom = clientRect.bottom;  // �޶����
//   CString str = "���ġ���������";
//   CRect temp = rect;
//   int height = pDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // ����ı��߶�
//
//   rect.DeflateRect(0,(rect.Height() - height) / 2); // �ı�rect
//   pDC->DrawText(str,rect, DT_CENTER | DT_EDITCONTROL | DT_WORDBREAK);
//*/
//
//	CFont font;
//	font.CreatePointFont(200,L"΢���ź�"); 
//	pControlDC->SetBkMode(TRANSPARENT);
//	pControlDC->SetTextColor(RGB(255,0,0));    //Static�ؼ�1��������ɫ-��ɫ
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
//   //int height = pControlDC->DrawText(str,temp,DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_EDITCONTROL); // ����ı��߶�
//   //rect.DeflateRect(0,(rect.Height() - height) / 2); // �ı�rect
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

//���崮�ڽ������ݺ�������
void CALLBACK CPatrolCarDlg::OnOneSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
{
	//CString strRecv=L"";
	//BYTE *pRecvBuf = NULL; //���ջ�����
	////�õ�������ָ��
	//CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;

	BYTE OneRecv[1024]={0};
	static BYTE OneRecvBuf[1024]={0};
	static int OneRevPos=0;
	int j=0;
	//�õ�������ָ��
	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
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

	////�����յĻ�����������pRecvBuf��
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
	BYTE TwoRecvBuf[34]={0}; //���ջ�����
	BOOL TwoRevFlag=false;

	CString strRecv = L"";
	CString strSend=L"";

	//�õ�������ָ��
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

	//�����յĻ�����������pRecvBuf��
	//strRecv=CString((char*)buf);
	//m_strTwoRecv+=strRecv;
	//strSend.Format(L"%s_%d@",strRecv,bufLen);

	//TRACE(strSend);
	//TRACE(_T("���ڴ򿪳ɹ�"));
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
	//BYTE *pRecvBuf = NULL; //���ջ�����
	static BYTE ThreeRecvBuf[1024]={0};
	static int ThreeRevPos=0;
	int j=0;
	int iTemp=0;
	int revint16[4];
	//�õ�������ָ��
	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
	//�����յĻ�����������pRecvBuf��
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
		//02 03 00 22 00 01 24 33 ��ȡͨ��1Ƶ��
		//02 03 02 00 00 FC 44����ֵ
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
		//02 03 00 28 00 04 C4 32 ��ȡ����ͨ��������ֵ
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

//// ���ڽ������ݴ�����
//LONG CPatrolCarDlg::OnRecvSerialData(WPARAM wParam,LPARAM lParam)
//{
//	//CString strOldRecv = L"";
//	//CString strRecv = L"";
//	//���ڽ��յ���BUF
//	CHAR *pBuf = (CHAR*)wParam;
//	//Wendu=*pBuf;
//	//���ڽ��յ���BUF����
//	DWORD dwBufLen = lParam;
//	
//	CHAR cVal[10];
//
//	//���տ�
//	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
//	//ASSERT(pEdtRecvMsg != NULL);
//
//	//�õ����տ��е���ʷ�ı�
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
//	//���½��յ����ı���ӵ����տ���
//	//strOldRecv = strOldRecv + strRecv;
//	
//	//pEdtRecvMsg->SetWindowTextW(strRecv);
//
//	//�ͷ��ڴ�
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
////���崮�ڽ������ݺ�������
//void CALLBACK CPatrolCarDlg::OnTwoSerialRead(void * pOwner,BYTE* buf,DWORD bufLen)
//{
//	BYTE *pRecvBuf = NULL; //���ջ�����
//	//�õ�������ָ��
//	CPatrolCarDlg* pThis = (CPatrolCarDlg*)pOwner;
//	//�����յĻ�����������pRecvBuf��
//	pRecvBuf = new BYTE[bufLen];
//	CopyMemory(pRecvBuf,buf,bufLen);
//
//	//�����첽��Ϣ����ʾ�յ��������ݣ���Ϣ�����꣬Ӧ�ͷ��ڴ�
//	pThis->PostMessage(WM_RECV_TSERIAL_DATA,WPARAM(pRecvBuf),bufLen);
//
//}
//
//// ���ڽ������ݴ�����
//LONG CPatrolCarDlg::OnRecvTSerialData(WPARAM wParam,LPARAM lParam)
//{
//	//CString strOldRecv = L"";
//	CString strRecv = L"";
//	//���ڽ��յ���BUF
//	CHAR *pBuf = (CHAR*)wParam;
//	//Wendu=*pBuf;
//	//���ڽ��յ���BUF����
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
//	//���տ�
//	//CEdit *pEdtRecvMsg = (CEdit*)GetDlgItem(IDC_STATIC2);
//	//ASSERT(pEdtRecvMsg != NULL);
//
//	//�õ����տ��е���ʷ�ı�
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
//	//���½��յ����ı���ӵ����տ���
//	//strOldRecv = strOldRecv + strRecv;
//	
//	//pEdtRecvMsg->SetWindowTextW(strRecv);
//
//	//�ͷ��ڴ�
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	// TODO: �ڴ˴������Ϣ����������
	m_rgnMileageButton.DeleteObject();
	m_rgnDirectionButton.DeleteObject();
	m_rgnAddMileageButton.DeleteObject();
	KillTimer(1);
	//�رմ���
	//
	if (m_pOneSerial != NULL)
	{
		//�رմ���
		m_pOneSerial->ClosePort();

		//�ͷŴ��ڶ���
		delete m_pOneSerial;
		m_pOneSerial = NULL;
		//AfxMessageBox(L"���ڹرճɹ�");
	}

	if (m_pTwoSerial != NULL)
	{
		//�رմ���
		m_pTwoSerial->ClosePort();

		//�ͷŴ��ڶ���
		delete m_pTwoSerial;
		m_pTwoSerial = NULL;
		//AfxMessageBox(L"���ڹرճɹ�");
	}
	if (m_pThreeSerial != NULL)
	{
		//�رմ���
		m_pThreeSerial->ClosePort();

		//�ͷŴ��ڶ���
		delete m_pThreeSerial;
		m_pThreeSerial = NULL;
		//AfxMessageBox(L"���ڹرճɹ�");
	}


}

void CPatrolCarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	//��������
	char * buf  =NULL;  //���巢�ͻ�����
	DWORD dwBufLen = 0;   //���巢�ͻ���������

	//�������û�д򿪣�ֱ�ӷ���
	if (m_pOneSerial == NULL)
		return -1;
	
	//�������͵��ַ���ת���ɵ��ֽڣ����з���
	buf = new char[strData.GetLength()*2+1];
	ZeroMemory(buf,strData.GetLength()*2+1);
	//ת���ɵ��ֽڽ��з���	
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strData.GetBuffer(strData.GetLength())
	   ,strData.GetLength(),buf,strData.GetLength()*2,NULL,NULL);

	//dwBufLen = strlen(buf) + 1;
	dwBufLen = strlen(buf);

	//�����ַ���
	m_pSerial->WriteSyncPort((BYTE*)buf,dwBufLen);

	//�ͷ��ڴ�
	delete[] buf;
	buf = NULL;

	return 0;
}


int CPatrolCarDlg::FileRead(CString filename,CString* content)
{
	int lRet;
	HANDLE hFile = INVALID_HANDLE_VALUE;	// �ļ���� 
	if(::GetFileAttributes(filename)==0xFFFFFFFF)
	{
	//::AfxMessageBox(_T("�ļ�������"));
		return -5;
	}
	// ����һ���ļ����һ���ļ�
	hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, 
					   NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//AfxMessageBox(_T("���ļ�ʧ��!"));
		return -1;
	}

	DWORD filelen,actlen;
	char *pcharbuff;
	
	filelen = GetFileSize(hFile, NULL);							// ��ȡ�ļ���С
	if (filelen == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("��ȡ�ļ���Сʧ��!"));
		return -2;	
	}

	BOOL ret = SetFilePointer(hFile, 0, NULL, FILE_BEGIN);		// �ƶ��ļ�ָ�뵽�ļ���ͷ
	if (ret == 0xFFFFFFFF)
	{
		//AfxMessageBox(_T("���ļ�ָ�������ļ���ͷʧ��!"));
		return -3;	
	}

	pcharbuff = new char[filelen];
	ret = ReadFile(hFile, pcharbuff, filelen, &actlen, NULL);	// ���ļ��ж������� 
	if (ret == TRUE)
	{
		LPTSTR pStr = content->GetBuffer(filelen);	
		// ���ֽ�ת��Ϊ Unicode �ַ���
		MultiByteToWideChar(CP_ACP, 0, pcharbuff, filelen, pStr, filelen);
		content->ReleaseBuffer();
		//AfxMessageBox(_T("���ļ��ɹ�!"));	
		lRet=0;
	}
	else
	{
		//AfxMessageBox(_T("���ļ�ʧ��!"));	
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
