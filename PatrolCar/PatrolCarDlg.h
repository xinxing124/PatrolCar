// PatrolCarDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "CESeries.h"

// CPatrolCarDlg 对话框
class CPatrolCarDlg : public CDialog
{
// 构造
public:
	CPatrolCarDlg(CWnd* pParent = NULL);	// 标准构造函数
	
// 对话框数据
	enum { IDD = IDD_PATROLCAR_DIALOG };
	CWnd* pWnd; //表示控件窗口的变量 
    CDC* pControlDC; //表示控件窗口的设备描述表.DC 

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	HICON m_myhIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	CCESeries *m_pOneSerial;
	CCESeries *m_pTwoSerial;
	CCESeries* m_pThreeSerial;
	int		m_iThreeSerialMode;
	double m_Xishu[2];
	//定义串口接收数据函数类型
	static void CALLBACK OnOneSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	static void CALLBACK OnTwoSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	static void CALLBACK OnThreeSerialRead(void * pOwner,BYTE* buf,DWORD bufLen);
	int FileRead(CString filename,CString* content);
	void Split(CString source, CStringArray& dest, CString division);
	void ReadParam(void);

	CBitmap m_BitmapMain;
	CBitmap m_BitmapMileage;
	CBitmap m_BitmapKey;
	CBitmap m_BitmapAdd;
	CBitmap m_BitmapReduce;
	CBitmap m_BitmapUp;
	CBitmap m_BitmapDown;
	CBitmap m_BitmapBattery_1;
	CBitmap m_BitmapBattery_2;
	CBitmap m_BitmapBattery_3;
	CBitmap m_BitmapBattery_4;
	CBitmap m_BitmapBattery_5;
	CBitmap m_BitmapBattery_6;
	
	int m_iExitCount;
	CRect m_RectExitShow;
	CRgn  m_rgnExitShow;

	CRect m_RectDateTimeShow;
	CRect m_RectMileageShow;
	CRect m_RectMeterShow;
	CRect m_RectBatteryShow_1;
	CRect m_RectBatteryShow_2;
	CRect m_RectSpeedShow;
	CRect m_RectTotalMileageShow;
	CRect m_RectInfoShow;

	CRect m_RectKey0;
	CRect m_RectKey1;
	CRect m_RectKey2;
	CRect m_RectKey3;
	CRect m_RectKey4;
	CRect m_RectKey5;
	CRect m_RectKey6;
	CRect m_RectKey7;
	CRect m_RectKey8;
	CRect m_RectKey9;
	CRect m_RectKeyAdd;
	CRect m_RectKeySubtract;
	CRect m_RectKeyUp;
	CRect m_RectKeyDwon;
	CRect m_RectKeyOk;
	CRect m_RectKeyBack;
	CRect m_RectKeyClear;
	CRect m_RectKeyDecimal;
	CRect m_RectKeyReturn;

	CRgn  m_rgnKey0;
	CRgn  m_rgnKey1;
	CRgn  m_rgnKey2;
	CRgn  m_rgnKey3;
	CRgn  m_rgnKey4;
	CRgn  m_rgnKey5;
	CRgn  m_rgnKey6;
	CRgn  m_rgnKey7;
	CRgn  m_rgnKey8;
	CRgn  m_rgnKey9;
	CRgn  m_rgnKeyAdd;
	CRgn  m_rgnKeySubtract;
	CRgn  m_rgnKeyUp;
	CRgn  m_rgnKeyDwon;
	CRgn  m_rgnKeyOk;
	CRgn  m_rgnKeyBack;
	CRgn  m_rgnKeyClear;
	CRgn  m_rgnKeyDecimal;
	CRgn  m_rgnKeyReturn;

	CRect m_RectMileageButton;
	CRect m_RectDirectionButton;
	CRect m_RectAddMileageButton;
	CRgn  m_rgnMileageButton;
	CRgn  m_rgnDirectionButton;
	CRgn  m_rgnAddMileageButton;

	CRect m_RectBatteryButton_1;
	CRect m_RectBatteryButton_2;
	bool m_MileageInputFlag;
	int m_iDirectionFlag;
	int m_iAddMileageFlag;
	int m_iTempDirection;
	int m_iTempAddMileage;
	//char m_cCurrMileage[1024];
	
	BYTE m_cRecvBuf[1024];
	CString m_strInfo;
	CString m_strTempMileage;
	CString m_strCurrMileage;
	double m_dbVoltage_1;
	double m_dbVoltage_2;
	double m_dbSpeed;
	double m_dbOldVoltage_1;
	double m_dbOldVoltage_2;
	double m_dbOldSpeed;

	double m_dbTotalMileage;
public:
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int SendData(CCESeries *m_pSerial,CString strData);
};
