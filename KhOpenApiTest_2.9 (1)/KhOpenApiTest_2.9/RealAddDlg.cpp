//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2015 (주)한국소리마치                /
//******************************************************************/
//! File Name     : RealAddDlg.cpp
//! Function      : 리얼등록 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2015/04/01
//! Update        : 
//! Comment       : 
//******************************************************************/
// RealAddDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "KhOpenApiTest.h"
#include "RealAddDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define FIDLIST	_T("9001;302;10;11;25;12;13")

// {조회 키,		리얼 키,	행, 열, 타입,			색 변경, 정렬, 앞 문자, 뒷 문자}
const stGRID lstOPTSBFID[] = 
{
	{_T("종목코드"),		_T("9001"),	-1,	0,	DT_NONE			, FALSE	, DT_CENTER	,	_T(""),	_T("")}, 
	{_T("종목명"),		_T("302"),	-1,	1,	DT_NONE			, FALSE	, DT_LEFT	,	_T(""),	_T("")}, 
	{_T("현재가"),		_T("10"),	-1,	2,	DT_ZERO_NUMBER	, TRUE	, DT_RIGHT	,	_T(""),	_T("")}, 
	{_T("전일대비기호"),	_T("25"),	-1,	3,	DT_SIGN			, TRUE	, DT_CENTER	,	_T(""),	_T("")}, 
	{_T("전일대비"),		_T("11"),	-1,	4,	DT_ZERO_NUMBER	, TRUE	, DT_RIGHT	,	_T(""),	_T("")}, 
	{_T("등락률"),		_T("12"),	-1,	5,	DT_ZERO_NUMBER	, TRUE	, DT_RIGHT	,	_T(""),	_T("")}, 
	{_T("거래량"),		_T("13"),	-1,	6,	DT_ZERO_NUMBER	, FALSE	, DT_RIGHT	,	_T(""),	_T("")}, 
};

CRealAddDlg::CRealAddDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRealAddDlg::IDD, pParent)
{
	m_pParent = pParent;
	m_strScrNo = _T("9999");
	m_cellSelect.row = 0;
	m_cellSelect.col = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strPreConditionName = _T("");
	m_strConditionName = _T("");
	m_nPreConditionIndex = -1;	
	m_nConditionIndex = -1;
}

void CRealAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRealAddDlg)
	DDX_Control(pDX, IDC_GRD_REALADD,		m_grdRealAdd);
	DDX_Control(pDX, IDC_CMB_CON,			m_cmbCon);
	DDX_Control(pDX, IDC_LIST_INSERT,		m_listCtl_Insert);
	DDX_Control(pDX, IDC_LIST_DELETE,		m_listCtl_Delete);
	//}}AFX_DATA_MAP
}

//*******************************************************************/
// BEGIN_MESSAGE_MAP
//*******************************************************************/
BEGIN_MESSAGE_MAP(CRealAddDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_REAL_REG,		&CRealAddDlg::OnBnRealReg)
	ON_BN_CLICKED(IDC_BTN_REAL_REMOVE,	&CRealAddDlg::OnBnRealRemove)
	ON_BN_CLICKED(IDC_BTN_REAL_REMOVE_ALL,	&CRealAddDlg::OnBnRealRemoveAll)
	ON_BN_CLICKED(IDC_BTN_CONDITION_LOAD, &CRealAddDlg::OnBnConditionLoad)
	ON_BN_CLICKED(IDC_BTN_COND_SEARCH, &CRealAddDlg::OnBnConditionSearch)
	ON_BN_CLICKED(IDC_BTN_COND_SEARCH_REAL, &CRealAddDlg::OnBnSearchReal)
	ON_BN_CLICKED(IDC_BTN_COND_SEARCH_REAL_STOP, &CRealAddDlg::OnBnSearchRealStop)	
END_MESSAGE_MAP()

BOOL CRealAddDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	InitRealAddGrid();				// 그리드 초기 처리

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRealAddDlg::OnPaint()
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

