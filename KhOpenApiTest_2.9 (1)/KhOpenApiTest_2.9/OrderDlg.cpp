//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2014 (주)한국소리마치        /
//******************************************************************/
//! File Name     : OrderDlg.cpp
//! Function      : 주문 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : 2014/06/02
//! Update        : 
//! Comment       : 
//******************************************************************/
// OrderDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "KhOpenApiTest.h"
#include "OrderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COrderDlg 대화 상자
COrderDlg::COrderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(COrderDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_strScrNo = "8888";
	m_strJongCode = "";
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COrderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COrderDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_LIST1, m_listCtl);
}

//*******************************************************************/
// BEGIN_MESSAGE_MAP
//*******************************************************************/
BEGIN_MESSAGE_MAP(COrderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ORDER,	OnBtnOrder)
	ON_BN_CLICKED(IDC_ORDERTYPE_DEFAULT, &COrderDlg::OnBnClickedOrdertypeDefault)
	ON_BN_CLICKED(IDC_ORDERTYPE_MARGIN, &COrderDlg::OnBnClickedOrdertypeMargin)
	ON_CBN_SELCHANGE(IDC_COM_SBTP, &COrderDlg::OnSelchangeComSbtp)
	ON_CBN_SELCHANGE(IDC_COM_LOAN_TP, &COrderDlg::OnSelchangeComLoanTP)
	ON_BN_CLICKED(IDC_BTN_QRY_LOAN_DATE, &COrderDlg::OnBnClickedQryLoanDate)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &COrderDlg::OnBnClickedBtnClear)
END_MESSAGE_MAP()

BOOL COrderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	((CEdit*)GetDlgItem(IDC_EDT_ACCNO))->SetLimitText(10);		// 계좌번호 최대길이 설정
	((CEdit*)GetDlgItem(IDC_EDT_JONGCODE))->SetLimitText(6);	// 종목코드 최대길이 설정

	// 주문입력 콤보 인덱스 초기화
	((CComboBox*)GetDlgItem(IDC_COM_TRTP))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COM_SBTP))->SetCurSel(0);

	// 거래구분 데이터 세팅
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:시간외종가
	int nTrTpCode[] = {0, 3, 5, 6, 7, 10, 13, 16, 20, 23, 26, 61, 62, 81};
	int i, nCnt = sizeof(nTrTpCode) / sizeof(*nTrTpCode);		// 전체크기 / 원소크기 = 원소개수
	for (i = 0; i < nCnt; i++)
	{
		((CComboBox*)GetDlgItem(IDC_COM_TRTP))->SetItemData(i, nTrTpCode[i]);
	}

	/// 신용주문
	GetDlgItem(IDC_EDT_ACCNO)->SetWindowText(_T(""));
	/// 현금주문을 기본값으로...
	((CButton*)GetDlgItem(IDC_ORDERTYPE_DEFAULT))->SetCheck(TRUE);
	OnBnClickedOrdertypeDefault();
	
	/// 신규매수를 기본값으로...
	SetCreditType(0);

	InitFIDName();
	InitDefault();
	return FALSE;
}

void COrderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void COrderDlg::OnClose()
{
	// 화면 닫을 때 부모윈도우에게 통보(화면 관리 위해)
	if (m_pParent)
	{
		int nLen = m_strScrNo.GetLength();
		char *cScrNo = new char[nLen + 1];
		memset(cScrNo, 0x00, nLen + 1);
		memcpy(cScrNo, m_strScrNo, nLen);
		m_pParent->PostMessage(UM_SCRENN_CLOSE, 0U, (LPARAM)cScrNo);
	}

	CDialogEx::OnClose();
}

