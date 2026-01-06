
// MFC _drawView.cpp: CMFCdrawView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFC _draw.h"
#endif

#include "MFC _drawDoc.h"
#include "MFC _drawView.h"
#include "resource.h"
#include "CSetPenSizeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCdrawView

IMPLEMENT_DYNCREATE(CMFCdrawView, CView)

BEGIN_MESSAGE_MAP(CMFCdrawView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_32771, &CMFCdrawView::OnDrawLineSegment)
	ON_COMMAND(ID_32772, &CMFCdrawView::OnDrawRectangle)
	ON_COMMAND(ID_32773, &CMFCdrawView::OnDrawCircle)
	ON_COMMAND(ID_32774, &CMFCdrawView::On32774)
	ON_COMMAND(ID_32776, &CMFCdrawView::OnSetColor)
	ON_COMMAND(ID_32775, &CMFCdrawView::OnSetPenSize)
	ON_COMMAND(ID_32777, &CMFCdrawView::OnText)

	ON_COMMAND(ID_32778, &CMFCdrawView::OnSetPencil)
	ON_COMMAND(ID_32779, &CMFCdrawView::OnSetEraser)
	ON_COMMAND(ID_FILE_OPEN, &CMFCdrawView::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CMFCdrawView::OnFileSave)
	ON_COMMAND(ID_32780, &CMFCdrawView::OnPen)
	ON_COMMAND(ID_EDIT_UNDO, &CMFCdrawView::OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMFCdrawView::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMFCdrawView::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMFCdrawView::OnUpdateEditRedo)
END_MESSAGE_MAP()

// CMFCdrawView 构造/析构

CMFCdrawView::CMFCdrawView() noexcept
{
	// TODO: 在此处添加构造代码
	 m_PenSize = 1;//画笔
	 m_PenColor = RGB(0, 0, 0);
	 m_BrushColor = RGB(0, 0, 0);//画笔颜色
	 m_PointBegin = CPoint(0, 0);
      m_PointEnd =  CPoint(0, 0);//起点和终点
	  m_DrawType = m_DrawType::LineSegment;//初始值为线段
	  m_TextId = 10086;//文本输入id
	  m_bDrawing = FALSE;
}

CMFCdrawView::~CMFCdrawView()
{
}

BOOL CMFCdrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCdrawView 绘图

void CMFCdrawView::OnDraw(CDC* pDC)
{
	CMFCdrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// 重绘所有命令
	pDoc->RedrawAll(pDC);
}


// CMFCdrawView 打印

BOOL CMFCdrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCdrawView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCdrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCdrawView 诊断

#ifdef _DEBUG
void CMFCdrawView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCdrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCdrawDoc* CMFCdrawView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCdrawDoc)));
	return (CMFCdrawDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCdrawView 消息处理程序


void CMFCdrawView::OnLButtonDown(UINT nFlags, CPoint point)//鼠标消息 按动
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_PointBegin = m_PointEnd = point;//初始化
	m_bDrawing = TRUE;
	
	// 对于铅笔和橡皮擦，记录起始点
	if (m_DrawType == m_DrawType::Pencil || m_DrawType == m_DrawType::Eraser)
	{
		m_currentPencilPoints.clear();
		m_currentPencilPoints.push_back(point);
	}
	
	CView::OnLButtonDown(nFlags, point);
}