void CRealAddDlg::OnClose()
{
	// 화면 닫을 때 부모윈도우에게 통보(화면 관리 위해)
	if (m_pParent)
	{
		//시세 실시간 해제
		theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));

		if (m_strConditionName != "" && m_nConditionIndex > -1)
		{
			theApp.m_khOpenApi.SendConditionStop(m_strScrNo, GetConditionName(m_strConditionName), m_nConditionIndex);
		}
		int nLen = m_strScrNo.GetLength();
		char *cScrNo = new char[nLen + 1];
		memset(cScrNo, 0x00, nLen + 1);
		memcpy(cScrNo, m_strScrNo, nLen);
		m_pParent->PostMessage(UM_SCRENN_CLOSE, 0U, (LPARAM)cScrNo);
	}

	CDialogEx::OnClose();
}

//******************************************************************/
HCURSOR CRealAddDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRealAddDlg::OnBnRealReg()
{//실시간 시세 등록 함수 호출
	//종목에 대한 현재가, 대비, 등락률, 거래량등에 대한 실시간을 받기 위해 반드시 호출해야함.
	int nCount = m_grdRealAdd.GetRowCount();
	CString strCodeList;
	
	for (int i = 1; i < nCount; i++)
	{
		strCodeList += m_grdRealAdd.GetItemText(i, 0) + _T(";");
	}

	long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, strCodeList, FIDLIST, _T("1"));
}

void CRealAddDlg::OnBnRealRemove()
{// 실시간 시세 해제.
	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));
}

// 그리드 초기 처리
void CRealAddDlg::InitRealAddGrid()
{
	m_grdRealAdd.SetEditable(false);				//cell을 에디트 못하게 함.
	m_grdRealAdd.EnableScrollBars(SB_BOTH, FALSE);

	COLORREF clr = RGB(215, 227, 241);

	// 고정 행/열 설정
	m_grdRealAdd.SetFixedRowCount(1);

	// 행/열 갯수 설정
	m_grdRealAdd.SetRowCount(1);
	m_grdRealAdd.SetColumnCount(8);

	// 열의 넓이 설정
	int i, nWidth[] = {60, 130, 80, 20, 80, 70, 100, 15};
	CString strHeader[] = {_T("종목코드"), _T("종목명"), _T("현재가"), _T(""), _T("전일대비"), _T("등락률"), _T("거래량"), _T("")};
	int nCnt = sizeof(nWidth) / sizeof(*nWidth);		// 전체크기 / 원소크기 = 원소개수
	for (i = 0; i < nCnt; i++)
	{
		m_grdRealAdd.SetColumnWidth(i, nWidth[i]);
		m_grdRealAdd.SetItemFormat(0, i, DT_CENTER);
		m_grdRealAdd.SetItemText(0, i, strHeader[i]);
		m_grdRealAdd.SetItemBkColour(0, i, clr);	// 지정된 셀의 배경색 설정
	}

	// 행의 높이 설정
	m_grdRealAdd.SetRowHeight(0, 24);

	m_grdRealAdd.Invalidate();
}

void CRealAddDlg::OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{// 시세조회 수신 이벤트
	return;
}

void CRealAddDlg::OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{// 메시지 수신 이벤트
}