HCURSOR COrderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//*******************************************************************/
//! Function Name : OnBtnOrder
//! Function      : 주문 버튼
//! Param         : void
//! Return        : void
//! Create        : 2014/06/02
//! Comment       : 
//******************************************************************/
void COrderDlg::OnBtnOrder()
{
	// 계좌번호 입력 여부
	CString strAccNo;
	((CEdit*)GetDlgItem(IDC_EDT_ACCNO))->GetWindowText(strAccNo);
	if (strAccNo.GetLength() != 10)
	{
		AfxMessageBox(_T("계좌번호 10자를 입력 해 주세요~!"));
		((CEdit*)GetDlgItem(IDC_EDT_ACCNO))->SetFocus();
		return;
	}

	// 종목코드 입력 여부
	((CEdit*)GetDlgItem(IDC_EDT_JONGCODE))->GetWindowText(m_strJongCode);
	if (m_strJongCode.GetLength() != 6)
	{
		AfxMessageBox(_T("종목코드 6자를 입력 해 주세요~!"));
		((CEdit*)GetDlgItem(IDC_EDT_JONGCODE))->SetFocus();
		return;
	}

	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)
	long lOrderType = ((CComboBox*)GetDlgItem(IDC_COM_SBTP))->GetCurSel() + 1;

	// 주문수량 입력 여부
	CString strQty;
	((CEdit*)GetDlgItem(IDC_EDT_QUANTITY))->GetWindowText(strQty);
	int lQty = atol(strQty);
	if ( (lOrderType == 1 || lOrderType == 2) && (lQty < 1) )
	{/// 정정/취소주문시 주문수량이 0이면 미체결 전량 정정/취소
		AfxMessageBox(_T("주문수량을 0보다 큰 수로 입력 해 주세요~!"));
		((CEdit*)GetDlgItem(IDC_EDT_QUANTITY))->SetFocus();
		return;
	}

	// 거래구분 취득
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:장후시간외종가
	CString strHogaGb;
	int		nIndex = ((CComboBox*)GetDlgItem(IDC_COM_TRTP))->GetCurSel();
	DWORD	dwData = ((CComboBox*)GetDlgItem(IDC_COM_TRTP))->GetItemData(nIndex);
	strHogaGb.Format(_T("%02d"), dwData);

	// 주문가격 입력 여부
	CString strPrice;
	((CEdit*)GetDlgItem(IDC_EDT_PRICE))->GetWindowText(strPrice);
	long lPrice = atoi(strPrice);
	
	// 원주문번호 입력 여부
	CString strOrgNo;
	((CEdit*)GetDlgItem(IDC_EDT_ORGNO))->GetWindowText(strOrgNo);
	if (lOrderType > 2 && strOrgNo.GetLength() < 1)
	{/// 매수취소 ~ 매수정정
		AfxMessageBox(_T("원주문번호를 입력 해 주세요~!"));
		((CEdit*)GetDlgItem(IDC_EDT_ORGNO))->SetFocus();
		return;
	}

	/// 현금(현물)주문인지 신용주문인지 체크...
	int	nCheckedOrder = GetCheckedRadioButton(IDC_ORDERTYPE_DEFAULT, IDC_ORDERTYPE_MARGIN);

	CString strRQName(_T("주식주문"));
	if(lOrderType == 1)
		strRQName = _T("신규매수");
	else if(lOrderType == 2)
		strRQName = _T("신규매도");
	else if(lOrderType == 3 || lOrderType == 4)
		strRQName = _T("취소주문");
	else if(lOrderType == 5 || lOrderType == 6)
		strRQName = _T("정정주문");

	long lRet = OP_ERR_ORD_OVERFLOW;
		
	if(nCheckedOrder == IDC_ORDERTYPE_DEFAULT)
	{/// 현금(현물)주문이면...
		lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, strAccNo, lOrderType, m_strJongCode, lQty, lPrice, strHogaGb, strOrgNo);
	}
	else if(nCheckedOrder == IDC_ORDERTYPE_MARGIN)
	{/// 신용주문이면...
		/// 신용구분 값 유형(매수, 매도)별 신용구분값은 다음과 같다...
		/// 신용매수 : 자기융자 (03)
		/// 신용매도 : 자기융자 (33), 자기융자합(99)
		CString		strCreditType(GetLoanType());

		/// 대출일 
		/// 신용매도 자기융자(33)일때만 정확한 대출일 필요
		CString		strLoandDate(GetLoanDate());
		lRet = theApp.m_khOpenApi.SendOrderCredit(strRQName, m_strScrNo, strAccNo, lOrderType, m_strJongCode, lQty, lPrice, strHogaGb, strCreditType, strLoandDate, strOrgNo);
	}

	if (lRet == OP_ERR_ORD_OVERFLOW)
	{
		Sleep(300);
	}
	if (!theApp.IsError(lRet))
	{
		return;
	}
}

