
// PadDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TankPad.h"
#include "PadDlg.h"
#include "afxdialogex.h"
#include "mix.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const LPCTSTR g_svgButtonId[(int)EButton::MAX] =
{ L"", L"dpadup", L"dpaddown", L"dpadleft", L"dpadright", L"r1", L"r2", L"r3",
L"l1", L"l2", L"l3", L"options", L"share", L"triangle", L"x", L"rect", L"circle",
L"ps", L"touchdown", L"leftstick", L"rightstick" };

CPadDlg::CPadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TANKPAD_DIALOG, pParent),
	m_svgView(this),
	m_boldFont(FW_BOLD),
	m_listFont(FW_NORMAL, 11)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAMEPAD_NAME, m_padName);
	DDX_Control(pDX, IDC_ACTIONLIST, m_actList);
	DDX_Control(pDX, IDC_INVERTY, m_invertCamY);
	DDX_Control(pDX, IDC_SINGLEPRESS, m_singlePress);
	DDX_Control(pDX, IDC_SENS, m_senSlider);
	DDX_Control(pDX, IDC_ASSIGN, m_assignBtn);
	DDX_Control(pDX, IDC_SVG, m_svgView);
}

BEGIN_MESSAGE_MAP(CPadDlg, CDialogEx)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_MESSAGE(MSG_ASSIGN_BTN, OnAssignButton)
	ON_MESSAGE(MSG_PROP_SEL_CHANGED, OnSelChanged)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_INVERTY, &CPadDlg::OnBnClickedInverty)
	ON_BN_CLICKED(IDC_RESET, &CPadDlg::OnBnClickedReset)
	ON_WM_ACTIVATE()
	ON_BN_CLICKED(IDC_ASSIGN, &CPadDlg::OnBnClickedAssign)
	ON_BN_CLICKED(IDC_MFCLINK2, &CPadDlg::OnBnClickedMfclink2)
	ON_BN_CLICKED(IDC_SINGLEPRESS, &CPadDlg::OnBnClickedSinglepress)
END_MESSAGE_MAP()

// CPadDlg message handlers

