// GdiObjectWrapper.h: GDI 对象 RAII 包装类
// 设计模式：RAII + 异常安全
//

#pragma once

#include <afxwin.h>
#include <stdexcept>

// GDI 对象创建失败异常类
class CGdiObjectException : public std::runtime_error
{
public:
	CGdiObjectException(const char* message) : std::runtime_error(message) {}
	CGdiObjectException(const CString& message) : std::runtime_error(CT2A(message)) {}
};

// CPen 的 RAII 包装类
class CPenWrapper
{
private:
	CPen m_pen;
	BOOL m_bCreated;
	CDC* m_pDC;
	CPen* m_pOldPen;

	// 禁止拷贝构造和赋值
	CPenWrapper(const CPenWrapper&) = delete;
	CPenWrapper& operator=(const CPenWrapper&) = delete;

public:
	// 构造函数：创建画笔
	CPenWrapper(int nPenStyle, int nWidth, COLORREF crColor)
		: m_bCreated(FALSE), m_pDC(nullptr), m_pOldPen(nullptr)
	{
		if (!m_pen.CreatePen(nPenStyle, nWidth, crColor))
		{
			throw CGdiObjectException(_T("Failed to create pen"));
		}
		m_bCreated = TRUE;
	}

	// 构造函数：从现有画笔创建
	explicit CPenWrapper(CPen* pPen)
		: m_bCreated(FALSE), m_pDC(nullptr), m_pOldPen(nullptr)
	{
		if (pPen == nullptr)
		{
			throw CGdiObjectException(_T("Pen pointer is null"));
		}
		// 注意：这里只是保存指针，不拥有所有权
		// 如果需要拥有所有权，应该使用 Attach
	}

	// 析构函数：自动清理
	~CPenWrapper()
	{
		Restore();
		if (m_bCreated)
		{
			m_pen.DeleteObject();
		}
	}

	// 选择画笔到设备上下文（自动保存旧画笔）
	CPen* Select(CDC* pDC)
	{
		if (pDC == nullptr)
		{
			throw CGdiObjectException(_T("DC pointer is null"));
		}
		
		// 如果已经选择到其他 DC，先恢复
		if (m_pDC != nullptr && m_pDC != pDC)
		{
			Restore();
		}
		
		if (m_pDC == nullptr)
		{
			m_pDC = pDC;
			m_pOldPen = pDC->SelectObject(&m_pen);
			if (m_pOldPen == nullptr)
			{
				m_pDC = nullptr;
				throw CGdiObjectException(_T("Failed to select pen"));
			}
		}
		
		return m_pOldPen;
	}

	// 恢复到之前的画笔
	void Restore()
	{
		if (m_pDC != nullptr && m_pOldPen != nullptr)
		{
			m_pDC->SelectObject(m_pOldPen);
			m_pDC = nullptr;
			m_pOldPen = nullptr;
		}
	}

	// 获取画笔对象
	CPen* Get() { return &m_pen; }
	const CPen* Get() const { return &m_pen; }

	// 操作符重载：获取画笔对象
	operator CPen*() { return &m_pen; }
	operator const CPen*() const { return &m_pen; }
};

// CBrush 的 RAII 包装类
class CBrushWrapper
{
private:
	CBrush m_brush;
	BOOL m_bCreated;
	CDC* m_pDC;
	CBrush* m_pOldBrush;

	// 禁止拷贝构造和赋值
	CBrushWrapper(const CBrushWrapper&) = delete;
	CBrushWrapper& operator=(const CBrushWrapper&) = delete;

public:
	// 构造函数：创建实心画刷
	explicit CBrushWrapper(COLORREF crColor)
		: m_bCreated(FALSE), m_pDC(nullptr), m_pOldBrush(nullptr)
	{
		if (!m_brush.CreateSolidBrush(crColor))
		{
			throw CGdiObjectException(_T("Failed to create solid brush"));
		}
		m_bCreated = TRUE;
	}

	// 构造函数：创建图案画刷
	explicit CBrushWrapper(CBitmap* pBitmap)
		: m_bCreated(FALSE), m_pDC(nullptr), m_pOldBrush(nullptr)
	{
		if (pBitmap == nullptr)
		{
			throw CGdiObjectException(_T("Bitmap pointer is null"));
		}
		if (!m_brush.CreatePatternBrush(pBitmap))
		{
			throw CGdiObjectException(_T("Failed to create pattern brush"));
		}
		m_bCreated = TRUE;
	}

	// 析构函数：自动清理
	~CBrushWrapper()
	{
		Restore();
		if (m_bCreated)
		{
			m_brush.DeleteObject();
		}
	}

