//------------------------------------------
//
// 数値を扱う処理群の定義 [math.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once

#include "main.h"
#include <random>
#include <cmath>
#include "DirectXMath.h"
#include <d3dx9math.h>

#define RandomAngle CMath::Random(-D3DX_PI, D3DX_PI) // ランダムな角度を取得するマクロ
#define RandomColor D3DXCOLOR(CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f))
#define RandomColorTranslucent D3DXCOLOR(CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 0.5f))
#define RandomColorA(alpha) D3DXCOLOR(CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f),CMath::Random(0.0f, 1.0f),alpha)

//------------------------------------------
// 数学クラス (インスタンスは持たない)
//------------------------------------------
class CMath
{
public:
	CMath() = delete;

	static HRESULT Rotation(D3DXVECTOR3 resultPos[], const D3DXVECTOR2 size, const float fAngle);

	template<typename T>
	static T Random(T min, T max);
	static int RandomInt(int min, int max);
	static float RandomFloat(float min, float max);
private:
	static std::random_device rd;
	static std::mt19937 gen;
};

//-------------------------------------------
// 乱数を生成する
//--------------------------------------------
template<typename T>
T CMath::Random(T min, T max)
{
    if constexpr (std::is_integral<T>::value)
    {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(gen);
    }
    else if constexpr (std::is_floating_point<T>::value)
    {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(gen);
    }
    else if constexpr (std::is_enum<T>::value)
    {
        std::uniform_int_distribution<int> dist(static_cast<int>(min), static_cast<int>(max));
        return static_cast<T>(dist(gen));
    }
    else
    {
        static_assert(std::is_arithmetic<T>::value, "T must be numeric");
        return T{};
    }
}