//*******************************************************************/
//! Function Name : OnReceiveTrDataKhopenapictrl
//! Function      : 조회 응답 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg
//! Return        : void
//! Create        : 2014/06/02
//! Comment       : 
//******************************************************************/
void COrderDlg::OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	CString strRQName(sRQName);
	if (strRQName == _T("주식주문") 
	 || strRQName == _T("신규매수")
	 || strRQName == _T("신규매도")
	 || strRQName == _T("취소주문")
	 || strRQName == _T("정정주문")
	 )		// 주식기본정보 설정
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrcode, sRQName, 0, _T("주문번호"));	strData.Trim();
		if(strData.IsEmpty() == FALSE)
		{
			CString strFmt;strFmt.Format(_T("%d"), _ttoi(strData));
			((CEdit*)GetDlgItem(IDC_EDT_ORGNO))->SetWindowText(strFmt);
		}
	}
	else if(strRQName == _T("대출일조회"))
	{/// 
		CString		strTRCode(sTrcode);
		int		nCnt = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));
		
		if(nCnt < 1)
		{
			m_listCtl.AddString(_T("대출일 조회 반복데이터 없음."));
			return;
		}

		CString		strSymbol, strSymbolName, strLoanDate, strText;
		for(int nIdx = 0; nIdx < nCnt; nIdx++)
		{
			strSymbol		= theApp.m_khOpenApi.GetCommData(strTRCode, strRQName, nIdx, _T("종목번호"));
			strSymbolName	= theApp.m_khOpenApi.GetCommData(strTRCode, strRQName, nIdx, _T("종목명"));
			strLoanDate		= theApp.m_khOpenApi.GetCommData(strTRCode, strRQName, nIdx, _T("대출일"));

			strSymbol.Trim();
			if(!strSymbol.IsEmpty())
			{
				strSymbol = strSymbol.Mid(1, 6);
			}
			strSymbolName.Trim();
			strLoanDate.Trim();
			strText.Format(_T("대출일[%s]    종목번호[%s]    종목명[%s] \n"), strLoanDate, strSymbol, strSymbolName);
			m_listCtl.AddString(strText);
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveMsgKhopenapictrl
//! Function      : 조회, 주문시 서버 메시지
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg
//! Return        : void
//! Create        : 2014/06/02
//! Comment       : 
//******************************************************************/
void COrderDlg::OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	((CStatic*)GetDlgItem(IDC_STC_MSGBAR))->SetWindowText(sMsg);
}

void COrderDlg::OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{
}

void COrderDlg::InitFIDName()
{
	m_mapFIDName.SetAt(_T("9201")	, _T("계좌번호"));
	m_mapFIDName.SetAt(_T("9203")	, _T("주문번호"));
	m_mapFIDName.SetAt(_T("9001")	, _T("종목코드"));
	m_mapFIDName.SetAt(_T("913")	, _T("주문상태"));
	m_mapFIDName.SetAt(_T("302")	, _T("종목명"));
	m_mapFIDName.SetAt(_T("900")	, _T("주문수량"));
	m_mapFIDName.SetAt(_T("901")	, _T("주문가격"));
	m_mapFIDName.SetAt(_T("902")	, _T("미체결수량"));
	m_mapFIDName.SetAt(_T("903")	, _T("체결누계금액"));
	m_mapFIDName.SetAt(_T("904")	, _T("원주문번호"));
	m_mapFIDName.SetAt(_T("905")	, _T("주문구분"));
	m_mapFIDName.SetAt(_T("906")	, _T("매매구분"));
	m_mapFIDName.SetAt(_T("907")	, _T("매도수구분"));
	m_mapFIDName.SetAt(_T("908")	, _T("주문/체결시간"));
	m_mapFIDName.SetAt(_T("909")	, _T("체결번호"));
	m_mapFIDName.SetAt(_T("910")	, _T("체결가"));
	m_mapFIDName.SetAt(_T("911")	, _T("체결량"));
	m_mapFIDName.SetAt(_T("10")		, _T("현재가"));
	m_mapFIDName.SetAt(_T("27")		, _T("(최우선)매도호가"));
	m_mapFIDName.SetAt(_T("28")		, _T("(최우선)매수호가"));
	m_mapFIDName.SetAt(_T("914")	, _T("단위체결가"));
	m_mapFIDName.SetAt(_T("915")	, _T("단위체결량"));
	m_mapFIDName.SetAt(_T("919")	, _T("거부사유"));
	m_mapFIDName.SetAt(_T("920")	, _T("화면번호"));

	m_mapFIDName.SetAt(_T("917")	, _T("신용구분"));
	m_mapFIDName.SetAt(_T("916")	, _T("대출일"));
	m_mapFIDName.SetAt(_T("930")	, _T("보유수량"));
	m_mapFIDName.SetAt(_T("931")	, _T("매입단가"));
	m_mapFIDName.SetAt(_T("932")	, _T("총매입가"));
	m_mapFIDName.SetAt(_T("933")	, _T("주문가능수량"));
	m_mapFIDName.SetAt(_T("945")	, _T("당일순매수수량"));
	m_mapFIDName.SetAt(_T("946")	, _T("매도/매수구분"));
	m_mapFIDName.SetAt(_T("950")	, _T("당일총매도손일"));
	m_mapFIDName.SetAt(_T("951")	, _T("예수금"));
	m_mapFIDName.SetAt(_T("307")	, _T("기준가"));
	m_mapFIDName.SetAt(_T("8019")	, _T("손익율"));
	m_mapFIDName.SetAt(_T("957")	, _T("신용금액"));
	m_mapFIDName.SetAt(_T("958")	, _T("신용이자"));
	m_mapFIDName.SetAt(_T("918")	, _T("만기일"));
	m_mapFIDName.SetAt(_T("990")	, _T("당일실현손익(유가"));
	m_mapFIDName.SetAt(_T("991")	, _T("당일실현손익률(유가"));
	m_mapFIDName.SetAt(_T("992")	, _T("당일실현손익(신용"));
	m_mapFIDName.SetAt(_T("993")	, _T("당일실현손익률(신용"));

	m_mapFIDName.SetAt(_T("397")	, _T("파생상품거래단위"));
	m_mapFIDName.SetAt(_T("305")	, _T("상한가"));
	m_mapFIDName.SetAt(_T("306")	, _T("하한가"));
}