	// 选择画刷到设备上下文（自动保存旧画刷）
	CBrush* Select(CDC* pDC)
	{
		if (pDC == nullptr)
		{
			throw CGdiObjectException(_T("DC pointer is null"));
		}
		
		// 如果已经选择到其他 DC，先恢复
		if (m_pDC != nullptr && m_pDC != pDC)
		{
			Restore();
		}
		
		if (m_pDC == nullptr)
		{
			m_pDC = pDC;
			m_pOldBrush = pDC->SelectObject(&m_brush);
			if (m_pOldBrush == nullptr)
			{
				m_pDC = nullptr;
				throw CGdiObjectException(_T("Failed to select brush"));
			}
		}
		
		return m_pOldBrush;
	}

	// 恢复到之前的画刷
	void Restore()
	{
		if (m_pDC != nullptr && m_pOldBrush != nullptr)
		{
			m_pDC->SelectObject(m_pOldBrush);
			m_pDC = nullptr;
			m_pOldBrush = nullptr;
		}
	}

	// 获取画刷对象
	CBrush* Get() { return &m_brush; }
	const CBrush* Get() const { return &m_brush; }

	// 操作符重载：获取画刷对象
	operator CBrush*() { return &m_brush; }
	operator const CBrush*() const { return &m_brush; }
};

// CFont 的 RAII 包装类
class CFontWrapper
{
private:
	CFont m_font;
	BOOL m_bCreated;
	CDC* m_pDC;
	CFont* m_pOldFont;

	// 禁止拷贝构造和赋值
	CFontWrapper(const CFontWrapper&) = delete;
	CFontWrapper& operator=(const CFontWrapper&) = delete;

public:
	// 构造函数：从 LOGFONT 创建字体
	explicit CFontWrapper(const LOGFONT* lpLogFont)
		: m_bCreated(FALSE), m_pDC(nullptr), m_pOldFont(nullptr)
	{
		if (lpLogFont == nullptr)
		{
			throw CGdiObjectException(_T("LogFont pointer is null"));
		}
		if (!m_font.CreateFontIndirect(lpLogFont))
		{
			throw CGdiObjectException(_T("Failed to create font"));
		}
		m_bCreated = TRUE;
	}

	// 析构函数：自动清理
	~CFontWrapper()
	{
		Restore();
		if (m_bCreated)
		{
			m_font.DeleteObject();
		}
	}

	// 选择字体到设备上下文（自动保存旧字体）
	CFont* Select(CDC* pDC)
	{
		if (pDC == nullptr)
		{
			throw CGdiObjectException(_T("DC pointer is null"));
		}
		
		// 如果已经选择到其他 DC，先恢复
		if (m_pDC != nullptr && m_pDC != pDC)
		{
			Restore();
		}
		
		if (m_pDC == nullptr)
		{
			m_pDC = pDC;
			m_pOldFont = pDC->SelectObject(&m_font);
			if (m_pOldFont == nullptr)
			{
				m_pDC = nullptr;
				throw CGdiObjectException(_T("Failed to select font"));
			}
		}
		
		return m_pOldFont;
	}

	// 恢复到之前的字体
	void Restore()
	{
		if (m_pDC != nullptr && m_pOldFont != nullptr)
		{
			m_pDC->SelectObject(m_pOldFont);
			m_pDC = nullptr;
			m_pOldFont = nullptr;
		}
	}

	// 获取字体对象
	CFont* Get() { return &m_font; }
	const CFont* Get() const { return &m_font; }

	// 操作符重载：获取字体对象
	operator CFont*() { return &m_font; }
	operator const CFont*() const { return &m_font; }
};

// HBITMAP 的 RAII 包装类
class CBitmapWrapper
{
private:
	HBITMAP m_hBitmap;
	BOOL m_bOwned;

	// 禁止拷贝构造和赋值
	CBitmapWrapper(const CBitmapWrapper&) = delete;
	CBitmapWrapper& operator=(const CBitmapWrapper&) = delete;

public:
	// 构造函数：创建位图
	CBitmapWrapper(HDC hDC, int nWidth, int nHeight)
		: m_hBitmap(nullptr), m_bOwned(FALSE)
	{
		if (hDC == nullptr)
		{
			throw CGdiObjectException(_T("DC handle is null"));
		}
		m_hBitmap = CreateCompatibleBitmap(hDC, nWidth, nHeight);
		if (m_hBitmap == nullptr)
		{
			throw CGdiObjectException(_T("Failed to create bitmap"));
		}
		m_bOwned = TRUE;
	}