void CRealAddDlg::OnReceiveRealDataKhopenapictrl(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{// 실시간 시세 데이터 수신처리
	CString strIndex;

	if (!m_mapJongCode.Lookup(sJongmokCode, strIndex))
	{
		return;
	}

	CString strData;
	CStringArray arrData;

	arrData.Add(sJongmokCode);
	int i, nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// 전체크기 / 원소크기 = 원소개수
	for (i = 1; i < nFieldCnt; i++)
	{
		if (atoi(lstOPTSBFID[i].strRealKey) < 0)
		{
			arrData.Add("");
			continue;
		}
		strData = theApp.m_khOpenApi.GetCommRealData(sJongmokCode, atoi(lstOPTSBFID[i].strRealKey));	strData.Trim();
		arrData.Add(strData);
	}
	SetDataRealAddGrid(arrData, sRealType);
}

void CRealAddDlg::OnReceiveRealCondition(LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex)
{// 실시간 조건검색 결과 수신 (종목 편입/이탈)

	CString strName, strMsg, strIndex, sType, sCode;
	sType = strType;	//종목 편입, 이탈 구분
	sCode = strCode;	//종목코드
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strName);	//현재 선택된 조건명

	if (GetConditionName(strName) == strConditionName)	//현재 선택된 조건명과 실시간으로 들어온 조건명이 같은지 비교.
	{
		if (sType == _T("I"))	//종목 편입
		{
			m_grdRealAdd.InsertRow(sCode, 1);

			// 종목명/
			CString strCodeName;
			strCodeName = theApp.m_khOpenApi.GetMasterCodeName(sCode);
			m_grdRealAdd.SetItemText(1, 1, strCodeName);

			//하단 메세지 보이기
			//메세지 5줄만 넘으로 클리어하기.
			if (m_listCtl_Insert.GetCount() > 5)
			{
				m_listCtl_Insert.ResetContent();
			}

			strMsg.Format(_T("[%s] 종목이 편입되었습니다."), sCode);
			m_listCtl_Insert.AddString(strMsg);

			//기존조회된 종목의 시세가 들어오고 있는 상황에서 추가로 편입된 종목의 시세를 보여줄때 
			// 마지막 파라메터에 "1"을 넣어주어야 기존 종목과 함께 편입된 종목의 시세가 들어온다.
			// 만약 "1" 이외의 "0"을 넣어주면 기존에 등록한 종목들에 대한 시세가 끊어진다.
			//실시간등록 함수 호출
			long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, strCode, FIDLIST, _T("1"));
		} 
		else if (sType == _T("D"))	//종목 이탈
		{
			//기존에 있던 종목인지 확인.
			if (!m_mapJongCode.Lookup(sCode, strIndex))
			{
				return;
			}

			//이탈 종목 삭제.
			m_grdRealAdd.DeleteRow(_ttoi(strIndex));

			//이탈된 종목의 시세를 받지 않을려면 사용한다.
			theApp.m_khOpenApi.SetRealRemove(m_strScrNo, sCode);

			//하단 메세지 보이기
			//메세지 5줄만 넘으로 클리어하기.
			if (m_listCtl_Delete.GetCount() > 5)
			{
				m_listCtl_Delete.ResetContent();
			}

			strMsg.Format(_T("[%s] 종목이 이탈되었습니다."), sCode);
			m_listCtl_Delete.AddString(strMsg);
		}

		m_grdRealAdd.Invalidate();

 		m_mapJongCode.RemoveAll();

		int nCount = m_grdRealAdd.GetRowCount();
		for (int i = 1; i < nCount; i++)
		{
			sCode = m_grdRealAdd.GetItemText(i, 0);
			strIndex.Format(_T("%d"), i);
			m_mapJongCode.SetAt(sCode, strIndex);
		}
	}
}

//*******************************************************************/
//! Function Name	: OnReceiveTrCondition
//! Function		: 조건검색 종목리스트 조회수신
//! Param			: LPCTSTR sScrNo					- 화면번호
//!					: PCTSTR strCodeList				- 종목리스트
//!					: LPCTSTR strConditionName			- 조건명
//!					: int nIndex						- 조건식 고유번호
//!					: int nNext							- 연속조회(2:연속조회, 0:연속조회없음)
//! Return        : void
//! Create        : 2015/04/01
//! Comment       : 
//******************************************************************/
void CRealAddDlg::OnReceiveTrCondition(LPCTSTR sScrNo,PCTSTR strCodeList, LPCTSTR strConditionName, int nIndex, int nNext)
{// 조건검색 요청 결과 수신
	CString strCndName, sConditionName;
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strCndName);	//선택된 조건명

	if (strConditionName == GetConditionName(strCndName))	// 선택한 조건명과 수신한 조건명이 같을때만 처리.
	{
		if (strCodeList != _T(""))
		{
			int nIndex(0);
			CString strConditionCode;
			m_mapJongCode.RemoveAll();		// 종목코드 초기화
			m_grdRealAdd.SetRowCount(1);

			while ( AfxExtractSubString( strConditionCode, strCodeList, nIndex++, _T(';')) )
			{// 검색한 종목수를 구해서 그 수만큼 Row추가
			}
		
			m_grdRealAdd.SetRowCount(nIndex-1);

			nIndex = 0;
			CString strIndex, strCodeName;
			int nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// 전체크기 / 원소크기 = 원소개수
			
			while ( AfxExtractSubString( strConditionCode, strCodeList, nIndex++, _T(';')) )
			{
				if (strConditionCode.IsEmpty())
					continue;
				
				for(auto i = 0; i < nFieldCnt; i++)
				{
					m_grdRealAdd.SetItemFormat(nIndex, lstOPTSBFID[i].nCol, lstOPTSBFID[i].nAlign);
				}

				// 종목코드
				m_grdRealAdd.SetItemText(nIndex, 0, strConditionCode);

				// 종목명
				strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strConditionCode).Trim();
				m_grdRealAdd.SetItemText(nIndex, 1, strCodeName);

				strIndex.Format("%d", nIndex);
				m_mapJongCode.SetAt(strConditionCode, strIndex);			
			}
		}
	}
}

