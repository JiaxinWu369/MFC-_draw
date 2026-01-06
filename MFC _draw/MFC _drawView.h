
// MFC _drawView.h: CMFCdrawView 类的接口
//

#pragma once

#include <vector>


class CMFCdrawView : public CView//构造函数实例化时首先调用这个函数
{
protected: // 仅从序列化创建
	CMFCdrawView() noexcept;
	DECLARE_DYNCREATE(CMFCdrawView)

// 特性
public:
	CMFCdrawDoc* GetDocument() const;

// 操作
public:
	int m_PenSize;//画笔
	COLORREF m_PenColor, m_BrushColor;//画笔颜色
	CPoint m_PointBegin, m_PointEnd;//起点和终点

	enum class m_DrawType {
		LineSegment,Circle,Rectangle,Ellipse,Pencil,Text,Eraser
	}m_DrawType;//可以画的图形，线段，圆形，矩形,椭圆  (类的声明)

	int m_TextId;
	CEdit* m_Edit = nullptr;
	CPoint m_TextPos = CPoint(0, 0);
	
	// 用于记录当前操作的临时数据
	std::vector<CPoint> m_currentPencilPoints;  // 铅笔/橡皮擦的连续点
	BOOL m_bDrawing;  // 是否正在绘制
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCdrawView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDrawLineSegment();
	afx_msg void OnDrawRectangle();
	afx_msg void OnDrawCircle();
	afx_msg void On32774();
	afx_msg void OnSetColor();
	afx_msg void OnSetPenSize();
	afx_msg void OnText();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void On32778();
	afx_msg void OnSetPencil();
	afx_msg void OnSetEraser();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnPen();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
#ifdef _DEBUG
	afx_msg LRESULT OnTestGdiWrapper(WPARAM wParam, LPARAM lParam);
#endif
};

#ifndef _DEBUG  // MFC _drawView.cpp 中的调试版本
inline CMFCdrawDoc* CMFCdrawView::GetDocument() const
   { return reinterpret_cast<CMFCdrawDoc*>(m_pDocument); }
#endif

