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

// Direct3D
//#include <d3d10_1.h>
//#pragma comment(lib, "d3d10_1.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

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

    CComPtr<ID2D1Factory2> spD2DFactory;
    if (SUCCEEDED(hr))
    {
        D2D1_FACTORY_OPTIONS options;
        ZeroMemory(&options, sizeof(options));

        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION; //D2D1_DEBUG_LEVEL_NONE;

        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_MULTI_THREADED,
            options,
            &spD2DFactory);
    }

    CComPtr<ID3D11Device> spD3DDevice;
    CComPtr<ID3D11DeviceContext> spD3DDeviceContext;
    D3D_FEATURE_LEVEL d3dFeatureLevel;
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
    }

    CComPtr<IDXGIDevice> spDxgiDevice;
    if (SUCCEEDED(hr))
    {
        hr = spD3DDevice->QueryInterface(&spDxgiDevice);
    }

    /*
    UINT dwSupport_DXGI_FORMAT_R8G8B8A8_UNORM;
    if (SUCCEEDED(hr))
    {
        hr = spD3DDevice->CheckFormatSupport(
            DXGI_FORMAT_R8G8B8A8_UNORM,
            &dwSupport_DXGI_FORMAT_R8G8B8A8_UNORM);
    }

    UINT dwSupport_DXGI_FORMAT_B8G8R8A8_UNORM;
    if (SUCCEEDED(hr))
    {
        hr = spD3DDevice->CheckFormatSupport(
            DXGI_FORMAT_B8G8R8A8_UNORM,
            &dwSupport_DXGI_FORMAT_B8G8R8A8_UNORM);
    }*/
    
    /*
    CComPtr<ID3D10Texture2D> spTexture;
    if (SUCCEEDED(hr))
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/dd370966(v=vs.85).aspx

        D3D10_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.Width = 128;
        desc.Height = 128;

        desc.ArraySize = 1;
        desc.BindFlags = D3D10_BIND_RENDER_TARGET;
        desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.MipLevels = 1;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D10_USAGE_DEFAULT;

        hr = spD3DDevice->CreateTexture2D(
            &desc,
            NULL,
            &spTexture);
    }

    CComPtr<IDXGISurface> spSurface;
    if (SUCCEEDED(hr))
    {
        hr = spTexture->QueryInterface(&spSurface);
    }
    */

    /*
    CComPtr<IWICBitmap> spBitmap;
    if (SUCCEEDED(hr))
    {
        hr = spImagingFactory->CreateBitmap(
            1024,
            1024,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapCacheOnLoad,
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

    CComPtr<ID3D11Texture2D> spSharedTexture;
    if (SUCCEEDED(hr))
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.Width = dwBitmapWidth;
        desc.Height = dwBitmapHeight;
        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0; // D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.MipLevels = 1;
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA subData;
        ZeroMemory(&subData, sizeof(subData));

        subData.pSysMem = pBitmapData;
        subData.SysMemPitch = dwBitmapStride;
        subData.SysMemSlicePitch = dwBitmapStride * dwBitmapHeight; //cbBitmap;

        hr = spD3DDevice->CreateTexture2D(
            &desc,
            &subData,
            &spSharedTexture);
    }
    */

    CComPtr<ID3D11Texture2D> spSharedTexture;
    if (SUCCEEDED(hr))
    {
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));

        desc.Width = 1024;
        desc.Height = 1024;
        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.MipLevels = 1;
        desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;

        hr = spD3DDevice->CreateTexture2D(
            &desc,
            NULL,
            &spSharedTexture);
    }

    /*
    D3D11_MAPPED_TEXTURE2D mapped;
    ZeroMemory(&mapped, sizeof(mapped));
    if (SUCCEEDED(hr))
    {
        hr = spSharedTexture->Map(
            D3D10CalcSubresource(0, 0, 1),
            D3D10_MAP_READ_WRITE,
            0,
            &mapped);
    }

    if (SUCCEEDED(hr))
    {
        spSharedTexture->Unmap(
            D3D10CalcSubresource(0, 0, 1));
    }*/

    CComPtr<IDXGISurface> spSharedDxgiSurface;
    if (SUCCEEDED(hr))
    {
        hr = spSharedTexture->QueryInterface(&spSharedDxgiSurface);
    }

    /*
    DXGI_MAPPED_RECT mappedRect;
    ZeroMemory(&mappedRect, sizeof(mappedRect));
    if (SUCCEEDED(hr))
    {
        hr = spSharedDxgiSurface->Map(
            &mappedRect,
            DXGI_MAP_DISCARD | DXGI_MAP_WRITE);
    }

    if (SUCCEEDED(hr))
    {
        hr = spSharedDxgiSurface->Unmap();
    }
    */

    CComPtr<ID2D1Device1> spD2DDevice;
    if (SUCCEEDED(hr))
    {
        hr = spD2DFactory->CreateDevice(
            spDxgiDevice,
            &spD2DDevice);
    }

    CComPtr<ID2D1DeviceContext1> spD2DDeviceContext;
    if (SUCCEEDED(hr))
    {
        hr = spD2DDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &spD2DDeviceContext);
    }

    /*CComPtr<ID2D1RenderTarget> spRenderTarget;
    if (SUCCEEDED(hr))
    {
        D2D1_RENDER_TARGET_PROPERTIES rtProps;
        ZeroMemory(&rtProps, sizeof(rtProps));

        rtProps.dpiX = 96;
        rtProps.dpiY = 96;
        rtProps.minLevel = D2D1_FEATURE_LEVEL_10;
        rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        rtProps.pixelFormat.format = DXGI_FORMAT_UNKNOWN;
        rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;

        hr = spD2DFactory->CreateDxgiSurfaceRenderTarget(
            spSharedDxgiSurface,
            &rtProps,
            &spRenderTarget);
    }*/

    /*
    CComPtr<ID2D1Bitmap1> spD2DBitmap;
    if (SUCCEEDED(hr))
    {
        D2D1_BITMAP_PROPERTIES1 bitmapProps;
        ZeroMemory(&bitmapProps, sizeof(bitmapProps));

        bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        bitmapProps.pixelFormat.format = DXGI_FORMAT_UNKNOWN;

        hr = spD2DDeviceContext->CreateBitmapFromDxgiSurface(
            spSharedDxgiSurface,
            bitmapProps,
            &spD2DBitmap);
    }
    */

    CComPtr<ID2D1Bitmap1> spD2DBitmap;
    if (SUCCEEDED(hr))
    {
        D2D1_BITMAP_PROPERTIES bitmapProps;
        ZeroMemory(&bitmapProps, sizeof(bitmapProps));

        //bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
        bitmapProps.pixelFormat.format = DXGI_FORMAT_UNKNOWN;

        hr = spD2DDeviceContext->CreateSharedBitmap(
            __uuidof(IDXGISurface),
            (void*)spSharedDxgiSurface,
            &bitmapProps,
            &spD2DBitmap);
    }


    D2D1_MAPPED_RECT mappedRect;
    ZeroMemory(&mappedRect, sizeof(mappedRect));
    if (SUCCEEDED(hr))
    {
        hr = spD2DBitmap->Map(
            D2D1_MAP_OPTIONS_READ,
            &mappedRect);
    }

    if (SUCCEEDED(hr))
    {
        hr = spD2DBitmap->Unmap();
    }

    if (SUCCEEDED(hr))
    {
        spD2DDeviceContext->SetTarget(spD2DBitmap);
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

    D2D1_TAG tag1;
    D2D1_TAG tag2;
    if (SUCCEEDED(hr))
    {
        spD2DDeviceContext->BeginDraw();
        
        D2D1_COLOR_F color;
        ZeroMemory(&color, sizeof(color));

        color.a = 1.0f;
        color.b = 1.0f;
        color.g = 1.0f;
        color.r = 1.0f;
        spD2DDeviceContext->Clear(color);
        spD2DDeviceContext->Flush(&tag1, &tag2);

        D2D1_ELLIPSE ellipse;
        ZeroMemory(&ellipse, sizeof(ellipse));

        ellipse.point.x = 64;
        ellipse.point.y = 64;
        ellipse.radiusX = 32;
        ellipse.radiusY = 32;

        spD2DDeviceContext->FillEllipse(
            ellipse,
            spBrush);
        spD2DDeviceContext->Flush(&tag1, &tag2);

        ZeroMemory(&tag1, sizeof(tag1));
        ZeroMemory(&tag2, sizeof(tag2));
        hr = spD2DDeviceContext->EndDraw(&tag1, &tag2);
    }

    if (SUCCEEDED(hr))
    {
        spBrush.Release();
        spD2DDeviceContext->SetTarget(NULL);
        spD2DBitmap.Release();
        spD2DDeviceContext.Release();
        spD2DDevice.Release();
        spSharedDxgiSurface.Release();
        spSharedTexture.Release();
        //spBitmapLock.Release();
    }

    /*
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
    */

    return 0;
}

