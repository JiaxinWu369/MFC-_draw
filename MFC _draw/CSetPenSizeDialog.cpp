// CSetPenSizeDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFC _draw.h"
#include "afxdialogex.h"
#include "CSetPenSizeDialog.h"


// CSetPenSizeDialog 对话框

IMPLEMENT_DYNAMIC(CSetPenSizeDialog, CDialogEx)

CSetPenSizeDialog::CSetPenSizeDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CSetPenSizeDialog::~CSetPenSizeDialog()
{
}

void CSetPenSizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT4, m_Edit);

	CString str;
	m_Edit.GetWindowTextW(str);//把输入的值存到str中去
	m_PenSize = _ttoi(str);

}


BEGIN_MESSAGE_MAP(CSetPenSizeDialog, CDialogEx)
END_MESSAGE_MAP()


// CSetPenSizeDialog 消息处理程序