void CMFCdrawView::OnMouseMove(UINT nFlags, CPoint point)//鼠标移动
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_LBUTTON) {
		CClientDC dc(this);//获取当前数据,鼠标左键是否被按下
		CPen newPen, * oldPen;
	
		newPen.CreatePen(PS_SOLID,m_PenSize,m_PenColor);//创建个新笔，设置画笔尺寸，颜色等
		oldPen = dc.SelectObject(&newPen);


		switch (m_DrawType){
		case m_DrawType::LineSegment://画直线
		{
			dc.SetROP2(R2_NOTXORPEN);//设置DC的绘图模式，如果已经画了一条线，再画一条线，覆盖掉上一条线，以达到线段移动的效果
			dc.MoveTo(m_PointBegin);
			dc.LineTo(m_PointEnd);

			dc.MoveTo(m_PointBegin);
			dc.LineTo(point);

			m_PointEnd = point;
			break;
		}
		case m_DrawType::Rectangle://画矩形
		{
			dc.SetROP2(R2_NOTXORPEN);
			dc.SelectStockObject(5);
			CRect rectP1(m_PointBegin, m_PointEnd);

			dc.Rectangle(rectP1);

			CRect rectP2(m_PointBegin, point);
			dc.Rectangle(rectP2);
			m_PointEnd = point;
			break;
		}
		case m_DrawType::Ellipse://画椭圆
		{
			dc.SetROP2(R2_NOTXORPEN);
			dc.SelectStockObject(5);
			CRect rectP1(m_PointBegin, m_PointEnd);

			dc.Ellipse(rectP1);

			CRect rectP2(m_PointBegin, point);
			dc.Ellipse(rectP2);
			m_PointEnd = point;
			break;
		}
		case m_DrawType::Circle://画圆形
		{
			dc.SetROP2(R2_NOTXORPEN);
			dc.SelectStockObject(5);

			int length_1 = m_PointEnd.y - m_PointBegin.y;//确定正方形的边长
			if (m_PointEnd.x < m_PointBegin.x) {
				m_PointEnd.x = m_PointBegin.x - abs(length_1);//对length1_1取绝对值

			}
			else {
				m_PointEnd.x = m_PointBegin.x + abs(length_1);
			}

			CRect rectP1(m_PointBegin, m_PointEnd);//确定正方形

			dc.Ellipse(rectP1);//用正方形绘制椭圆
			int length_2 = point.y - m_PointBegin.y;//point为当前点
			if (point.x < m_PointBegin.x) {
				m_PointEnd.x = m_PointBegin.x - abs(length_2);//对length1_1取绝对值

			}//用现在当前点和现在的起点的横坐标来确定终点的横坐标
			else {
				m_PointEnd.x = m_PointBegin.x + abs(length_2);
			}
			m_PointEnd.y = point.y;//用纵坐标的差来绘制圆，所以纵坐标不用变

			CRect rectP2(m_PointBegin, m_PointEnd);
			dc.Ellipse(rectP2);
			m_PointEnd = point;
			break;
		}
		case m_DrawType::Text://文本输入
		{
			if (m_Edit != nullptr) {
				delete m_Edit;
			}
			CEdit* myEdit = new CEdit();//新建一个Edit对象
			myEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(m_PointBegin, point), this, m_TextId);

			m_Edit = myEdit;
			myEdit->ShowWindow(SW_SHOW);
			break;
		}
		case m_DrawType::Pencil: //创建铅笔
		{
			m_PointBegin = m_PointEnd;
			m_PointEnd = point;
			dc.MoveTo(m_PointBegin);
			dc.LineTo(m_PointEnd);
			
			// 记录点
			if (m_currentPencilPoints.empty() || m_currentPencilPoints.back() != point)
			{
				m_currentPencilPoints.push_back(point);
			}

			break;
		}
		case m_DrawType::Eraser://创建橡皮
		{
			COLORREF pColor = dc.GetBkColor();//颜色设置成背景颜色
			CPen newPen(PS_SOLID,m_PenSize,pColor);// 创建一个与背景颜色相同的画笔
			dc.SelectObject(&newPen);//选择这个画笔到设备上下文中
			m_PointBegin = m_PointEnd;
			m_PointEnd = point;//更新起点和终点
			dc.MoveTo(m_PointBegin);
			dc.LineTo(m_PointEnd);//移动到起点，然后画到终点
			
			// 记录点
			if (m_currentPencilPoints.empty() || m_currentPencilPoints.back() != point)
			{
				m_currentPencilPoints.push_back(point);
			}

			break;
		}
		default:

			break;
		}
		dc.SelectObject(&oldPen);                                                         
	}
	CView::OnMouseMove(nFlags, point);
}


