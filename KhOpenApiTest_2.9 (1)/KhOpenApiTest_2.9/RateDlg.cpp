//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2015 (주)한국소리마치                /
//******************************************************************/
//! File Name			: RateDlg.cpp
//! Function			: 수익률계산
//! System Name	: 키움 오픈API 테스트
//! Create				: , 2015/05/07
//! Update				: 
//! Comment			: 
//******************************************************************/
// RateDlg.cpp : 구현 파일

#include "stdafx.h"
#include "KhOpenApiTest.h"
#include "RateDlg.h"
#include "afxdialogex.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define  COMMISSION		0.0035	// 모의투자 매체수수료
#define  COMMISSION1	0.0003	// 제세금(거래소)
#define  COMMISSION2	0.0018	// 제세금(코스닥)
#define  COMMISSION3	0.0015	// 제세금(거래소 농어촌특별세)
#define  COMMISSION4	0.0018	// 제세금(K-OTC)

// {조회 키,		리얼 키,	행, 열, 타입,			색 변경, 정렬, 앞 문자, 뒷 문자}
const stGRID lstFID[] = 
{
	{_T("신용구분"),	_T("917"),	-1,	0,	DT_NONE,		FALSE,	DT_CENTER,	_T(""),	_T("")},
	{_T("종목코드"),	_T("9001"),	-1,	1,	DT_NONE,		FALSE,	DT_CENTER,	_T(""),	_T("")},
	{_T("종목명"),	_T("302"),	-1,	2,	DT_NONE,		FALSE,	DT_LEFT,	_T(""),	_T("")},
	{_T("현재가"),	_T("10"),	-1,	3,	DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	_T(""),	_T("")},
	{_T("보유수량"),	_T("930"),	-1,	4,	DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	_T(""),	_T("")},
	{_T("평가금액"),	_T("-1"),	-1,	5,	DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	_T(""),	_T("")},
	{_T("평가손익"),	_T("-1"),	-1,	6,	DT_NONE		,	FALSE,	DT_RIGHT,	_T(""),	_T("")},
	{_T("수익률"),	_T("-1"),	-1,	7,	DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	_T(""),	_T("")},
	{_T("매입금액"),	_T("932"),	-1,	8,	DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	_T(""),	_T("")},
	{_T("현재가"),	_T("10"),	-1,	9,	DT_NONE,		FALSE,	DT_RIGHT,	_T(""),	_T("")},	//(계산용으로 숨김)
	{_T("보유수량"),	_T("930"),	-1,	10,	DT_NONE,		FALSE,	DT_RIGHT,	_T(""),	_T("")},	//(계산용으로 숨김)
	{_T("매입금액"),	_T("932"),	-1,	11,	DT_NONE,		FALSE,	DT_RIGHT,	_T(""),	_T("")},	//(계산용으로 숨김)
	{_T("평가금액"),	_T("-1"),	-1,	12,	DT_NONE,		FALSE,	DT_RIGHT,	_T(""),	_T("")},	//(계산용으로 숨김)
};