//*******************************************************************/
//! Function Name : OnReceiveChejanData
//! Function      : 체결잔고 실시간 처리
//! Param         : LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList
//! Return        : void
//! Create        : 2014/06/02
//! Comment       : 
//******************************************************************/
#define S_EQLINE	_T("==================================================")
void COrderDlg::OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList)
{
	CString strGubun(sGubun), strFidList(sFidList);
	if(strGubun.IsEmpty() || strFidList.IsEmpty())
		return;
	
	m_listCtl.AddString(S_EQLINE);
	CString	strAccNo, strOrdNo, strOrdPrice, strOrdCnt, strText;

	int			nIndex(0);
	CString		strFID, strOneData, strFIDName;
	while(AfxExtractSubString(strFID, strFidList, nIndex++, _T(';')))
	{
		if(m_mapFIDName.Lookup(strFID, strFIDName) == FALSE)
			strFIDName = strFID;

		if(strFIDName.IsEmpty())	strFIDName = strFID;

		strOneData = theApp.m_khOpenApi.GetChejanData(atoi(strFID)).Trim();
		
		strText.Format(_T("구분[%s] FID[%4s:%s] = [%s]"),strGubun, strFID, strFIDName, strOneData);
		m_listCtl.AddString(strText);
		strFIDName.Empty();
	}
}

///////////////////////////////////////////////////////////
///////////////////// 신용주문관련 처리. /////////////////////
///////////////////////////////////////////////////////////
void COrderDlg::OnBnClickedOrdertypeDefault()
{/// 현금(현물)주문 선택......
	/// 신용주문관련 컨트롤을 비활성 시킨다...
	EnableCreditOrderControl(FALSE);
	((CComboBox*)(GetDlgItem(IDC_COM_CREDIT_TP)))->SetCurSel(0);	
}

void COrderDlg::OnBnClickedOrdertypeMargin()
{/// 신용주문 선택....
	/// 신용주문관련 컨트롤을 활성 시킨다...
	EnableCreditOrderControl(TRUE);
	((CComboBox*)(GetDlgItem(IDC_COM_CREDIT_TP)))->SetCurSel(0);

	OnSelchangeComSbtp();
}

void COrderDlg::EnableCreditOrderControl(BOOL bEnable)
{/// 신용주문 관련 컨트롤 활성/비활성
	GetDlgItem(IDC_STATIC_CREDIT)->EnableWindow(bEnable);
	GetDlgItem(IDC_COM_CREDIT_TP)->EnableWindow(bEnable);		/// 신용구분
	GetDlgItem(IDC_STATIC_LOAN_DATE)->EnableWindow(bEnable);
	GetDlgItem(IDC_LOAN_DATE)->EnableWindow(bEnable);			/// 대출일
	GetDlgItem(IDC_STATIC_LOAN)->EnableWindow(bEnable);			/// 융자구분
	GetDlgItem(IDC_COM_LOAN_TP)->EnableWindow(bEnable);			/// 융자콤보
	GetDlgItem(IDC_BTN_QRY_LOAN_DATE)->EnableWindow(bEnable);	/// 대출일
}

