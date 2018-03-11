// dxgi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

// ATL/COM
#define _ATL_FREE_THREADED
#include <atlbase.h>
#include <atlcoll.h>
#include <atlcom.h>
#include <atlcomcli.h>

// DXGI
#include <dxgi.h>
#include <dxgi1_2.h>
#pragma comment(lib, "dxgi.lib")

// Direct3D
#include <d3d10_1.h>
#pragma comment(lib, "d3d10_1.lib")

//#include <d3d11.h>
//#pragma comment(lib, "d3d11.lib")

// Direct2D
#include <d2d1_2.h>
#pragma comment(lib, "d2d1.lib")

// WIC
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

int main()
{
    HRESULT hr = S_OK;

    if (SUCCEEDED(hr))
    {
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    }

    CComPtr<IWICImagingFactory> spImagingFactory;
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(
            CLSID_WICImagingFactory1,
            NULL,
            CLSCTX_INPROC_SERVER,
            __uuidof(IWICImagingFactory),
            reinterpret_cast<void**>(&spImagingFactory));
    }

    CComPtr<IDXGIFactory1> spDxgiFactory;
    if (SUCCEEDED(hr))
    {
        hr = CreateDXGIFactory1(IID_PPV_ARGS(&spDxgiFactory));
    }

    CComPtr<ID2D1Factory2> spD2DFactory;
    if (SUCCEEDED(hr))
    {
        D2D1_FACTORY_OPTIONS options;
        ZeroMemory(&options, sizeof(options));

        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED,
            options,
            &spD2DFactory);
    }

    CComPtr<ID2D1Multithread> spD2DLock;
    if (SUCCEEDED(hr))
    {
        hr = spD2DFactory->QueryInterface(&spD2DLock);
    }

    CComPtr<ID3D10Device1> spD3DDevice;
    D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL_10_1;
    if (SUCCEEDED(hr))
    {
        hr = D3D10CreateDevice1(
            NULL,
            D3D10_DRIVER_TYPE_WARP,
            NULL,
            D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS
                | D3D10_CREATE_DEVICE_BGRA_SUPPORT
                | D3D10_CREATE_DEVICE_DEBUG,
            D3D10_FEATURE_LEVEL_10_1,
            D3D10_1_SDK_VERSION,
            &spD3DDevice);
    }
    
    /*
    CComPtr<ID3D11Device> spD3DDevice;
    CComPtr<ID3D11DeviceContext> spD3DDeviceContext;
    if (SUCCEEDED(hr))
    {
        hr = D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_WARP,
            NULL,
            D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS
                | D3D11_CREATE_DEVICE_BGRA_SUPPORT
                | D3D11_CREATE_DEVICE_DEBUG,
            NULL,
            0,
            D3D11_SDK_VERSION,
            &spD3DDevice,
            &d3dFeatureLevel,
            &spD3DDeviceContext);
    }*/

    CComPtr<IDXGIDevice> spDxgiDevice;
    if (SUCCEEDED(hr))
    {
        hr = spD3DDevice->QueryInterface(&spDxgiDevice);
    }
    
    CComPtr<ID2D1Device> spD2DDevice;
    if (SUCCEEDED(hr))
    {
        hr = spD2DFactory->CreateDevice(
            spDxgiDevice,
            &spD2DDevice);
    }

    CComPtr<IWICBitmap> spBitmap;
    if (SUCCEEDED(hr))
    {
        hr = spImagingFactory->CreateBitmap(
            1024,
            1024,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnDemand,
            &spBitmap);
    }

    CComPtr<IWICBitmapLock> spBitmapLock;
    if (SUCCEEDED(hr))
    {
        hr = spBitmap->Lock(
            NULL,
            WICBitmapLockRead | WICBitmapLockWrite,
            &spBitmapLock);
    }

    UINT dwBitmapWidth;
    UINT dwBitmapHeight;
    if (SUCCEEDED(hr))
    {
        hr = spBitmapLock->GetSize(
            &dwBitmapWidth,
            &dwBitmapHeight);
    }

    BYTE* pBitmapData;
    UINT cbBitmap;
    if (SUCCEEDED(hr))
    {
        hr = spBitmapLock->GetDataPointer(
            &cbBitmap,
            &pBitmapData);
    }

    UINT dwBitmapStride;
    if (SUCCEEDED(hr))
    {
        hr = spBitmapLock->GetStride(&dwBitmapStride);
    }

    // Try to share bitmap as a texture and then map the texture?
    CComPtr<ID3D10Texture2D> spSharedTexture;
    if (SUCCEEDED(hr))
    {
        D3D10_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.Width = dwBitmapWidth;
        desc.Height = dwBitmapHeight;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D10_USAGE_DEFAULT;
        desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0
            | D3D10_RESOURCE_MISC_SHARED
            //| D3D10_RESOURCE_MISC_SHARED_NTHANDLE
            //| D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX
            ;

        D3D10_SUBRESOURCE_DATA subData;
        ZeroMemory(&subData, sizeof(subData));

        subData.pSysMem = pBitmapData;
        subData.SysMemPitch = dwBitmapStride;
        subData.SysMemSlicePitch = dwBitmapStride * dwBitmapHeight;

        hr = spD3DDevice->CreateTexture2D(
            &desc,
            &subData,
            &spSharedTexture);
    }

    CComPtr<IDXGISurface> spSharedDxgiSurface;
    if (SUCCEEDED(hr))
    {
        hr = spSharedTexture->QueryInterface(&spSharedDxgiSurface);
    }

    CComPtr<IDXGIResource> spDxgiResource;
    if (SUCCEEDED(hr))
    {
        hr = spSharedTexture->QueryInterface(&spDxgiResource);
    }

    /*CComPtr<IDXGIKeyedMutex> spSurfaceLock;
    if (SUCCEEDED(hr))
    {
        hr = spDxgiResource->QueryInterface(&spSurfaceLock);
    }*/

    /*CComPtr<ID2D1DeviceContext> spD2DDeviceContext;
    if (SUCCEEDED(hr))
    {
        hr = spD2DDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
            &spD2DDeviceContext);
    }

    CComPtr<ID2D1Bitmap1> spD2DBitmap;
    if (SUCCEEDED(hr))
    {
        D2D1_BITMAP_PROPERTIES1 bitmapProps;
        ZeroMemory(&bitmapProps, sizeof(bitmapProps));

        bitmapProps.dpiX = 96;
        bitmapProps.dpiY = 96;
        bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        bitmapProps.pixelFormat.format = DXGI_FORMAT_UNKNOWN;

        hr = spD2DDeviceContext->CreateBitmapFromDxgiSurface(
            spSharedDxgiSurface,
            bitmapProps,
            &spD2DBitmap);
    }

    if (SUCCEEDED(hr))
    {
        spD2DDeviceContext->SetTarget(spD2DBitmap);
    }*/

    CComPtr<ID2D1RenderTarget> spRenderTarget;
    if (SUCCEEDED(hr))
    {
        hr = spD2DFactory->CreateDxgiSurfaceRenderTarget(
            spSharedDxgiSurface,
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                96,
                96),
            &spRenderTarget);
    }

    CComPtr<ID2D1DeviceContext> spD2DDeviceContext;
    if (SUCCEEDED(hr))
    {
        hr = spRenderTarget->QueryInterface(&spD2DDeviceContext);
    }

    D2D1_SIZE_U rtSize;
    ZeroMemory(&rtSize, sizeof(rtSize));
    if (SUCCEEDED(hr))
    {
        rtSize = spD2DDeviceContext->GetPixelSize();
    }

    CComPtr<ID2D1SolidColorBrush> spBrush;
    if (SUCCEEDED(hr))
    {
        D2D1_COLOR_F color;
        ZeroMemory(&color, sizeof(color));

        color.a = 1.0f;
        color.b = 1.0f;
        color.g = 0.0f;
        color.r = 0.0f;

        hr = spD2DDeviceContext->CreateSolidColorBrush(
            color,
            &spBrush);
    }

    /*if (SUCCEEDED(hr)) 
    {
        hr = spSurfaceLock->AcquireSync(0, INFINITE);
    }*/

    if (SUCCEEDED(hr))
    {
        spD2DDeviceContext->BeginDraw();

        spD2DDeviceContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_COPY);

        D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Blue);
        //ZeroMemory(&color, sizeof(color));

        /*
        color.a = 1.0f;
        color.b = 1.0f;
        color.g = 1.0f;
        color.r = 1.0f;*/


        spD2DDeviceContext->Clear(color);
        //spD2DDeviceContext->Flush(&tag1, &tag2);

        D2D1_ELLIPSE ellipse;
        ZeroMemory(&ellipse, sizeof(ellipse));

        ellipse.point.x = 64;
        ellipse.point.y = 64;
        ellipse.radiusX = 32;
        ellipse.radiusY = 32;

        spD2DDeviceContext->FillEllipse(
            ellipse,
            spBrush);
    }

    if (SUCCEEDED(hr))
    {
        hr = spD2DDeviceContext->Flush();
    }

    D2D1_TAG tag1;
    D2D1_TAG tag2;
    if (SUCCEEDED(hr))
    {
        ZeroMemory(&tag1, sizeof(tag1));
        ZeroMemory(&tag2, sizeof(tag2));
        hr = spD2DDeviceContext->EndDraw(&tag1, &tag2);
    }

    /*
    if (SUCCEEDED(hr))
    {
        spD2DLock->Enter();
        spD2DLock->Leave();
    }
    */

    /*if (SUCCEEDED(hr))
    {
        hr = spSurfaceLock->ReleaseSync(0);
    }*/

    ::Sleep(2000);

    /*
    if (SUCCEEDED(hr))
    {
        hr = spSurfaceLock->AcquireSync(0, INFINITE);
    }

    if (SUCCEEDED(hr))
    {
        spD3DDeviceContext->Flush();
    }

    if (SUCCEEDED(hr))
    {
        hr = spSurfaceLock->ReleaseSync(0);
    }

    ::Sleep(2000);
    */

    if (SUCCEEDED(hr))
    {
        spBrush.Release();
        //spD2DDeviceContext->SetTarget(NULL);
        //spD2DBitmap.Release();
        //spD2DDeviceContext.Release();
        //spD2DDevice.Release();
        spD2DDeviceContext.Release();
        spD2DDevice.Release();
        //spD3DDeviceContext.Release();
        spD3DDevice.Release();
        spSharedDxgiSurface.Release();
        spSharedTexture.Release();
        spBitmapLock.Release();
    }

    // Save an image to disk
    CComPtr<IWICStream> spStream;
    if (SUCCEEDED(hr))
    {
        hr = spImagingFactory->CreateStream(&spStream);
    }

    if (SUCCEEDED(hr))
    {
        hr = spStream->InitializeFromFilename(
            L"c:\\temp\\output.png",
            GENERIC_WRITE);
    }

    CComPtr<IWICBitmapEncoder> spEncoder;
    if (SUCCEEDED(hr))
    {
        hr = spImagingFactory->CreateEncoder(
            GUID_ContainerFormatPng,
            NULL,
            &spEncoder);
    }

    if (SUCCEEDED(hr))
    {
        hr = spEncoder->Initialize(
            spStream,
            WICBitmapEncoderNoCache);
    }

    CComPtr<IWICBitmapFrameEncode> spFrame;
    if (SUCCEEDED(hr))
    {
        hr = spEncoder->CreateNewFrame(
            &spFrame,
            NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = spFrame->Initialize(NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = spFrame->WriteSource(
            spBitmap,
            NULL);
    }

    if (SUCCEEDED(hr))
    {
        hr = spFrame->Commit();
    }

    if (SUCCEEDED(hr))
    {
        hr = spEncoder->Commit();
    }

    return 0;
}

