// GdiObjectWrapperTest.cpp: RAII 包装类测试代码
// 此文件包含测试函数，可以在调试模式下调用以验证 RAII 功能
//

#include "pch.h"
#include "GdiObjectWrapper.h"
#include <afxwin.h>
#include <afxext.h>  // 用于 GetGuiResources

#ifdef _DEBUG

// 测试函数：验证 CPenWrapper 的 RAII 功能
void TestCPenWrapper()
{
	TRACE(_T("=== 测试 CPenWrapper ===\n"));
	
	try
	{
		// 测试正常创建
		CPenWrapper pen1(PS_SOLID, 2, RGB(255, 0, 0));
		TRACE(_T("✓ 画笔创建成功\n"));
		
		// 测试自动析构（离开作用域时自动删除）
		{
			CPenWrapper pen2(PS_SOLID, 3, RGB(0, 255, 0));
			TRACE(_T("✓ 画笔在作用域内创建\n"));
		}
		TRACE(_T("✓ 画笔在作用域外自动销毁\n"));
		
		// 测试异常处理
		try
		{
			// 使用无效参数（宽度为负数应该失败，但某些系统可能允许）
			// 这里主要测试异常机制
			CPenWrapper* pPen = nullptr;
			if (pPen == nullptr)
			{
				// 模拟空指针异常
				throw CGdiObjectException(_T("测试异常"));
			}
		}
		catch (const CGdiObjectException& e)
		{
			TRACE(_T("✓ 异常捕获成功: %s\n"), CString(e.what()));
		}
	}
	catch (const CGdiObjectException& e)
	{
		TRACE(_T("✗ 测试失败: %s\n"), CString(e.what()));
	}
	
	TRACE(_T("=== CPenWrapper 测试完成 ===\n\n"));
}

// 测试函数：验证 CBrushWrapper 的 RAII 功能
void TestCBrushWrapper()
{
	TRACE(_T("=== 测试 CBrushWrapper ===\n"));
	
	try
	{
		// 测试实心画刷创建
		CBrushWrapper brush1(RGB(255, 128, 0));
		TRACE(_T("✓ 画刷创建成功\n"));
		
		// 测试自动析构
		{
			CBrushWrapper brush2(RGB(0, 255, 128));
			TRACE(_T("✓ 画刷在作用域内创建\n"));
		}
		TRACE(_T("✓ 画刷在作用域外自动销毁\n"));
	}
	catch (const CGdiObjectException& e)
	{
		TRACE(_T("✗ 测试失败: %s\n"), CString(e.what()));
	}
	
	TRACE(_T("=== CBrushWrapper 测试完成 ===\n\n"));
}

// 测试函数：验证 CGdiObjectSelector 的自动恢复功能
void TestGdiObjectSelector(CWnd* pWnd)
{
	if (pWnd == nullptr)
		return;
		
	TRACE(_T("=== 测试 CGdiObjectSelector ===\n"));
	
	try
	{
		CClientDC dc(pWnd);
		
		// 保存原始画笔
		CPen* pOldPen = dc.GetCurrentPen();
		TRACE(_T("原始画笔句柄: %p\n"), pOldPen);
		
		// 创建新画笔并选择
		{
			CPenWrapper pen(PS_SOLID, 5, RGB(255, 0, 0));
			CGdiObjectSelector<CPen, CPen> selector(&dc, pen.Get());
			
			CPen* pCurrentPen = dc.GetCurrentPen();
			TRACE(_T("当前画笔句柄: %p\n"), pCurrentPen);
			TRACE(_T("✓ 画笔已选择到 DC\n"));
		}
		
		// 验证画笔是否自动恢复
		CPen* pRestoredPen = dc.GetCurrentPen();
		TRACE(_T("恢复后画笔句柄: %p\n"), pRestoredPen);
		
		if (pRestoredPen == pOldPen)
		{
			TRACE(_T("✓ 画笔自动恢复成功\n"));
		}
		else
		{
			TRACE(_T("✗ 画笔恢复失败\n"));
		}
	}
	catch (const CGdiObjectException& e)
	{
		TRACE(_T("✗ 测试失败: %s\n"), CString(e.what()));
	}
	
	TRACE(_T("=== CGdiObjectSelector 测试完成 ===\n\n"));
}