// CRateDlg 대화 상자
//*******************************************************************************************************/
//! Function Name	: CRateDlg::CRateDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CRateDlg::IDD, pParent)
//! Function			: 생성 초기 처리
//! Param				: HANDLE hBitmap, int bits
//! Return				: void
//! Create				: , 2015/05/07
//! Comment			: 
//*******************************************************************************************************/
CRateDlg::CRateDlg(CWnd* pParent /*=NULL*/, CString strSeverType)
	: CDialogEx(CRateDlg::IDD, pParent)
{
	m_pParent			= pParent;
	m_strScrNo			= _T("9999");

	m_TotalEval			= 0;
	m_TotalPurchase		= 0;
	m_TotalProfit		= 0;
	m_TotalRate			= 0;
	m_Eval				= 0;
	m_Profit			= 0;
	m_Rate				= 0;
	m_Purchase			= 0;
	m_Comm				= 0;
	m_BuyComm			= 0;
	m_SellComm			= 0;
	m_Tax				= 0;
	m_Price				= 0;
	m_Qty				= 0;
	m_nCount			= 0;
	m_nNextRow			= 0;

	m_bNextFlag			= FALSE;
	if(strSeverType.CompareNoCase(_T("1")) == 0)// 모의투자 서버인 경우
		m_dFee = 0.0035;
	else
		m_dFee = 0.00015;	// 실서버 거래수수료율
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//*******************************************************************/
// Function Name	: DoDataExchange
// Function			: DDX/DDV에 관련 된 함수 호출
// Param				: CDataExchange*	pDX
// Return				: void
// Create				: , 2015/05/07
// Comment			: DDX - 컨트롤과 멤버변수사이에 정보를 교환하는 역할
//							: DDV - 멤버변수에 저장 될 값의 유효성 체크
//							: UapdateData()함수로 간접 호출 할 수 있음
//*******************************************************************/
void CRateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRateDlg)
		DDX_Control(pDX, IDC_EDT_ACC,			m_EdtAcc);
		DDX_Control(pDX, IDC_GRD_RATE,			m_grdRate);
		DDX_Control(pDX, IDC_EDT_EVAL,			m_EdtTotalEval);
		DDX_Control(pDX, IDC_EDT_PURCHASE,	m_EdtTotalPurchase);
		DDX_Control(pDX, IDC_EDT_PROFIT,		m_EdtTotalProfit);
		DDX_Control(pDX, IDC_EDT_RATE,			m_EdtTotalRate);
		
	//}}AFX_DATA_MAP
}

//*******************************************************************/
// BEGIN_MESSAGE_MAP
//*******************************************************************/
BEGIN_MESSAGE_MAP(CRateDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBtnSearch)
END_MESSAGE_MAP()

//*******************************************************************/
//! Function Name	: OnInitDialog
//! Function			: 생성 초기 처리
//! Param				: void
//! Return				: BOOL
//! Create				: 2015/05/07
//! Comment			: CRateDlg 메시지 처리기
//******************************************************************/
BOOL CRateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	InitRealAddGrid();					// 그리드 초기 처리

	CString strMsg;
	strMsg.Format(_T("수수료: %.2f%%, 거래소 종목 거래세: %.2f%%, 농특세: %.2f%%, 코스닥 종목 거래세: %.2f%%")
		, m_dFee * 100
		, COMMISSION1 * 100
		, COMMISSION3 * 100
		, COMMISSION2 * 100
		);
	SetDlgItemText(IDC_ST_RATE_INFO, strMsg);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRateDlg::OnPaint()
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

void CRateDlg::OnClose()
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

HCURSOR CRateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRateDlg::InitRealAddGrid()
{// 그리드 초기화, OpenAPI기능과 관계없음

	m_grdRate.SetEditable(false);				//cell을 에디트 못하게 함.
	m_grdRate.EnableScrollBars(SB_HORZ, FALSE);

	COLORREF clr = RGB(215, 227, 241);

	// 고정 행/열 설정
	m_grdRate.SetFixedRowCount(1);

	// 행/열 갯수 설정
	m_grdRate.SetRowCount(1);
	int nCount = sizeof(lstFID) / sizeof(*lstFID);		// 전체크기 / 원소크기 = 원소개수
	m_grdRate.SetColumnCount(nCount);

	// 열의 넓이 설정
	int i, nWidth[] = {70, 79, 80, 80, 60, 80, 80, 80, 90, 0, 0, 0, 0};
	CString strHeader[] = {"신용구분", "종목코드", "종목명", "현재가", "보유수량", "평가금액", "평가손익", "수익률", "매입금액", "현재가(계산용)", "보유수량(계산용)", "매입금액(계산용)", "평가금액(계산용)"};

	int nCnt = sizeof(nWidth) / sizeof(*nWidth);		// 전체크기 / 원소크기 = 원소개수
	for (i = 0; i < nCnt; i++)
	{
		m_grdRate.SetColumnWidth(i, nWidth[i]);
		m_grdRate.SetItemFormat(0, i, DT_CENTER);
		m_grdRate.SetItemText(0, i, strHeader[i]);
		m_grdRate.SetItemBkColour(0, i, clr);	// 지정된 셀의 배경색 설정
	}

	// 행의 높이 설정
	m_grdRate.SetRowHeight(0, 24);

	m_grdRate.Invalidate();
}