void COrderDlg::OnSelchangeComSbtp()
{/// 매매유형 변경시...

	int	nChecked = GetCheckedRadioButton(IDC_ORDERTYPE_DEFAULT, IDC_ORDERTYPE_MARGIN);
	if(nChecked == IDC_ORDERTYPE_DEFAULT)
	{/// 현금(현물)주문인 경우...
		EnableCreditOrderControl(FALSE);
		return;
	}

	/// 신용주문일때만....	
	EnableCreditOrderControl(TRUE);

	int	nCurSel = ((CComboBox*)(GetDlgItem(IDC_COM_SBTP)))->GetCurSel();
	SetCreditType(nCurSel);
}

void COrderDlg::SetCreditType(int nIndex)
{/// 매매유형에 따라 신용구분값을 설정한다.

	/// 신용매수 : 자기융자매수 (03)
	/// 신용매도 : 자기융자상환 (33), 자기융자합산(99)
	CComboBox	*pCreditGb	= ((CComboBox*)(GetDlgItem(IDC_COM_CREDIT_TP)));
	CComboBox	*pLoanGb	= ((CComboBox*)(GetDlgItem(IDC_COM_LOAN_TP)));
	
	ASSERT(pCreditGb	!= NULL);
	ASSERT(pLoanGb		!= NULL);

	if(pCreditGb	== NULL)	return;
	if(pLoanGb		== NULL)	return;

	pCreditGb->ResetContent();
	pLoanGb->ResetContent();

	BOOL	bEnable = FALSE;
	if(nIndex == 0)
	{/// 신규매수 주문
		pCreditGb->AddString(_T("자기융자매수   (03)"));
		pLoanGb->AddString(_T("융자            (03)"));
		/// 신용매수 주문시 대출일은 비활성...
		/// 신용매수 주문시 대출일 조회는 비활성
		bEnable = FALSE;
	}
	else if(nIndex == 1)
	{/// 신규매도 주문
		pCreditGb->AddString(_T("자기융자상환   (33)"));
		pLoanGb->AddString(_T("융자            (33)"));
		pLoanGb->AddString(_T("융자합           (99)"));
		
		/// 신용매도 주문시 대출일은 활성...
		/// 신용매도 주문시 대출일 조회는 활성
		bEnable = TRUE;
	}
	else
	{
		EnableCreditOrderControl(FALSE);
		return;
	}

	GetDlgItem(IDC_LOAN_DATE)->EnableWindow(bEnable);
	pCreditGb->SetCurSel(0);
	pLoanGb->SetCurSel(0);
}

void COrderDlg::OnSelchangeComLoanTP()
{/// 신용매도 주문 "융자(33)"일때만 대출일을 활성화...
	CComboBox	*pLoanGb = ((CComboBox*)(GetDlgItem(IDC_COM_LOAN_TP)));
	
	BOOL	bEnable = TRUE;
	/// "융자"일때만 ...
	if(pLoanGb->GetCurSel() == 1)
	{/// 융자합(99)이면 비활성시킨다.
		bEnable = FALSE;
	}
	else
	{/// 융자(33)일때는 활성화
		bEnable	= TRUE;
	}

	GetDlgItem(IDC_LOAN_DATE)->EnableWindow(bEnable);
	//GetDlgItem(IDC_BTN_QRY_LOAN_DATE)->EnableWindow(bEnable);
}

CString	COrderDlg::GetLoanType()
{/// 신용구분값을 얻는다....
	CString		strLoanType (_T("03"));
	/// 신용주문 유형(매수, 매도)별 신용구분값은 다음과 같다...
	/// 신용매수 : 자기융자 (03)
	/// 신용매도 : 자기융자 (33), 자기융자합(99)

	/// 매수주문인지, 매도주문인지 얻는다....
	int	nOrderType = ((CComboBox*)(GetDlgItem(IDC_COM_SBTP)))->GetCurSel();
	if(nOrderType == 1)
	{/// 매도주문이면
		/// 융자인지, 융자합인지 찾는다...
		int nIdx = ((CComboBox*)GetDlgItem(IDC_COM_LOAN_TP))->GetCurSel();

		if(		nIdx == 0)	strLoanType = _T("33");	/// 융자
		else if(nIdx == 1)	strLoanType = _T("99");	/// 융자합
	}
	else if(nOrderType == 0)
	{/// 매수주문이면...
		strLoanType = _T("03");
	}

	return	strLoanType;
}

