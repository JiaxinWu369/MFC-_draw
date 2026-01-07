# advanced-programming
advanced programming
# 简易绘图板 - AI交流日志
记录与AI的交流过程、问题解决、代码迭代等内容，用于项目复盘和知识沉淀。

## 基本信息
-
 项目名称：简易绘图板
-
 开始时间：2025-12-13
-
 技术栈：MFC、C++、Windows SDK

## 日志正文
### 2025-12-10 问题：画布初始化，双缓冲防止闪烁
#### 问题描述
##### 画布初始化（双缓冲 + Strategy 模式）
开发场景：实现功能 F-01（画布初始化，双缓冲防止闪烁），基于 Strategy 模式设计 IRenderStrategy 接口。  
Prompt：基于MFC CScrollView实现图板画布，要求：1.双缓冲绘制防止闪烁；2.遵循Strategy模式定义IRenderStrategy接口，支持不同渲染方式切换；3.适配Windows 10/11 x64，VS2022环境；4.符合RAII规范管理GDI句柄。请提供完整的头文件和cpp文件核心代码。
```cpp
// IRenderStrategy.h
#pragma once
#include <afxwin.h>

class IRenderStrategy {
public:
    virtual ~IRenderStrategy() = default;
    virtual void Render(CDC* pDC, const CRect& rect) = 0;
};

// DoubleBufferRender.h
#pragma once
#include "IRenderStrategy.h"

class DoubleBufferRender : public IRenderStrategy {
public:
    void Render(CDC* pDC, const CRect& rect) override {
        CDC memDC;
        CBitmap memBmp;
        memDC.CreateCompatibleDC(pDC);
        memBmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
        CBitmap* pOldBmp = memDC.SelectObject(&memBmp);
        
        // 绘制逻辑（占位）
        memDC.FillSolidRect(rect, RGB(255, 255, 255));
        
        pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
        memDC.SelectObject(pOldBmp);
    }
};

// DrawingView.h
#pragma once
#include <afxscrollview.h>
#include "DoubleBufferRender.h"

class CDrawingView : public CScrollView {
    DECLARE_DYNCREATE(CDrawingView)
private:
    std::unique_ptr<IRenderStrategy> m_pRenderStrategy;
public:
    CDrawingView() { m_pRenderStrategy = std::make_unique<DoubleBufferRender>(); }
    void SetRenderStrategy(std::unique_ptr<IRenderStrategy> pStrategy) { m_pRenderStrategy = std::move(pStrategy); }
protected:
    void OnDraw(CDC* pDC) override {
        CRect rect;
        GetClientRect(&rect);
        m_pRenderStrategy->Render(pDC, rect);
    }
};
```
### 2025-12-11 问题：基本图形绘制
#### 问题描述
##### 图形工厂实现（ShapeFactory）
开发场景：实现功能 F-02（基本图形绘制），基于 Factory 模式创建 ShapeFactory 类，支持直线、矩形、椭圆绘制  
Prompt：基于MFC GDI+实现图形绘制工厂：1. 定义抽象基类IShape，包含纯虚函数Draw(CDC*)和SetAttributes(COLORREF, int)；2. 实现LineShape、RectShape、EllipseShape子类；3. 创建ShapeFactory类，提供CreateShape(ShapeType)静态方法；4. 支持颜色和线宽设置，适配项目的属性面板数据。
```cpp
// ShapeType.h
enum class ShapeType { Line, Rect, Ellipse };

// IShape.h
#pragma once
#include <afxwin.h>

class IShape {
public:
    virtual ~IShape() = default;
    virtual void Draw(CDC* pDC) = 0;
    void SetAttributes(COLORREF color, int lineWidth) {
        m_color = color;
        m_lineWidth = lineWidth;
    }
protected:
    COLORREF m_color = RGB(0, 0, 0);
    int m_lineWidth = 1;
};

// LineShape.h
class LineShape : public IShape {
public:
    void SetPoints(CPoint start, CPoint end) { m_start = start; m_end = end; }
    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CPen* pOldPen = pDC->SelectObject(&pen);
        pDC->MoveTo(m_start);
        pDC->LineTo(m_end);
        pDC->SelectObject(pOldPen);
    }
private:
    CPoint m_start, m_end;
};

// ShapeFactory.h
class ShapeFactory {
public:
    static std::unique_ptr<IShape> CreateShape(ShapeType type) {
        switch (type) {
            case ShapeType::Line: return std::make_unique<LineShape>();
            case ShapeType::Rect: return std::make_unique<RectShape>();
            case ShapeType::Ellipse: return std::make_unique<EllipseShape>();
            default: throw std::invalid_argument("Invalid shape type");
        }
    }
};
```
### 2025-12-11 问题：撤销 / 重做
#### 问题描述
##### 撤销 / 重做功能（Command 模式）
开发场景：实现功能 F-04（撤销 / 重做），基于 Command 模式设计命令栈，管理图形绘制操作  
Prompt：基于MFC实现绘图板的撤销/重做功能：1. 定义ICommand抽象接口，包含Execute()、Undo()、Redo()方法；2. 实现DrawShapeCommand类，封装IShape对象的绘制和删除；3. 设计CommandStack类，维护Undo栈和Redo栈，支持PushCommand、Undo、Redo方法；4. 线程安全，适配项目的TaskScheduler单例。
```cpp
// ICommand.h
#pragma once

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

// DrawShapeCommand.h
#include "ICommand.h"
#include "IShape.h"
#include "DrawingModel.h"

class DrawShapeCommand : public ICommand {
public:
    DrawShapeCommand(std::shared_ptr<DrawingModel> pModel, std::unique_ptr<IShape> pShape)
        : m_pModel(pModel), m_pShape(std::move(pShape)) {}
    
    void Execute() override { m_pModel->AddShape(m_pShape->Clone()); }
    void Undo() override { m_pModel->RemoveLastShape(); }
    void Redo() override { m_pModel->AddShape(m_pShape->Clone()); }
private:
    std::shared_ptr<DrawingModel> m_pModel;
    std::unique_ptr<IShape> m_pShape;
};

// CommandStack.h
#include <vector>
#include <memory>

class CommandStack {
public:
    void PushCommand(std::unique_ptr<ICommand> pCmd) {
        pCmd->Execute();
        m_undoStack.push_back(std::move(pCmd));
        m_redoStack.clear();
    }
    
    bool Undo() {
        if (m_undoStack.empty()) return false;
        auto pCmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();
        pCmd->Undo();
        m_redoStack.push_back(std::move(pCmd));
        return true;
    }
    
    bool Redo() {
        if (m_redoStack.empty()) return false;
        auto pCmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();
        pCmd->Redo();
        m_undoStack.push_back(std::move(pCmd));
        return true;
    }
private:
    std::vector<std::unique_ptr<ICommand>> m_undoStack;
    std::vector<std::unique_ptr<ICommand>> m_redoStack;
};
```
### 2025-12-11 问题：基本图形绘制
#### 问题描述
##### GDI 句柄 RAII 包装与错误处理
开发场景：实现功能 F-07（错误处理），基于 RAII 模式封装 GDI 对象，确保资源安全释放  
Prompt：实现MFC GDI对象的RAII包装类：1. 支持CPen、CBrush、CImage、CFont等常用GDI对象；2. 构造函数创建对象，析构函数自动释放，支持手动释放；3. 重载->和*运算符，方便访问原始对象；4. 支持移动语义，禁止拷贝；5. 集成项目的LogWriter，记录创建和释放日志。
```cpp
// CGdiObjectWrapper.h
#pragma once
#include <afxwin.h>
#include <utility>
#include "LogWriter.h"

template <typename T>
class CGdiObjectWrapper {
static_assert(std::is_base_of<CGdiObject, T>::value, "T must be derived from CGdiObject");
public:
    // 默认构造（空对象）
    CGdiObjectWrapper() : m_pObject(nullptr), m_own(true) {}

    // 构造函数（直接创建对象，适用于无参数构造的GDI对象）
    CGdiObjectWrapper() : m_pObject(new T()), m_own(true) {
        LogWriter::GetInstance().WriteInfo("CGdiObjectWrapper created: " + std::string(typeid(T).name()));
    }

    // 移动构造
    CGdiObjectWrapper(CGdiObjectWrapper&& other) noexcept : m_pObject(other.m_pObject), m_own(other.m_own) {
        other.m_pObject = nullptr;
        other.m_own = false;
    }

    // 禁止拷贝
    CGdiObjectWrapper(const CGdiObjectWrapper&) = delete;
    CGdiObjectWrapper& operator=(const CGdiObjectWrapper&) = delete;

    // 析构函数
    ~CGdiObjectWrapper() {
        Release();
    }

    // 手动释放
    void Release() {
        if (m_pObject && m_own) {
            LogWriter::GetInstance().WriteInfo("CGdiObjectWrapper released: " + std::string(typeid(T).name()));
            delete m_pObject;
            m_pObject = nullptr;
        }
    }

    // 重载运算符
    T* operator->() const { return m_pObject; }
    T& operator*() const { return *m_pObject; }
    operator T*() const { return m_pObject; }

private:
    T* m_pObject;
    bool m_own; // 是否拥有对象所有权
};
```