void CRateDlg::OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{// 조회요청 수신 이벤트

	CString strPrevNext(sPrevNext);
	CString strRQName(sRQName);
	if (strRQName == _T("계좌수익률"))			// 계좌수익률
	{
		m_bNextFlag = FALSE;
		CString strData;
		int nFieldCnt = sizeof(lstFID) / sizeof(*lstFID);		// 전체크기 / 원소크기 = 원소개수

		strRQName = _T("계좌수익률");
		int i, j, nCnt = theApp.m_khOpenApi.GetRepeatCnt(sTrcode, strRQName);	//데이터 건수
		m_nCount += (nCnt+1);	//데이터 건수에 그리드 헤더 추가
		m_grdRate.SetRowCount(m_nCount);

		for (i = 0; i < nCnt; i++)
		{
			CString strCode;

			for (j = 0; j < nFieldCnt; j++)
			{
				strData = theApp.m_khOpenApi.GetCommData(sTrcode, strRQName, i, lstFID[j].strKey);
				strData.Trim();

				if (j == 0) //신용구분
				{
					if (strData == _T("00"))
					{
						strData = _T("현금");
					} 
					else if (strData == _T("03"))
					{
						strData = _T("융자");
					}
					else if (strData == _T("99"))
					{
						strData = _T("융자합");
					}
				}
				else if (j == 1)	//종목명
				{
					strCode = strData;
				}

				m_grdRate.SetItemFormat(i + 1 + m_nNextRow, lstFID[j].nCol, lstFID[j].nAlign);
				if (strData != _T(""))
				{
					m_grdRate.SetItemText(i + 1 + m_nNextRow, j, theApp.ConvDataFormat(lstFID[j].nDataType, strData, lstFID[j].strBeforeData, lstFID[j].strAfterData));
				}
			}

			int nRow(0);
			nRow = i + 1 + m_nNextRow;

			//종목별 수익률 계산
			SetRate(nRow, strCode);

			CString strIndex;
			strIndex.Format(_T("%d"), nRow);

			//종목을 키값으로 해서 로우값을 관리한다.
			m_mapJongCode.SetAt(strCode, strIndex);
		}

		//총 수익률 계산
		SetTotalRate();

		if (strPrevNext == _T("2"))	//연속조회
		{
			m_bNextFlag = TRUE;	//연속조회여부
			m_nCount = m_nCount - 1;	//그리드 헤더Row수/를 뺀다.
			m_nNextRow = m_nCount;	//연속조회로 마지막 Row값을 저장.
			
			CString strAccNo;
			m_EdtAcc.GetWindowText(strAccNo);	//입력한 계좌번호.

			//연속조회를 한다.
			theApp.m_khOpenApi.SetInputValue(_T("계좌번호"), strAccNo);
			theApp.m_khOpenApi.CommRqData(_T("계좌수익률"), _T("OPT10085"), 2, m_strScrNo);
		}

		//연속조회가 아닐때만 실시간 등록을 한다.
		if (!m_bNextFlag)
		{
			CString strRQName = _T("복수종목"), strCodeList, strCode;
			long  nCodeCount(0);

			for (int nRow = 1; nRow < m_nCount; nRow++)
			{
				nCodeCount++;
				strCode = m_grdRate.GetItemText(nRow, 1);
				strCode.Trim();
				strCodeList += strCode + _T(";");
			}

			long lRet = theApp.m_khOpenApi.CommKwRqData(strCodeList, 0, nCodeCount, 0, strRQName, m_strScrNo);
			if (!lRet)
			{
				return;
			}
		}

		m_grdRate.Invalidate();
	}
}

void CRateDlg::OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{// 주문이나 계좌관련 조회 서버 메시지 수신 이벤트
}

