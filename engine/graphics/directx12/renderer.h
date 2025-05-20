class Renderer {
public:
    // ... existing code ...

    // レンダーターゲット関連の関数
    HRESULT CreateRenderTargetTexture(UINT width, UINT height, DXGI_FORMAT format, ID3D12Resource** ppTexture);
    HRESULT CreateRenderTargetView(ID3D12Resource* pTexture, D3D12_CPU_DESCRIPTOR_HANDLE handle);
    HRESULT CreateDepthStencilBuffer(UINT width, UINT height, ID3D12Resource** ppDepthStencil);

private:
    // ... existing code ...
}; 