void CMFCdrawView::OnLButtonUp(UINT nFlags, CPoint point)//使相交的点不再是白色
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_bDrawing)
	{
		CView::OnLButtonUp(nFlags, point);
		return;
	}
	
	CClientDC dc(this);
	CPen newPen, * oldPen;
	CMFCdrawDoc* pDoc = GetDocument();
	CDrawCommand* pCommand = nullptr;

	newPen.CreatePen(PS_SOLID, m_PenSize, m_PenColor);//创建个新笔，设置画笔尺寸，颜色等
	oldPen = dc.SelectObject(&newPen);
	
	DrawData data;
	data.penSize = m_PenSize;
	data.penColor = m_PenColor;
	data.brushColor = m_BrushColor;
	data.pointBegin = m_PointBegin;
	
	switch (m_DrawType) {
	case m_DrawType::LineSegment://画直线
	{
		dc.MoveTo(m_PointBegin);
		dc.LineTo(point);
		m_PointEnd = point;
		data.pointEnd = point;
		data.drawType = DrawData::DrawType::LineSegment;
		pCommand = new CLineSegmentCommand(data);
		break;
	}
	case m_DrawType::Rectangle://画矩形
	{
		dc.SelectStockObject(5);
		CRect rectP2(m_PointBegin, point);
		dc.Rectangle(rectP2);
		data.pointEnd = point;
		data.drawType = DrawData::DrawType::Rectangle;
		pCommand = new CRectangleCommand(data);
		break;
	}
	case m_DrawType::Ellipse://画椭圆
	{
		dc.SelectStockObject(5);
		CRect rectP2(m_PointBegin, point);
		dc.Ellipse(rectP2);
		data.pointEnd = point;
		data.drawType = DrawData::DrawType::Ellipse;
		pCommand = new CEllipseCommand(data);
		break;
	}
	case m_DrawType::Circle://画圆形
	{
		dc.SelectStockObject(5);

		int length_2 = point.y - m_PointBegin.y;//point为当前点
		if (point.x < m_PointBegin.x) {
			m_PointEnd.x = m_PointBegin.x - abs(length_2);//对length1_1取绝对值

		}//用现在当前点和现在的起点的横坐标来确定终点的横坐标
		else {
			m_PointEnd.x = m_PointBegin.x + abs(length_2);
		}
		m_PointEnd.y = point.y;//用纵坐标的差来绘制圆，所以纵坐标不用变

		CRect rectP2(m_PointBegin, m_PointEnd);
		dc.Ellipse(rectP2);
		m_PointEnd = point;
		data.pointEnd = m_PointEnd;
		data.drawType = DrawData::DrawType::Circle;
		pCommand = new CCircleCommand(data);
		break;
	}
	case m_DrawType::Text:
	{
		CEdit* myEdit = new CEdit();//新建一个Edit对象
		myEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(m_PointBegin, point), this, m_TextId);
		myEdit->ShowWindow(SW_SHOW);

		if (nullptr != m_Edit) {
			delete m_Edit;
		}
		m_Edit = myEdit;
		m_TextPos = m_PointBegin;
		// 文本命令将在PreTranslateMessage中创建
		break;

	}
	case m_DrawType::Pencil: //创建铅笔
	{
		if (m_currentPencilPoints.size() > 1)
		{
			data.pencilPoints = m_currentPencilPoints;
			data.drawType = DrawData::DrawType::Pencil;
			pCommand = new CPencilCommand(data);
		}
		break;
	}
	case m_DrawType::Eraser://创建橡皮
	{
		if (m_currentPencilPoints.size() > 1)
		{
			data.pencilPoints = m_currentPencilPoints;
			data.drawType = DrawData::DrawType::Eraser;
			pCommand = new CEraserCommand(data);
		}
		break;
	}
	   default:

		break;
	}
	
	dc.SelectObject(oldPen);
	
	// 添加命令到文档
	if (pCommand != nullptr && pDoc != nullptr)
	{
		pDoc->AddCommand(pCommand);
	}
	
	m_bDrawing = FALSE;
	m_currentPencilPoints.clear();
	
	CView::OnLButtonUp(nFlags, point);
}


