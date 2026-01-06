// DrawCommand.cpp: 绘图命令类的实现
//

#include "pch.h"
#include "DrawCommand.h"
#include "GdiObjectWrapper.h"

// CLineSegmentCommand 实现
void CLineSegmentCommand::Execute(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		CPenWrapper pen(PS_SOLID, m_data.penSize, m_data.penColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		pDC->MoveTo(m_data.pointBegin);
		pDC->LineTo(m_data.pointEnd);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to execute line segment command\n"));
	}
}

void CLineSegmentCommand::Undo(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		// 使用背景色重绘以擦除
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		pDC->MoveTo(m_data.pointBegin);
		pDC->LineTo(m_data.pointEnd);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to undo line segment command\n"));
	}
}

CDrawCommand* CLineSegmentCommand::Clone() const
{
	return new CLineSegmentCommand(m_data);
}

// CRectangleCommand 实现
void CRectangleCommand::Execute(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		CPenWrapper pen(PS_SOLID, m_data.penSize, m_data.penColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		pDC->SelectStockObject(NULL_BRUSH);
		
		CRect rect(m_data.pointBegin, m_data.pointEnd);
		pDC->Rectangle(rect);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to execute rectangle command\n"));
	}
}

void CRectangleCommand::Undo(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		CBrushWrapper brush(bgColor);
		CGdiObjectSelector<CBrush, CBrush> brushSelector(pDC, brush.Get());
		
		CRect rect(m_data.pointBegin, m_data.pointEnd);
		pDC->Rectangle(rect);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to undo rectangle command\n"));
	}
}

CDrawCommand* CRectangleCommand::Clone() const
{
	return new CRectangleCommand(m_data);
}

// CCircleCommand 实现
void CCircleCommand::Execute(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		CPenWrapper pen(PS_SOLID, m_data.penSize, m_data.penColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		pDC->SelectStockObject(NULL_BRUSH);
		
		CRect rect(m_data.pointBegin, m_data.pointEnd);
		pDC->Ellipse(rect);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to execute circle command\n"));
	}
}

void CCircleCommand::Undo(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		CBrushWrapper brush(bgColor);
		CGdiObjectSelector<CBrush, CBrush> brushSelector(pDC, brush.Get());
		
		CRect rect(m_data.pointBegin, m_data.pointEnd);
		pDC->Ellipse(rect);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to undo circle command\n"));
	}
}

CDrawCommand* CCircleCommand::Clone() const
{
	return new CCircleCommand(m_data);
}

// CEllipseCommand 实现
void CEllipseCommand::Execute(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		CPenWrapper pen(PS_SOLID, m_data.penSize, m_data.penColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		pDC->SelectStockObject(NULL_BRUSH);
		
		CRect rect(m_data.pointBegin, m_data.pointEnd);
		pDC->Ellipse(rect);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to execute ellipse command\n"));
	}
}

void CEllipseCommand::Undo(CDC* pDC)
{
	if (pDC == nullptr)
		return;
	
	try
	{
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		CBrushWrapper brush(bgColor);
		CGdiObjectSelector<CBrush, CBrush> brushSelector(pDC, brush.Get());
		
		CRect rect(m_data.pointBegin, m_data.pointEnd);
		pDC->Ellipse(rect);
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to undo ellipse command\n"));
	}
}

CDrawCommand* CEllipseCommand::Clone() const
{
	return new CEllipseCommand(m_data);
}

// CPencilCommand 实现
void CPencilCommand::Execute(CDC* pDC)
{
	if (pDC == nullptr || m_data.pencilPoints.size() < 2)
		return;
	
	try
	{
		CPenWrapper pen(PS_SOLID, m_data.penSize, m_data.penColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		for (size_t i = 1; i < m_data.pencilPoints.size(); i++)
		{
			pDC->MoveTo(m_data.pencilPoints[i - 1]);
			pDC->LineTo(m_data.pencilPoints[i]);
		}
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to execute pencil command\n"));
	}
}

void CPencilCommand::Undo(CDC* pDC)
{
	if (pDC == nullptr || m_data.pencilPoints.size() < 2)
		return;
	
	try
	{
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		for (size_t i = 1; i < m_data.pencilPoints.size(); i++)
		{
			pDC->MoveTo(m_data.pencilPoints[i - 1]);
			pDC->LineTo(m_data.pencilPoints[i]);
		}
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to undo pencil command\n"));
	}
}

CDrawCommand* CPencilCommand::Clone() const
{
	return new CPencilCommand(m_data);
}

// CEraserCommand 实现
void CEraserCommand::Execute(CDC* pDC)
{
	if (pDC == nullptr || m_data.pencilPoints.size() < 2)
		return;
	
	try
	{
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		for (size_t i = 1; i < m_data.pencilPoints.size(); i++)
		{
			pDC->MoveTo(m_data.pencilPoints[i - 1]);
			pDC->LineTo(m_data.pencilPoints[i]);
		}
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to execute eraser command\n"));
	}
}

void CEraserCommand::Undo(CDC* pDC)
{
	// 橡皮擦的撤销需要恢复被擦除的内容
	// 这里简化处理，实际应该保存被擦除的内容
	// 暂时使用背景色重绘
	if (pDC == nullptr || m_data.pencilPoints.size() < 2)
		return;
	
	try
	{
		COLORREF bgColor = pDC->GetBkColor();
		CPenWrapper pen(PS_SOLID, m_data.penSize, bgColor);
		CGdiObjectSelector<CPen, CPen> penSelector(pDC, pen.Get());
		
		for (size_t i = 1; i < m_data.pencilPoints.size(); i++)
		{
			pDC->MoveTo(m_data.pencilPoints[i - 1]);
			pDC->LineTo(m_data.pencilPoints[i]);
		}
	}
	catch (const CGdiObjectException&)
	{
		// 错误处理：记录日志或显示错误消息
		TRACE(_T("Failed to undo eraser command\n"));
	}
}

CDrawCommand* CEraserCommand::Clone() const
{
	return new CEraserCommand(m_data);
}

// CTextCommand 实现
void CTextCommand::Execute(CDC* pDC)
{
	pDC->SetTextColor(m_data.penColor);
	pDC->TextOutW(m_data.pointBegin.x, m_data.pointBegin.y, m_data.textContent);
}

void CTextCommand::Undo(CDC* pDC)
{
	// 使用背景色重绘文本区域以擦除
	COLORREF bgColor = pDC->GetBkColor();
	pDC->SetTextColor(bgColor);
	pDC->TextOutW(m_data.pointBegin.x, m_data.pointBegin.y, m_data.textContent);
}

CDrawCommand* CTextCommand::Clone() const
{
	return new CTextCommand(m_data);
}

