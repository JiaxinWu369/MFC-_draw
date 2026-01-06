// DrawCommand.h: 绘图命令基类和具体命令类的声明
//

#pragma once

#include <vector>
#include <afxwin.h>

// 绘图数据结构
struct DrawData
{
	enum class DrawType {
		LineSegment, Circle, Rectangle, Ellipse, Pencil, Text, Eraser
	};

	DrawType drawType;
	CPoint pointBegin;
	CPoint pointEnd;
	int penSize;
	COLORREF penColor;
	COLORREF brushColor;
	CString textContent;  // 用于文本输入
	std::vector<CPoint> pencilPoints;  // 用于铅笔和橡皮擦的连续点

	DrawData() : drawType(DrawType::LineSegment), penSize(1), 
		penColor(RGB(0, 0, 0)), brushColor(RGB(0, 0, 0)) {}
};

// 命令基类
class CDrawCommand
{
public:
	virtual ~CDrawCommand() {}
	virtual void Execute(CDC* pDC) = 0;  // 执行命令
	virtual void Undo(CDC* pDC) = 0;     // 撤销命令
	virtual CDrawCommand* Clone() const = 0;  // 克隆命令
};

// 具体命令类：线段
class CLineSegmentCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CLineSegmentCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

// 具体命令类：矩形
class CRectangleCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CRectangleCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

// 具体命令类：圆形
class CCircleCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CCircleCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

// 具体命令类：椭圆
class CEllipseCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CEllipseCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

// 具体命令类：铅笔
class CPencilCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CPencilCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

// 具体命令类：橡皮擦
class CEraserCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CEraserCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

// 具体命令类：文本
class CTextCommand : public CDrawCommand
{
private:
	DrawData m_data;

public:
	CTextCommand(const DrawData& data) : m_data(data) {}
	virtual void Execute(CDC* pDC) override;
	virtual void Undo(CDC* pDC) override;
	virtual CDrawCommand* Clone() const override;
};

