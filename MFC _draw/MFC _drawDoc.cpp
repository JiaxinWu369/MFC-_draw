
// MFC _drawDoc.cpp: CMFCdrawDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFC _draw.h"
#endif

#include "MFC _drawDoc.h"
#include "DrawCommand.h"
#include "GdiObjectWrapper.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMFCdrawDoc

IMPLEMENT_DYNCREATE(CMFCdrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFCdrawDoc, CDocument)
END_MESSAGE_MAP()


CMFCdrawDoc::CMFCdrawDoc() noexcept
{
	// TODO: 在此添加一次性构造代码

}

void CMFCdrawDoc::AddCommand(CDrawCommand* pCommand)
{
	if (pCommand == nullptr) return;
	
	// 清除重做栈（新操作后不能再重做之前的操作）
	while (!m_redoStack.empty())
	{
		delete m_redoStack.top();
		m_redoStack.pop();
	}
	
	// 添加到撤销栈和所有命令列表
	m_undoStack.push(pCommand);
	m_allCommands.push_back(pCommand);
	
	// 标记文档已修改
	SetModifiedFlag(TRUE);
}

BOOL CMFCdrawDoc::Undo()
{
	if (m_undoStack.empty())
		return FALSE;
	
	CDrawCommand* pCommand = m_undoStack.top();
	m_undoStack.pop();
	
	// 从所有命令列表中移除
	auto it = std::find(m_allCommands.begin(), m_allCommands.end(), pCommand);
	if (it != m_allCommands.end())
	{
		m_allCommands.erase(it);
	}
	
	// 移到重做栈
	m_redoStack.push(pCommand);
	
	SetModifiedFlag(TRUE);
	return TRUE;
}

BOOL CMFCdrawDoc::Redo()
{
	if (m_redoStack.empty())
		return FALSE;
	
	CDrawCommand* pCommand = m_redoStack.top();
	m_redoStack.pop();
	
	// 移回撤销栈和所有命令列表
	m_undoStack.push(pCommand);
	m_allCommands.push_back(pCommand);
	
	SetModifiedFlag(TRUE);
	return TRUE;
}

void CMFCdrawDoc::ClearCommands()
{
	// 清除撤销栈
	while (!m_undoStack.empty())
	{
		delete m_undoStack.top();
		m_undoStack.pop();
	}
	
	// 清除重做栈
	while (!m_redoStack.empty())
	{
		delete m_redoStack.top();
		m_redoStack.pop();
	}
	
	// 清除所有命令
	for (auto* cmd : m_allCommands)
	{
		delete cmd;
	}
	m_allCommands.clear();
}

void CMFCdrawDoc::RedrawAll(CDC* pDC)
{
	for (auto* cmd : m_allCommands)
	{
		cmd->Execute(pDC);
	}
}

CMFCdrawDoc::~CMFCdrawDoc()
{
	ClearCommands();
}

BOOL CMFCdrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	ClearCommands();

	return TRUE;
}




// CMFCdrawDoc 序列化

void CMFCdrawDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMFCdrawDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	try
	{
		CString strText = _T("TODO: implement thumbnail drawing here");
		LOGFONT lf;

		CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		if (pDefaultGUIFont != nullptr)
		{
			pDefaultGUIFont->GetLogFont(&lf);
		}
		else
		{
			// 如果无法获取默认字体，使用系统默认值
			ZeroMemory(&lf, sizeof(LOGFONT));
			lf.lfHeight = 36;
		}
		lf.lfHeight = 36;

		CFontWrapper fontDraw(&lf);
		CGdiObjectSelector<CFont, CFont> fontSelector(&dc, fontDraw.Get());
		
		dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to draw thumbnail\n"));
	}
}

// 搜索处理程序的支持
void CMFCdrawDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMFCdrawDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMFCdrawDoc 诊断

#ifdef _DEBUG
void CMFCdrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFCdrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFCdrawDoc 命令
