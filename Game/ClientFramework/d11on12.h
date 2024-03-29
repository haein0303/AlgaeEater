#pragma once

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

//전방선언스
class DxEngine;

class UI_ASSET {
public:
	UI_ASSET() {};
	UI_ASSET(const UI_ASSET& tmp):_image(tmp._image),_rect(tmp._rect) {}
	UI_ASSET(ID2D1Bitmap* image, D2D1_RECT_F rect) :_image(image), _rect(rect) {}
public:
	ID2D1Bitmap* _image;
	D2D1_RECT_F _rect = D2D1::RectF(0.0f, 0.0f, 100.f, 100.f);
};

class d11on12
{
private:
	DxEngine* _engine;

	
	ComPtr<ID3D11Device> _Device;
	ComPtr<ID2D1Factory3> m_d2dFactory;
	ComPtr<ID2D1Device2> m_d2dDevice;
	ComPtr<ID3D11On12Device> m_d3d11On12Device;
	ComPtr<ID2D1DeviceContext2> m_d2dDeviceContext;
	ComPtr<IDWriteFactory3> m_dWriteFactory;

	ComPtr<ID3D12Resource> m_renderTargets[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D11Resource> m_wrappedBackBuffers[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID2D1Bitmap1> m_d2dRenderTargets[SWAP_CHAIN_BUFFER_COUNT];
	ComPtr<ID3D11DeviceContext> m_d3d11DeviceContext;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

	
	

	ComPtr<ID2D1SolidColorBrush> mSolidColorBrush = nullptr;
	ID2D1SolidColorBrush* mGrayBrush = nullptr;
	ID2D1SolidColorBrush* mBlueBrush = nullptr;
	ID2D1SolidColorBrush* mRedBrush = nullptr;
	ComPtr<IDWriteTextFormat> mDWriteTextFormat = nullptr;
	ComPtr<IDWriteTextFormat> m_mini_boss_font = nullptr;
	ComPtr<IDWriteTextFormat> m_boss_font = nullptr;
	ComPtr<IDWriteTextFormat> m_info_font = nullptr;



	//이미지용으로 따로 만들어야댕
	IWICImagingFactory* m_pWICFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	UINT m_rtvDescriptorSize;

	WindowInfo _win_info;

	ID2D1Bitmap* _loading_bg;
	vector<ID2D1Bitmap*> _loading_Resource;
	
	vector<LPCWSTR> _loading_msg;
	float loading_counter = 0.f;
	float loading_msg_counter = 0.f;
	int now_img = 0;
	int now_msg = 0;
	float loading_frame = 0.1f;
	D2D1_RECT_F _loading_rect;
	D2D1_RECT_F _loading_msg_rect;

	vector<ID2D1Bitmap*> stage1_boss_bg;
	vector<ID2D1Bitmap*> user_icon;
	vector<ID2D1Bitmap*> glich_effect;

	int glitch_draw = 0;
	int glitch_draw2 = 0;
	
public:
	ID2D1Bitmap* _boss_bg;
	ID2D1Bitmap* _alert_bg;
	ID2D1Bitmap* _clear_bg;
	ID2D1Bitmap* _fail_bg;
	
	ID2D1Bitmap* _stage2_boss_skill_bg;

	vector<ID2D1Bitmap*> _v_Resource;
	~d11on12() {
		m_d2dFactory->Release();
	}

	void init(DxEngine* engine, WindowInfo windowInfo);
	void LoadPipeline();
	void Loading_info();
	void Loading_draw(const float& time);

	void Late_load();

	ID2D1Bitmap* addResource(LPCWSTR path);
	void RenderUI(int mCurrBackbufferIndex);
	void LateRenderUI(vector<UI_ASSET> scene_asset);
	void draw_UI(const UI_ASSET& draw);
	void draw_text(LPCWSTR text, D2D1_RECT_F rect);
	void draw_game_clear(bool win);
	void draw_stage2_atk();

	void draw_bossUI(int hp,int stage,const OBJECT& boss_obj);
	void logic_update_glich();
	void draw_glich();

	void draw_infotext(LPCWSTR text, D2D1_RECT_F rect);
	void ExcuteUI(int mCurrBackbufferIndex);


	void draw_player_info(LPCWSTR text, int hp_max, int hp, int type);
};

template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}