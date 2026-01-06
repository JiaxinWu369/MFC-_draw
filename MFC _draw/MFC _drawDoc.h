
// MFC _drawDoc.h: CMFCdrawDoc 类的接口
//


#pragma once

#include <stack>
#include "DrawCommand.h"

class CMFCdrawDoc : public CDocument
{
protected: // 仅从序列化创建
	CMFCdrawDoc() noexcept;
	DECLARE_DYNCREATE(CMFCdrawDoc)

// 特性
public:

// 操作
public:
	// 添加命令到撤销栈
	void AddCommand(CDrawCommand* pCommand);
	// 撤销操作
	BOOL Undo();
	// 重做操作
	BOOL Redo();
	// 检查是否可以撤销
	BOOL CanUndo() const { return !m_undoStack.empty(); }
	// 检查是否可以重做
	BOOL CanRedo() const { return !m_redoStack.empty(); }
	// 清除所有命令（新建文档时）
	void ClearCommands();
	// 重绘所有命令
	void RedrawAll(CDC* pDC);

private:
	std::stack<CDrawCommand*> m_undoStack;  // 撤销栈
	std::stack<CDrawCommand*> m_redoStack;  // 重做栈
	std::vector<CDrawCommand*> m_allCommands;  // 存储所有命令用于重绘

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CMFCdrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