BOOL CPadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_mapping.LoadSettings();
	InitControls();
	DeviceRemoved();
	InitGamepad(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPadDlg::InitControls()
{
	m_svgView.LoadResource(IDR_DS4, 96);
	m_svgView.SetCursor(IDC_HAND);

	m_invertCamY.SetCheck(m_mapping.inverseY);
	m_singlePress.SetCheck(m_mapping.singlePress);
	
	m_senSlider.SetRange(1, 9);
	m_senSlider.SetPos(m_mapping.cameraSensCoeff);
	UpdateSensText();

	InitActionList();

	SetDlgItemTextW(IDC_GAMEPAD_LABEL, GetLocalText(ELocText::GamepadLabel));
	SetDlgItemTextW(IDC_INVERTY, GetLocalText(ELocText::InvertCameraY));
	SetDlgItemTextW(IDC_RESET, GetLocalText(ELocText::ResetDefault));
	SetDlgItemTextW(IDC_ASSIGN, GetLocalText(ELocText::AssignButton));
	SetDlgItemTextW(IDC_SVG_HELP, GetLocalText(ELocText::SvgAssignHelp));
	SetDlgItemTextW(IDC_SINGLEKEY, GetLocalText(ELocText::SingleKey));
	
	m_assignBtn.EnableWindowsTheming(FALSE);
	m_assignBtn.m_nFlatStyle = CMFCButton::BUTTONSTYLE_SEMIFLAT;
	m_assignBtn.SetTooltip(GetLocalText(ELocText::AssignButtonTip));
	m_assignBtn.m_bTransparent = FALSE;
	UpdateAssignBtn(false);

	m_boldFont.Apply(&m_padName);
	m_listFont.Apply(&m_actList);
}

void CPadDlg::InitActionList()
{
	m_actList.RemoveAll();

	// headers
	HDITEMW header;
	header.pszText = (LPWSTR)GetLocalText(ELocText::ActionColumn);
	header.mask = HDI_TEXT | HDI_WIDTH;
	header.cxy = m_actList.GetListRect().Size().cx / 2;
	m_actList.GetHeaderCtrl().SetItem(0, &header);

	header.pszText = (LPWSTR)GetLocalText(ELocText::ButtonColumn);
	m_actList.GetHeaderCtrl().SetItem(1, &header);

	// move 
	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(GetActionName(EAction::Move),
		GetButtonName(m_mapping.GetActionButton(EAction::Move)));
	pProp->AddOption(GetButtonName(EButton::LeftStick));
	pProp->AddOption(GetButtonName(EButton::RightStick));
	pProp->AllowEdit(FALSE);
	pProp->SetData((DWORD_PTR)EAction::Move); // save action index
	m_actList.AddProperty(pProp);

	// camera
	pProp = new CMFCPropertyGridProperty(GetActionName(EAction::Camera),
		GetButtonName(m_mapping.GetActionButton(EAction::Camera)));
	pProp->AddOption(GetButtonName(EButton::LeftStick));
	pProp->AddOption(GetButtonName(EButton::RightStick));
	pProp->AllowEdit(FALSE);
	pProp->SetData((DWORD_PTR)EAction::Camera); // save action index
	m_actList.AddProperty(pProp);

	// other
	for (int i = 2; i < m_mapping.GetActionCount(); i++)
	{
		pProp = new CMFCPropertyGridProperty(GetActionName((EAction)i),
			GetButtonName(m_mapping.GetActionButton((EAction)i)));
		AddButtonNames(pProp);
		pProp->AllowEdit(FALSE);
		pProp->SetData((DWORD_PTR)i); // save action index
		m_actList.AddProperty(pProp);
	}

	m_actList.SetCurSel(m_actList.GetProperty(0));
}

void CPadDlg::AddButtonNames(CMFCPropertyGridProperty* prop)
{
	ASSERT(prop);

	for (int i = 0; i < (int)EButton::LeftStick; i++)
	{
		prop->AddOption(GetButtonName((EButton)i));
	}
}

void CPadDlg::DeviceAttached(SGamepadData& device)
{
	if (!device.isDualShock)
	{
		m_svgView.LoadResource(IDR_XBOXONE);
	}
	else
	{
		m_svgView.LoadResource(IDR_DS4);
	}

	UpdateGamepadImage();

	m_padName.SetWindowTextW(device.description);	

	UpdateAssignBtn(true);
}

void CPadDlg::DeviceRemoved()
{
	SetDlgItemTextW(IDC_GAMEPAD_NAME, GetLocalText(ELocText::GamepadNotFound));
	UpdateAssignBtn(false);
}

bool CPadDlg::NeedDetectDevice()
{
	return m_isDialogActive;
}

void CPadDlg::ButtonDown(EButton buttonID)
{
	TRACE(L"ButtonDown\n");

	if (IsAssignButtonMode())
	{
		SendAssignButtonMessage(buttonID);
		return;
	}

	if (!m_isDialogActive)
		m_mapping.ButtonDown(buttonID);
}

void CPadDlg::ButtonUp(EButton buttonID)
{
	TRACE(L"ButtonUp\n");

	if (!m_isDialogActive)
		m_mapping.ButtonUp(buttonID);
}

void CPadDlg::LeftStick(EAxis axis, float value)
{
	if (IsAssignButtonMode())
	{
		SendAssignButtonMessage(EButton::LeftStick, value);
		return;
	}

	if (!m_isDialogActive)
		m_mapping.LeftStick(axis, value);
}

void CPadDlg::RightStick(EAxis axis, float value)
{
	if (IsAssignButtonMode())
	{
		SendAssignButtonMessage(EButton::RightStick, value);
		return;
	}

	if (!m_isDialogActive)
		m_mapping.RightStick(axis, value);

	//::SetWindowText(&m_padName, _T("G: %lf  M: %lf"), abs(value),  m_wot.lastCameraMoveInPixels);
}

LRESULT CPadDlg::OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam)
{
	CMFCPropertyGridProperty* changedProp = (CMFCPropertyGridProperty*)lparam;

	COleVariant newValue = changedProp->GetValue();
	ASSERT(newValue.bstrVal);

	int changedActionIndex = (int)changedProp->GetData();
	EButton newButton = GetButtonByName(newValue.bstrVal);
	EButton currButton = m_mapping.GetActionButton(changedActionIndex);
	
	// update if none and return
	if (newButton == EButton::None)
	{
		m_mapping.SetActionButton(changedActionIndex, EButton::None);
		UpdateGamepadImage(changedActionIndex);
		return(0);
	}

	// check duplicate button assignments
	for (int i = 0; i < m_mapping.GetActionCount(); i++)
	{
		if (i == changedActionIndex) continue;

		currButton = m_mapping.GetActionButton(i);
		EButton noneButton = EButton::None;

		if (currButton == newButton)
		{
			if (changedActionIndex == (int)EAction::Move || 
				changedActionIndex == (int)EAction::Camera) // sticks
			{
				noneButton = (newButton == EButton::LeftStick ? EButton::RightStick : EButton::LeftStick);
			}

			m_mapping.SetActionButton(i, noneButton);
			m_actList.GetProperty(i)->SetValue(GetButtonName(noneButton));
		}
	}
	
	m_mapping.SetActionButton(changedActionIndex, newButton);

	UpdateGamepadImage(changedActionIndex);

	return(0);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CPadDlg::OnCancel()
{
#ifndef _DEBUG
	if (AfxMessageBox(GetLocalText(ELocText::CloseAppQ), MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		return;
#endif
	
	ShutdownGamepad();
	m_mapping.SaveSettings();
	EndDialog(IDOK);
}

void CPadDlg::OnOK()
{}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPadDlg::OnBnClickedInverty()
{
	m_mapping.inverseY = (bool)(m_invertCamY.GetCheck() == BST_CHECKED);
}

void CPadDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = (CSliderCtrl*)(pScrollBar);

	m_senSlider.SetPos(pSlider->GetPos());
	m_mapping.cameraSensCoeff = pSlider->GetPos();

	UpdateSensText();
}

void CPadDlg::UpdateSensText()
{
	CString sensLabel;
	sensLabel.Format(L"%s (%d):", GetLocalText(ELocText::CameraSens), m_mapping.cameraSensCoeff);
	SetDlgItemTextW(IDC_SENS_TEXT, sensLabel);
}

void CPadDlg::OnBnClickedReset()
{
	if (AfxMessageBox(GetLocalText(ELocText::ResetDefaultQ),
									MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
		return;

	m_mapping.ResetToDefaultButtons();
	InitActionList();
}


void CPadDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	m_isDialogActive = (nState == WA_ACTIVE);
}

void CPadDlg::UpdateAssignBtn(bool value)
{
	if (value)
	{
		m_assignBtn.EnableWindow();
	}
	else
	{
		m_assignBtn.EnableWindow(FALSE);
		m_assignArmed = false;
		m_assignBtn.SetFaceColor(GetSysColor(COLOR_BTNFACE));
	}
}

void CPadDlg::OnBnClickedAssign()
{
	if (m_assignArmed)
	{
		m_assignArmed = false;
		m_assignBtn.SetFaceColor(GetSysColor(COLOR_BTNFACE));
	}
	else
	{
		m_assignArmed = true;
		m_assignBtn.SetFaceColor(RED_COLOR);
	}
}

void CPadDlg::SendAssignButtonMessage(EButton button, float value)
{
	if (!m_assignArmed || abs(value) < 0.2) return;

	SendMessage(MSG_ASSIGN_BTN, NULL, (LPARAM)button);
}

LRESULT CPadDlg::OnAssignButton(__in WPARAM wparam, __in LPARAM lparam)
{
	EButton button = (EButton)lparam;

	CMFCPropertyGridProperty* prop = m_actList.GetCurSel();
	ASSERT(prop);

	auto action = (EAction)prop->GetData();

	if (action == EAction::Move || action == EAction::Camera)
	{
		if (button != EButton::LeftStick && button != EButton::RightStick)
		{
			return(0);
		}
	}

	prop->SetValue(GetButtonName(button));
	OnPropertyChanged(NULL, (LPARAM)prop);
	OnBnClickedAssign();
	return (0);
}

LRESULT CPadDlg::OnSelChanged(__in WPARAM wparam, __in LPARAM lparam)
{
	ASSERT(lparam);

	CMFCPropertyGridProperty* newSelProp = (CMFCPropertyGridProperty*)lparam;

	UpdateGamepadImage((int)newSelProp->GetData());

	if (m_assignArmed)
		OnBnClickedAssign();

	return(0);
}

void CPadDlg::UpdateGamepadImage(int selectedAction)
{
	EButton button = m_mapping.GetActionButton(selectedAction);
	ASSERT((int)button < (int)EButton::MAX);
	
	if (button == EButton::None)
	{
		m_svgView.ClearSelection();
		return;
	}

	LPCTSTR buttSvgName = g_svgButtonId[(int)button];

	m_svgView.SelectShape(buttSvgName, SHAPE_SELECT_COLOR);
}

void CPadDlg::UpdateGamepadImage()
{
	CMFCPropertyGridProperty* selProp = m_actList.GetCurSel();
	if (!selProp) return;

	UpdateGamepadImage((int)selProp->GetData());
}

EButton GetButtonByShapeId(LPCTSTR shapeId)
{
	for (int i = 0; i < (int)EButton::MAX; i++)
	{
		if (_tcscmp(shapeId, g_svgButtonId[i]) == 0)
		{
			return (EButton)i;
		}
	}

	return EButton::None;
}

void CPadDlg::ShapeClicked(const CStringArray& shapeIds)
{
	ASSERT(shapeIds.GetCount() > 0);

	for (int i = 0; i < shapeIds.GetCount(); i++)
	{
		EButton button = GetButtonByShapeId(shapeIds[i]);	
		if (button == EButton::None) continue;

		auto action = (EAction)m_actList.GetCurSel()->GetData();

		if (action == EAction::Move || action == EAction::Camera)
		{
			// RightStick/R3 & LeftStick/L3 shapes are overlapped, requires specific handling

			if (button == EButton::LeftStick || button == EButton::L3)
			{
				AssignButtonBySvgClick(EButton::LeftStick);
			}
			else if (button == EButton::RightStick || button == EButton::R3)
			{
				AssignButtonBySvgClick(EButton::RightStick);
			}

			return;
		}

		// RightStick/R3 & LeftStick/L3 shapes are overlapped, requires specific handling
		if (button == EButton::LeftStick)
			button = EButton::L3;
		else if (button == EButton::RightStick)
			button = EButton::R3;

		AssignButtonBySvgClick(button);
		return;
	}
}

void CPadDlg::AssignButtonBySvgClick(EButton button)
{
	m_assignArmed = true;
	SendAssignButtonMessage(button);
}

void CPadDlg::OnBnClickedMfclink2()
{
	// TODO: Add your control notification handler code here
}


void CPadDlg::OnBnClickedSinglepress()
{
	m_mapping.singlePress = (bool)(m_singlePress.GetCheck() == BST_CHECKED);
}