void CMFCdrawView::OnDrawLineSegment()
{
	m_DrawType = m_DrawType::LineSegment;
	// TODO: 在此添加命令处理程序代码
}


void CMFCdrawView::OnDrawRectangle()
{
	m_DrawType = m_DrawType::Rectangle;
	// TODO: 在此添加命令处理程序代码
}


void CMFCdrawView::OnDrawCircle()
{
	m_DrawType = m_DrawType::Circle;
	// TODO: 在此添加命令处理程序代码
}


void CMFCdrawView::On32774()
{
	m_DrawType = m_DrawType::Ellipse;
	// TODO: 在此添加命令处理程序代码
}


void CMFCdrawView::OnSetColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog dlg;
	if (IDOK == dlg.DoModal())//显示颜色对话框
	{
		m_PenColor = dlg.GetColor();//将颜色存到pencolor中
	
	
	}
}


void CMFCdrawView::OnSetPenSize()
{
	// TODO: 在此添加命令处理程序代码
	CSetPenSizeDialog dlg;
	if (IDOK == dlg.DoModal()) {
		this->m_PenSize = dlg.m_PenSize;
	
	}
}


void CMFCdrawView::OnText()
{
	m_DrawType = m_DrawType::Text;
	// TODO: 在此添加命令处理程序代码
}


BOOL CMFCdrawView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (m_DrawType == m_DrawType::Text && m_Edit != nullptr)
		{
			//CEdit* pEdit = mEdits.back();
			CString pStr;
			m_Edit->GetWindowTextW(pStr);
			assert(m_Edit != nullptr);
			
			CClientDC dc(this);
			dc.TextOutW(m_TextPos.x, m_TextPos.y, pStr);
			
			// 创建文本命令
			DrawData data;
			data.drawType = DrawData::DrawType::Text;
			data.pointBegin = m_TextPos;
			data.textContent = pStr;
			data.penColor = m_PenColor;
			data.penSize = m_PenSize;
			
			CMFCdrawDoc* pDoc = GetDocument();
			if (pDoc != nullptr)
			{
				CDrawCommand* pCommand = new CTextCommand(data);
				pDoc->AddCommand(pCommand);
			}
			
			delete m_Edit;
			m_Edit = nullptr;

			return TRUE;
		}

		
	}


	return CView::PreTranslateMessage(pMsg);
}





void CMFCdrawView::OnSetPencil()
{
	// TODO: 在此添加命令处理程序代码
	m_DrawType = m_DrawType::Pencil;
}


void CMFCdrawView::OnSetEraser()
{
	// TODO: 在此添加命令处理程序代码
	m_DrawType = m_DrawType::Eraser;
}


void CMFCdrawView::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	CString filter, strPath;

	filter = "bmp图片(*.bmp)|*.bmp||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	//打开文件对应的名
	if (dlg.DoModal() == IDOK)
	{

		strPath = dlg.GetPathName();//检查是否获取文件路径

	}

	HBITMAP hBitmap = (HBITMAP)::LoadImage(
		NULL,
		strPath,
		IMAGE_BITMAP,
		0, 0,
		LR_LOADFROMFILE);
	//加载本地图片内容，按照实际尺寸加载

	CBitmap loadImageBitmap;
	//声明对象处理加载的位图

	loadImageBitmap.Attach(hBitmap);
	//绑定hBitmap到声明的对象上

	BITMAP bitMap;
	//定义一个位图结构体，获得图片的信息
	loadImageBitmap.GetBitmap(&bitMap);
	//利用传递参数bitMap,获得加载图片的信息

	CBrush newBrush, * oldBrush;
	//画刷和旧画笔的指针
	newBrush.CreatePatternBrush(&loadImageBitmap);
	//创建位图画刷，newbrush存储了位图的信息，相当于加载的位图创建图案刷

	CClientDC dc(this);
	//选取这个窗口
	oldBrush = (CBrush*)dc.SelectObject(&newBrush);
	//选择新画刷进行绘制，并保存旧的画笔
	dc.Rectangle(0, 0, bitMap.bmWidth, bitMap.bmHeight);
	//使用加载位图的尺寸在设备上绘制矩形
	dc.SelectObject(oldBrush);
	//画完以后把笔返回

}