CString	COrderDlg::GetLoanDate()
{/// 대출일을 얻는다...
	/// 대출일 형식은 YYYYMMDD, 즉 20150101 이렇게 입력해야 한다.
	/// 신용매도시 융자(33)일때 종목별 대출일을 입력.
	/// 신용매도시 융자합(99)으로 설정하면 원장에서 처리
	int	nOrderType = ((CComboBox*)(GetDlgItem(IDC_COM_SBTP)))->GetCurSel();
	if(nOrderType == 0)
	{/// 신용매수 신규주문일땐 대출일은 공백처리.
		return _T("");
	}
	else if(nOrderType == 1)
	{/// 신용매도 융자합(99) 일괄매도는 "99991231"로 매출일정의함...
		CString		strCreditType(GetLoanType());
		if(strCreditType == _T("99"))	return	_T("99991231");
	}

	CString		strLoanDate;
	GetDlgItem(IDC_LOAN_DATE)->GetWindowText(strLoanDate);
	strLoanDate.Remove(_T('-'));
	return	strLoanDate;
}

void COrderDlg::OnBnClickedQryLoanDate()
{/// 대출일 조회...
	/// 대출일 조회는 신용매도 상황(융자 33)일때만 처리.
	int nIdx = ((CComboBox*)GetDlgItem(IDC_COM_LOAN_TP))->GetCurSel();
	if(	nIdx != 0)	return;

	/// TR : OPW00005 체결잔고요청
	/// 계좌번호 : 신용계좌번호
	/// 비밀번호 : 공백
	/// 비밀번호입력매체구분 : 00

	CString strAccNo;
	((CEdit*)GetDlgItem(IDC_EDT_ACCNO))->GetWindowText(strAccNo);
	if (strAccNo.GetLength() != 10)
	{
		AfxMessageBox(_T("계좌번호 10자를 입력 해 주세요~!"));
		((CEdit*)GetDlgItem(IDC_EDT_ACCNO))->SetFocus();
		return;
	}
	theApp.m_khOpenApi.SetInputValue(_T("계좌번호")			, strAccNo);
	theApp.m_khOpenApi.SetInputValue(_T("비밀번호")			, _T(""));
	theApp.m_khOpenApi.SetInputValue(_T("비밀번호입력매체구분")	, _T("00"));

	LONG lRet = theApp.m_khOpenApi.CommRqData(_T("대출일조회"), _T("OPW00005"), 0, m_strScrNo);

	if (!theApp.IsError(lRet))
	{
		return;
	}
}

void COrderDlg::OnBnClickedBtnClear()
{/// 주문결과 삭제
	m_listCtl.ResetContent();
}

void COrderDlg::InitDefault()
{
	CString strAccList = theApp.m_khOpenApi.GetLoginInfo(_T("ACCTLIST_DETAIL"));
	if (strAccList.IsEmpty())
		return;

	int nIdx = 0;
	CString strInfo, strAcc, strName, strGubun, strRe;
	CString strTemp(strAccList);

	while (TRUE)
	{
		nIdx = strTemp.Find(_T(';'));
		if (nIdx < 0)
			break;

		strInfo = strTemp.Left(nIdx);
		strTemp = strTemp.Mid(nIdx+1);

		nIdx = strInfo.Find(_T(','));
		if (nIdx < 0)
			continue;
		strAcc = strInfo.Left(nIdx);
		strInfo = strInfo.Mid(nIdx+1);
		nIdx = strInfo.Find(_T(','));
		if (nIdx < 0)
			continue;
		strName = strInfo.Left(nIdx);
		strInfo = strInfo.Mid(nIdx+1);
		strGubun = strInfo;
		strGubun.Trim();

		if (strAcc.Right(2) == _T("11"))
		{
			((CStatic*)GetDlgItem(IDC_EDT_ACCNO))->SetWindowText(strAcc);
			break;
		}
	}

	((CEdit*)GetDlgItem(IDC_EDT_QUANTITY))->SetWindowText(_T("10"));
	((CEdit*)GetDlgItem(IDC_EDT_JONGCODE))->SetFocus();
}