void CRateDlg::OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{// 실시간 시세데이터 수신 이벤트

	CString strIndex, strData;
	if (!m_mapJongCode.Lookup(sJongmokCode, strIndex))
	{
		return;
	}

	CString strCode;

	int i, nFieldCnt = sizeof(lstFID) / sizeof(*lstFID);		// 전체크기 / 원소크기 = 원소개수
	for (i = 0; i < nFieldCnt; i++)
	{
		if (atoi(lstFID[i].strRealKey) < 0)
		{
			continue;
		}

		//실시간 데이터를 항목에 맞게 가져온다.
		strData = theApp.m_khOpenApi.GetCommRealData(sJongmokCode, atoi(lstFID[i].strRealKey));	strData.Trim();

		//항목에 맞는 데이터가 있을때만 그리드에 표시한다.
		if (strData != _T(""))
		{
			m_grdRate.SetItemText(_ttoi(strIndex), i, theApp.ConvDataFormat(lstFID[i].nDataType, strData, lstFID[i].strBeforeData, lstFID[i].strAfterData));
		}
	}

	//종목별 수익률 계산
	strCode = sJongmokCode;
	SetRate(_ttoi(strIndex), strCode);

	//총 수익률 계산
	SetTotalRate();

	m_grdRate.Invalidate();
}

void CRateDlg::OnReceiveChejanData(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList)
{// 실시간 주문,잔고통보 수신 이벤트

	CString strGuBun(sGubun), strAccNo, strAcc;

	strAccNo	= theApp.m_khOpenApi.GetChejanData(9201);	// 체결된 종목의 계좌번호
	m_EdtAcc.GetWindowText(strAcc);									// 현재 계좌번호

	//현재 수익률 조회한 계좌번호에 있는 체결된 종목만 받을 수 있게 처리.
	if (strAccNo == strAcc)
	{
		if (strGuBun == _T("4"))
		{
			OnBtnSearch(); //보유종목을 재조회한다.
		}
	}
}

void CRateDlg::OnBtnSearch()
{// 계좌 수익률 조회요청

	CString strAccNo;
	m_EdtAcc.GetWindowText(strAccNo);	//입력한 계좌번호.
	if(strAccNo.IsEmpty())
	{
		AfxMessageBox(_T("수익률 조회할 계좌번호 10자리를 입력해주세요."));
		return;
	}
	
	m_grdRate.SetRowCount(1);		// 행/열 갯수 설정
	m_nCount = 0;
	m_nNextRow = 0;

	//수익률계산 조회
	theApp.m_khOpenApi.SetInputValue(_T("계좌번호"), strAccNo);
	long lRet = theApp.m_khOpenApi.CommRqData(_T("계좌수익률"), _T("OPT10085"), 0, m_strScrNo);

	if (!theApp.IsError(lRet))
	{
	}
}

