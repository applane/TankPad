#pragma once
#include "GPadImpl.h"
#include "Mapping.h"
#include "SVGControl.h"

const COLORREF RED_COLOR = RGB(255, 138, 138);
const COLORREF SHAPE_SELECT_COLOR = RGBA(255, 138, 138, 255);

#define MSG_ASSIGN_BTN			WM_USER+100
#define MSG_PROP_SEL_CHANGED	WM_USER+101

class CPadPropertyGridCtrl : public CMFCPropertyGridCtrl
{
public:
	CPadPropertyGridCtrl() : CMFCPropertyGridCtrl(){}
	void OnChangeSelection(CMFCPropertyGridProperty* pNewSel, CMFCPropertyGridProperty* pOldSel) override
	{
		GetParent()->SendMessage(MSG_PROP_SEL_CHANGED, NULL, (LPARAM)pNewSel);
	}
};

class CPadDlg : public CDialogEx, public IGamepadCallback, public ISVGControlNotify
{
// Construction
public:
	CPadDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WOTPAD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	afx_msg LRESULT OnAssignButton(__in WPARAM wparam, __in LPARAM lparam);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnPropertyChanged(__in WPARAM wparam, __in LPARAM lparam);
	afx_msg LRESULT OnSelChanged(__in WPARAM wparam, __in LPARAM lparam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_padName;

	// IGamepadCallback
	void DeviceAttached(SGamepadData& device) override;
	void DeviceRemoved() override;
	bool NeedDetectDevice() override;
	void ButtonDown(EButton buttonID) override;
	void ButtonUp(EButton buttonID) override;
	void LeftStick(EAxis axis, float value) override;
	void RightStick(EAxis axis, float value) override;

	// ISVGControlNotify
	void ISVGControlNotify::ShapeClicked(const CStringArray& shapeIds) override;
protected:
	void OnCancel() override;
	void OnOK() override;
	void AddButtonNames(CMFCPropertyGridProperty* prop);
	void InitControls();
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void UpdateSensText();
	void SendAssignButtonMessage(EButton button, float value = 1.0);
	inline bool IsAssignButtonMode() { return m_assignArmed;  }
	void UpdateAssignBtn(bool value);
	void UpdateGamepadImage(int selectedAction);
	void UpdateGamepadImage();
	void AssignButtonBySvgClick(EButton button);
private:
	bool m_isDialogActive = false;
	bool m_assignArmed = false;
	Mapping m_mapping;
	void InitActionList();
	CPadPropertyGridCtrl m_actList;
	CSVGControl m_svgView;
	CStyledFont m_boldFont;
	CStyledFont m_listFont;
public:
	CButton m_invertCamY;
	CButton m_singlePress;
	CMFCButton m_assignBtn;
	afx_msg void OnBnClickedInverty();
	CSliderCtrl m_senSlider;
	afx_msg void OnBnClickedReset();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnBnClickedAssign();
	afx_msg void OnBnClickedMfclink2();
	afx_msg void OnBnClickedSinglepress();
};