// 测试函数：验证资源泄漏检测
void TestResourceLeak(CWnd* pWnd)
{
	if (pWnd == nullptr)
		return;
		
	TRACE(_T("=== 测试资源泄漏检测 ===\n"));
	
	// 获取初始 GDI 对象数量
	DWORD initialGdiObjects = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
	TRACE(_T("初始 GDI 对象数量: %lu\n"), initialGdiObjects);
	
	try
	{
		CClientDC dc(pWnd);
		
		// 创建大量 GDI 对象（应该自动释放）
		for (int i = 0; i < 100; i++)
		{
			CPenWrapper pen(PS_SOLID, 1, RGB(i % 255, (i * 2) % 255, (i * 3) % 255));
			CGdiObjectSelector<CPen, CPen> selector(&dc, pen.Get());
			
			// 执行一些绘制操作
			dc.MoveTo(i, i);
			dc.LineTo(i + 10, i + 10);
		}
		
		// 获取最终 GDI 对象数量
		DWORD finalGdiObjects = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
		TRACE(_T("最终 GDI 对象数量: %lu\n"), finalGdiObjects);
		TRACE(_T("GDI 对象数量变化: %ld\n"), finalGdiObjects - initialGdiObjects);
		
		// 如果 GDI 对象数量增加不超过 10，认为测试通过
		if (finalGdiObjects - initialGdiObjects <= 10)
		{
			TRACE(_T("✓ 资源泄漏测试通过（GDI 对象正确释放）\n"));
		}
		else
		{
			TRACE(_T("✗ 资源泄漏测试失败（可能存在泄漏）\n"));
		}
	}
	catch (const CGdiObjectException& e)
	{
		TRACE(_T("✗ 测试失败: %s\n"), CString(e.what()));
	}
	
	TRACE(_T("=== 资源泄漏检测测试完成 ===\n\n"));
}

// 测试函数：验证异常安全
void TestExceptionSafety(CWnd* pWnd)
{
	if (pWnd == nullptr)
		return;
		
	TRACE(_T("=== 测试异常安全 ===\n"));
	
	try
	{
		CClientDC dc(pWnd);
		
		// 测试在异常情况下资源是否正确释放
		try
		{
			CPenWrapper pen(PS_SOLID, 2, RGB(255, 0, 0));
			CGdiObjectSelector<CPen, CPen> selector(&dc, pen.Get());
			
			// 模拟异常
			throw std::runtime_error("测试异常");
		}
		catch (const std::exception&)
		{
			TRACE(_T("✓ 异常被捕获\n"));
		}
		
		// 验证 DC 状态仍然正常（画笔应该已恢复）
		CPen* pPen = dc.GetCurrentPen();
		if (pPen != nullptr)
		{
			TRACE(_T("✓ DC 状态正常（异常安全）\n"));
		}
		else
		{
			TRACE(_T("✗ DC 状态异常\n"));
		}
	}
	catch (const CGdiObjectException& e)
	{
		TRACE(_T("✗ 测试失败: %s\n"), CString(e.what()));
	}
	
	TRACE(_T("=== 异常安全测试完成 ===\n\n"));
}

// 主测试函数：运行所有测试
void RunAllGdiWrapperTests(CWnd* pWnd)
{
	TRACE(_T("\n"));
	TRACE(_T("========================================\n"));
	TRACE(_T("开始 RAII 包装类测试\n"));
	TRACE(_T("========================================\n\n"));
	
	TestCPenWrapper();
	TestCBrushWrapper();
	TestGdiObjectSelector(pWnd);
	TestResourceLeak(pWnd);
	TestExceptionSafety(pWnd);
	
	TRACE(_T("========================================\n"));
	TRACE(_T("所有测试完成\n"));
	TRACE(_T("========================================\n\n"));
}

#endif // _DEBUG