	// 构造函数：从现有句柄创建（不拥有所有权）
	explicit CBitmapWrapper(HBITMAP hBitmap, BOOL bTakeOwnership = FALSE)
		: m_hBitmap(hBitmap), m_bOwned(bTakeOwnership)
	{
		if (hBitmap == nullptr)
		{
			throw CGdiObjectException(_T("Bitmap handle is null"));
		}
	}

	// 析构函数：自动清理
	~CBitmapWrapper()
	{
		if (m_bOwned && m_hBitmap != nullptr)
		{
			DeleteObject(m_hBitmap);
			m_hBitmap = nullptr;
		}
	}

	// 获取位图句柄
	HBITMAP Get() const { return m_hBitmap; }

	// 操作符重载：获取位图句柄
	operator HBITMAP() const { return m_hBitmap; }

	// 分离所有权（调用后不再自动删除）
	HBITMAP Detach()
	{
		HBITMAP hBitmap = m_hBitmap;
		m_hBitmap = nullptr;
		m_bOwned = FALSE;
		return hBitmap;
	}
};

// HDC 的 RAII 包装类
class CDCWrapper
{
private:
	HDC m_hDC;
	HDC m_hCompatibleDC;
	BOOL m_bOwned;

	// 禁止拷贝构造和赋值
	CDCWrapper(const CDCWrapper&) = delete;
	CDCWrapper& operator=(const CDCWrapper&) = delete;

public:
	// 构造函数：创建兼容 DC
	explicit CDCWrapper(HDC hDC)
		: m_hDC(nullptr), m_hCompatibleDC(hDC), m_bOwned(FALSE)
	{
		if (hDC == nullptr)
		{
			throw CGdiObjectException(_T("DC handle is null"));
		}
		m_hDC = CreateCompatibleDC(hDC);
		if (m_hDC == nullptr)
		{
			throw CGdiObjectException(_T("Failed to create compatible DC"));
		}
		m_bOwned = TRUE;
	}

	// 构造函数：从现有句柄创建（不拥有所有权）
	explicit CDCWrapper(HDC hDC, BOOL bTakeOwnership)
		: m_hDC(hDC), m_hCompatibleDC(nullptr), m_bOwned(bTakeOwnership)
	{
		if (hDC == nullptr)
		{
			throw CGdiObjectException(_T("DC handle is null"));
		}
	}

	// 析构函数：自动清理
	~CDCWrapper()
	{
		if (m_bOwned && m_hDC != nullptr)
		{
			DeleteDC(m_hDC);
			m_hDC = nullptr;
		}
	}

	// 获取 DC 句柄
	HDC Get() const { return m_hDC; }

	// 操作符重载：获取 DC 句柄
	operator HDC() const { return m_hDC; }

	// 分离所有权（调用后不再自动删除）
	HDC Detach()
	{
		HDC hDC = m_hDC;
		m_hDC = nullptr;
		m_bOwned = FALSE;
		return hDC;
	}
};

// 通用的 GDI 对象选择器（用于自动恢复）
template<typename TObject, typename TOldObject>
class CGdiObjectSelector
{
private:
	CDC* m_pDC;
	TOldObject* m_pOldObject;
	BOOL m_bSelected;

	// 禁止拷贝构造和赋值
	CGdiObjectSelector(const CGdiObjectSelector&) = delete;
	CGdiObjectSelector& operator=(const CGdiObjectSelector&) = delete;

public:
	// 构造函数：选择对象到 DC
	CGdiObjectSelector(CDC* pDC, TObject* pObject)
		: m_pDC(pDC), m_pOldObject(nullptr), m_bSelected(FALSE)
	{
		if (pDC == nullptr || pObject == nullptr)
		{
			throw CGdiObjectException(_T("DC or object pointer is null"));
		}
		m_pOldObject = pDC->SelectObject(pObject);
		if (m_pOldObject == nullptr)
		{
			throw CGdiObjectException(_T("Failed to select object"));
		}
		m_bSelected = TRUE;
	}

	// 析构函数：自动恢复
	~CGdiObjectSelector()
	{
		Restore();
	}

	// 恢复到之前的对象
	void Restore()
	{
		if (m_bSelected && m_pDC != nullptr && m_pOldObject != nullptr)
		{
			m_pDC->SelectObject(m_pOldObject);
			m_bSelected = FALSE;
			m_pOldObject = nullptr;
		}
	}

	// 获取旧对象
	TOldObject* GetOldObject() const { return m_pOldObject; }
};