void CRealAddDlg::OnReceiveConditionVer(long lRet, LPCTSTR sMsg)
{// 사용자 조건식 수신하고 고유번호와 조건식 이름으로 파싱후 저장	
	if (lRet != 1)
	{
		CString		strMsg(sMsg);
		CString		strErr;
		strErr.Format(_T("사용자 조건읽기 실패 \r\n\r\n[%s]"), strMsg);
		AfxMessageBox(strErr);
		return;
	}

	//조건검색명 리스트를 불러온다.
	CString strCondition, strConditionName, strIndex;
	CString strConditionNameList = theApp.m_khOpenApi.GetConditionNameList();

	//받아온 조건검색명을 콤보에 넣는다.
	m_cmbCon.ResetContent();

	int i(0);

	while( AfxExtractSubString( strCondition, strConditionNameList, i++, _T(';')) )
	{
		if(strCondition.IsEmpty())
			continue;

		AfxExtractSubString(strIndex			, strCondition, 0, _T('^'));// 고유번호
		AfxExtractSubString(strConditionName	, strCondition, 1, _T('^'));// 조건식 이름
			
		m_mapNameList.SetAt(strCondition, strIndex);
		m_cmbCon.AddString(strCondition);
	}

	m_cmbCon.SetCurSel(0);
}

void CRealAddDlg::SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/)
{// 그리드 데이타 설정

	// 종목코드에 맞는 행 찾기
	CString strData = arrData.GetAt(0), strTemp;
	if (m_mapJongCode.Lookup(strData, strTemp) == FALSE)
	{
		return;
	}

	int i, nRow = atol(strTemp), nCnt = arrData.GetSize();

	// 데이타 설정
	CString strTempData;
	for (i = 1; i < nCnt; i++)
	{
		strData = arrData.GetAt(i);
		/////////////////////////////////////////////
		// 변경 된 데이터 선택 처리↓↓↓↓↓↓↓↓↓
		if (strRealType != _T("") && (i == 2 || i == 4 || i == 5))
		{
			strTempData = strData;
			strTempData.Replace(_T("+"), _T(""));
			strTempData.Replace(_T("-"), _T(""));	
			strTempData.Replace(_T("."), _T(""));

			strTemp = m_grdRealAdd.GetItemText(nRow, lstOPTSBFID[i].nCol);
			strTemp.Replace(_T("%"), _T(""));
			strTemp.Replace(_T("+"), _T(""));
			strTemp.Replace(_T("-"), _T(""));
			strTemp.Replace(_T(","), _T(""));	
			strTemp.Replace(_T("."), _T(""));
			if (strTempData != strTemp)
			{
				m_cellSelect.col = lstOPTSBFID[i].nCol;
				m_grdRealAdd.SetFocusCell(m_cellSelect);
				m_grdRealAdd.SetSelectedRange(nRow, m_cellSelect.col, nRow, m_cellSelect.col);
				m_grdRealAdd.Invalidate();
			}
		}
		// 변경 된 데이터 선택 처리↑↑↑↑↑↑↑↑↑
		/////////////////////////////////////////////
		if (lstOPTSBFID[i].bTextColor)
		{
			strTemp = arrData.GetAt(3);
			if (strTemp == _T("1") || strTemp == _T("2"))	// 부호에 따라 색상변경
			{
				strTemp = _T("1");
			}
			else if (strTemp == _T("4") || strTemp == _T("5"))	// 부호에 따라 색상변경
			{
				strTemp = _T("-1");
			}
			else
			{
				strTemp = _T("0");
			}
			theApp.SetDataFgColour(&m_grdRealAdd, nRow, lstOPTSBFID[i].nCol, strTemp);
		}
		if (lstOPTSBFID[i].nDataType == DT_SIGN)
		{
			if (strData.IsEmpty())
			{
				continue;
			}
			theApp.SetSignData(&m_grdRealAdd, nRow, lstOPTSBFID[i].nCol, strData);
		}
		else
		{
			if (strData.IsEmpty())
			{
				continue;
			}
			int nCol = lstOPTSBFID[i].nCol;
			m_grdRealAdd.SetItemText(nRow, lstOPTSBFID[i].nCol, theApp.ConvDataFormat(lstOPTSBFID[i].nDataType, strData, lstOPTSBFID[i].strBeforeData, lstOPTSBFID[i].strAfterData));
		}
	}
	m_grdRealAdd.Invalidate();
}