void CRateDlg::SetRate(int nRow, CString strCode)
{// 종목별 수익률 계산

	//주식 종목에 대한 수익률 계산이라 주식외 종목의 수익률은 다를수 있음을 알려드립니다.
	//현재가
	CString strData, strTmp;
	strData = m_grdRate.GetItemText(nRow, 9);
	strTmp = strData.Left(1);
	if (strTmp == _T("-") || strTmp == _T("+"))
	{
		strData = strData.Mid(1);
	}
	double nPrice = _ttof(strData);

	//수량
	double nQty = _ttof(m_grdRate.GetItemText(nRow, 10));

	//매입금액
	double nPurchase = _ttof(m_grdRate.GetItemText(nRow, 11));

	int nCount(0);
	nCount = m_grdRate.GetRowCount();

	//매수가계산 수수료 = 매입금액 * 매체수수료(0.015%) (10원미만 절사)
	m_BuyComm = nPurchase * m_dFee;
	m_BuyComm = floor(m_BuyComm / 10) * 10;	//(10원미만 절사)

	//매도가계산 수수료 = 현재가 * 수량 * 매체수수료(0.015%) (10원미만 절사)
	m_SellComm = nPrice * nQty * m_dFee;
	m_SellComm = floor(m_SellComm / 10) * 10;	//(10원미만 절사)

	//제세금
	long lMarketType = theApp.m_khOpenApi.GetMarketType(strCode);
	if (lMarketType == 0) //거래소
	{// 제세금(거래소) = 현재가 * 수량 * 0.15%(농어촌특별세) + 현재가 * 수량 * 0.05%(거래세)
		m_Tax = nPrice * nQty * COMMISSION1;
		m_Tax = floor(m_Tax);
		
		double dTax = nPrice * nQty * COMMISSION3;
		dTax = floor(dTax);
		m_Tax = m_Tax + dTax;		
	} 
	else if (lMarketType == 10)//코스닥
	{
		//제세금(코스닥) = 현재가 * 수량 * 0.3%(원미만 절사)
		m_Tax = floor(nPrice * nQty * COMMISSION2);
	}

	CString strTemp;

	//종목별 평가금액 = (현재가 * 수량) - 매수가계산 수수료 - 매도가계산 수수료 - 제세금 가계산
	m_Eval = (nPrice * nQty) - m_BuyComm - m_SellComm - m_Tax;
	strTemp.Format(_T("%0.lf"), m_Eval);
	m_grdRate.SetItemText(nRow, 5,  theApp.ConvDataFormat(lstFID[5].nDataType, strTemp, lstFID[5].strBeforeData, lstFID[5].strAfterData));
	m_grdRate.SetItemText(nRow, 12,  theApp.ConvDataFormat(lstFID[12].nDataType, strTemp, lstFID[12].strBeforeData, lstFID[12].strAfterData));

	//종목별 평가손익 = 평가금액 - 매입금액
	m_Profit = m_Eval - nPurchase;
	strTemp.Format(_T("%0.lf"), m_Profit);
	m_grdRate.SetItemText(nRow, 6,  theApp.ConvDataFormat(lstFID[6].nDataType, strTemp, lstFID[6].strBeforeData, lstFID[6].strAfterData));

	//종목별 수익률 = 평가손익 / 매입금액 * 100
	if (nPurchase == 0)
	{
		m_Rate = 0;
	}
	else
	{
		m_Rate = m_Profit / nPurchase * 100;
	}
	strTemp.Format(_T("%0.2lf %%"), m_Rate);
	m_grdRate.SetItemText(nRow, 7,  theApp.ConvDataFormat(lstFID[7].nDataType, strTemp, lstFID[7].strBeforeData, lstFID[7].strAfterData));
}

void CRateDlg::SetTotalRate()
{// 전체 수익률 계산
	int nCnt = m_grdRate.GetRowCount();	//그리드 Row 갯수 구하기
	double dTotalEval(0), dTotalPurchase(0), dTotalProfit, dTotalRate(0);
	CString strTotalEval, strTotalPurchase, strTotalProfit, strTotalRate;

	for (int i = 1; i < nCnt; i++)
	{
		dTotalEval += _ttof(m_grdRate.GetItemText(i, 12));			// 총 평가금액
		dTotalPurchase += _ttof(m_grdRate.GetItemText(i, 11));	// 총 매입금액
	}
	
	strTotalEval.Format(_T("%0.lf"), dTotalEval);
	strTotalEval = theApp.ConvDataFormat(DT_ZERO_NUMBER, strTotalEval);
	m_EdtTotalEval.SetWindowText(strTotalEval);

	strTotalPurchase.Format(_T("%0.lf"), dTotalPurchase);
	strTotalPurchase = theApp.ConvDataFormat(DT_ZERO_NUMBER, strTotalPurchase);
	m_EdtTotalPurchase.SetWindowText(strTotalPurchase);


	// 총 평가손익
	dTotalProfit = dTotalEval - dTotalPurchase;
	strTotalProfit.Format(_T("%0.lf"), dTotalProfit);
	strTotalProfit = theApp.ConvDataFormat(DT_ZERO_NUMBER, strTotalProfit);
	m_EdtTotalProfit.SetWindowText(strTotalProfit);

	// 총 수익률
	dTotalRate = dTotalProfit / dTotalPurchase * 100;
	strTotalRate.Format(_T("%0.2lf"), dTotalRate);
	strTotalRate = theApp.ConvDataFormat(DT_NONE, strTotalRate, _T(""), _T("%"));
	m_EdtTotalRate.SetWindowText(strTotalRate);
}