void CMFCdrawView::OnFileSave()
{
	// TODO: 在此添加命令处理程序代码
	CClientDC dc(this);
	CRect rect;


	BOOL  showMsgTag;
	//是否要弹出”图像保存成功对话框" 

	GetClientRect(&rect);                  //获取窗口区域大小    
	HBITMAP hbitmap = CreateCompatibleBitmap(dc, rect.right - rect.left, rect.bottom - rect.top);
	//创建与设备环境兼容位图  

	HDC hdc = CreateCompatibleDC(dc);      //创建兼容DC，以便将图像保存为不同的格式   

	HBITMAP hOldMap = (HBITMAP)SelectObject(hdc, hbitmap);  //将位图选入DC，并保存返回值   

	BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, dc, 0, 0, SRCCOPY);
	//将屏幕DC的图像复制到内存DC中   

	CImage image;
	image.Attach(hbitmap);


	//将位图转化为一般图像     




	showMsgTag = TRUE;
	CString  strFilter = _T("位图文件(*.bmp)|*.bmp|JPEG 图像文件|*.jpg|GIF图像文件 | *.gif | PNG图像文件 | *.png | 其他格式 * .*) | *.* || ");

	CFileDialog dlg(FALSE, _T("bmp"), _T("iPaint1.bmp"), NULL, strFilter);//创建文件对话框

	if (dlg.DoModal() != IDOK)//如果用户没有选择，则返回
		return;
	CString strFileName;          //如果用户没有指定文件扩展名，则为其添加一个  

	CString strExtension;




	strFileName = dlg.m_ofn.lpstrFile;

	if (dlg.m_ofn.nFileExtension = 0)               //扩展名项目为0    
	{
		switch (dlg.m_ofn.nFilterIndex)// 根据过滤器索引确定文件类型
		{
		case 1:
			strExtension = "bmp"; break;
		case 2:
			strExtension = "jpg"; break;
		case 3:
			strExtension = "gif"; break;
		case 4:
			strExtension = "png"; break;
		default:
			break;
		}
		strFileName = strFileName + "." + strExtension;// 添加正确的文件扩展名
	}
	CString saveFilePath = strFileName;     // 保存文件路径



	HRESULT hResult = image.Save(saveFilePath);     //保存图像    
	if (FAILED(hResult))
	{
		MessageBox(_T("保存图像文件失败！"));
	}
	else
	{
		if (showMsgTag)
			MessageBox(_T("文件保存成功！"));
	}
	image.Detach();//从Cimage对象分离位图
	SelectObject(hdc, hOldMap);//恢复原来的位图对象


}


void CMFCdrawView::OnPen()
{
	// TODO: 在此添加命令处理程序代码
}

void CMFCdrawView::OnEditUndo()
{
	CMFCdrawDoc* pDoc = GetDocument();
	if (pDoc == nullptr) return;
	
	if (pDoc->Undo())
	{
		Invalidate();  // 重绘视图
	}
}

void CMFCdrawView::OnEditRedo()
{
	CMFCdrawDoc* pDoc = GetDocument();
	if (pDoc == nullptr) return;
	
	if (pDoc->Redo())
	{
		Invalidate();  // 重绘视图
	}
}

void CMFCdrawView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CMFCdrawDoc* pDoc = GetDocument();
	if (pDoc != nullptr)
	{
		pCmdUI->Enable(pDoc->CanUndo());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMFCdrawView::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CMFCdrawDoc* pDoc = GetDocument();
	if (pDoc != nullptr)
	{
		pCmdUI->Enable(pDoc->CanRedo());
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