void CRealAddDlg::OnBnRealRemoveAll()
{// 전화면 실시간 시세 등록 해제
	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));
}

void CRealAddDlg::OnBnConditionLoad()
{// 서버에 사용자 조건식 요청
	long lRet = theApp.m_khOpenApi.GetConditionLoad();
	if(lRet > 0)
	{
		m_cmbCon.ResetContent();
		m_mapNameList.RemoveAll();
	}
}

void CRealAddDlg::OnBnConditionSearch()
{// 조건검색 요청
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), m_strConditionName);
	CString strIndex;
	if(m_mapNameList.Lookup(m_strConditionName, strIndex) == FALSE)
	{/// 선택한 조건식 못찾음.
		return;
	}

	m_nConditionIndex		= _ttoi(strIndex);
	m_strPreConditionName	= m_strConditionName;
	m_nPreConditionIndex	= m_nConditionIndex;

	// 종목리스트는 OnReceiveTrCondition()을 통하여 들어온다.
	long lRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, GetConditionName(m_strConditionName), m_nConditionIndex, 0);
	if (lRet == 0)
	{//에러
		return;
	}
}

void CRealAddDlg::OnBnSearchReal()
{// 실시간 조건 검색 요청
	CString strConditionCodeList = _T(""), strIndex;

	//선택한 조건명의 검색된 종목리스트를 불러온다.
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), m_strConditionName);
	if(m_mapNameList.Lookup(m_strConditionName, strIndex) == FALSE)
	{/// 선택한 조건식 못찾음.
		return;
	}

	m_nConditionIndex		= _ttoi(strIndex);
	m_strPreConditionName	= m_strConditionName;
	m_nPreConditionIndex	= m_nConditionIndex;

	//조검검색 실시간 조회
	//종목리스트는 OnReceiveTrCondition()을 통하여 들어온다.
	//실시간 조건 검색을 하고 싶으면 마지막 파라메터에 1로 준다.
	long lRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, GetConditionName(m_strConditionName), m_nConditionIndex, 1);
	if (lRet == 0)
	{//에러
		return;
	}

	/// 종목편입, 이탈 로그 삭제
	m_listCtl_Insert.ResetContent();
	m_listCtl_Delete.ResetContent();
}

void CRealAddDlg::OnBnSearchRealStop()
{// 실시간 조건검색 중지
	m_strPreConditionName = _T("");
 	CString strConditionName, strIndex;
 	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strConditionName);
	if(m_mapNameList.Lookup(m_strConditionName, strIndex) == TRUE)
	{
		int nConditionIndex = _ttoi(strIndex);
		theApp.m_khOpenApi.SendConditionStop(m_strScrNo, GetConditionName(strConditionName), nConditionIndex);
	}
}


//"0^조건식" 스트링에서 순수 조건검색이름을 구하기 위한함수.
CString CRealAddDlg::GetConditionName(CString strTotCondition)
{
	CString strRet;

	if (strTotCondition.Find('^') >= 0)
		AfxExtractSubString(strRet	, strTotCondition, 1, _T('^'));
	else
		strRet = strTotCondition;


	return strRet;
}