#include "ge_log.h"
#include "ge_aurora_noise_shader.h"
#include "ge_visual_effect_impl.h"

namespace OHOS {
namespace Rosen {
using CacheDataType = std::shared_ptr<Drawing::Image>;
GEAuroraNoiseShader::GEAuroraNoiseShader() {}
GEAuroraNoiseShader::GEAuroraNoiseShader(Drawing::GEAuroraNoiseShaderParams& param)
{
    auroraNoiseParams_ = param;
}
std::shared_ptr<GEAuroraNoiseShader> GEAuroraNoiseShader::CreateAuroraNoiseShader(
    Drawing::GEAuroraNoiseShaderParams& param)
{
    std::shared_ptr<GEAuroraNoiseShader> auroraNoiseShader = std::make_shared<GEAuroraNoiseShader>(param);
    return auroraNoiseShader;
}
void GEAuroraNoiseShader::MakeDrawingShader(const Drawing::Rect& rect, float progress)
{
    drShader_ = MakeAuroraNoiseShader(rect);
}
std::shared_ptr<Drawing::RuntimeShaderBuilder> GEAuroraNoiseShader::GetAuroraNoiseAtlasPrecalculationBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> auroraNoiseAtlasShaderPrecalculation_ = nullptr;
    if (auroraNoiseAtlasShaderPrecalculation_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform vec2 iResolution;

            const float TILE_COUNT = 8.0;
            const float TILE_SIZE = 64.0;
            const float contrast = 4.64; // contrast constant, 464.0 / 100.0
            const float brightness = -0.17647; // brightness constant, -45.0 / 255.0

            float SNoise(in vec3 v);

            vec4 main(vec2 fragCoord)
            {
                vec2 tileSize = iResolution.xy / TILE_COUNT;
                vec2 tileCoord = floor(fragCoord / tileSize);
                float zIndex = tileCoord.y * TILE_COUNT + tileCoord.x;
                float t = (zIndex + 0.5) / (TILE_COUNT * TILE_COUNT); // [0,1)

                vec2 localXY = mod(fragCoord, tileSize);
                vec2 uv = (localXY + 0.5) / tileSize;

                float aspect = iResolution.x / iResolution.y;
                vec2 p = vec2((uv.x / 0.75 - 1.0) * aspect, (uv.y * 2.0 - 1.0));

                float freqX = 2.0;
                float freqY = mix(freqX, freqX * 0.5, smoothstep(1.0, 0.0, uv.y));
                vec2 dom = vec2(p.x * freqX, p.y * freqY);

                float n = abs(SNoise(vec3(dom, t)));
                float alpha = clamp(1.0 - (n * contrast + brightness), 0.0, 1.0);
                return vec4(alpha);
            }

            vec3 Mod289(in vec3 x)
            {
                return x - floor(x * (1.0 / 289.0)) * 289.0; // 289.0: prime number for permutation
            }

            vec4 Mod289(in vec4 x)
            {
                return x - floor(x * (1.0 / 289.0)) * 289.0; // 289.0: prime number for permutation
            }

            vec4 Permute(in vec4 x)
            {
                return Mod289(((x * 34.0) + 1.0) * x); // 34.0: prime number for permutation
            }

            vec4 TaylorInvSqrt(in vec4 r)
            {
                return 1.79284291400159 - 0.85373472095314 * r; // Taylor series approximation for 1/sqrt(x)
            }

            float SNoise(in vec3 v)
            {
                const vec2 c = vec2(0.16666666666667, 0.33333333333333); // Constants for noise function, 1/6 and 1/3
                const vec4 d = vec4(0.0, 0.5, 1.0, 2.0); // Constants for noise function

                // First corner
                vec3 i = floor(v + dot(v, c.yyy));
                vec3 x0 = v - i + dot(i, c.xxx);

                // Other corners
                vec3 g = step(x0.yzx, x0.xyz);
                vec3 l = 1.0 - g;
                vec3 i1 = min(g.xyz, l.zxy);
                vec3 i2 = max(g.xyz, l.zxy);

                vec3 x1 = x0 - i1 + c.xxx;
                vec3 x2 = x0 - i2 + c.yyy; // 2.0 * c.x = 1/3 = c.y
                vec3 x3 = x0 - d.yyy; // -1.0 + 3.0 * c.x = -0.5 = -d.y

                // Permutations
                i = Mod289(i);
                vec4 p = Permute(Permute(Permute(
                    i.z + vec4(0.0, i1.z, i2.z, 1.0))
                    + i.y + vec4(0.0, i1.y, i2.y, 1.0))
                    + i.x + vec4(0.0, i1.x, i2.x, 1.0));

                float n = 0.142857142857; // i.e., 1.0 / 7.0
                vec3 ns = n * d.wyz - d.xzx;

                vec4 j = p - 49.0 * floor(p * ns.z * ns.z); // 49.0: mod(p, 7 * 7)

                vec4 xs = floor(j * ns.z);
                vec4 ys = floor(j - 7.0 * xs); // mod(j, N) // 7.0: mod(p, 7)

                vec4 x = xs *ns.x + ns.yyyy;
                vec4 y = ys *ns.x + ns.yyyy;
                vec4 h = 1.0 - abs(x) - abs(y);

                vec4 b0 = vec4(x.xy, y.xy);
                vec4 b1 = vec4(x.zw, y.zw);

                vec4 s0 = floor(b0) * 2.0 + 1.0; // 2.0: step size
                vec4 s1 = floor(b1) * 2.0 + 1.0; // 2.0: step size
                vec4 sh = -step(h, vec4(0.0));

                vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy ;
                vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww ;

                vec3 p0 = vec3(a0.xy, h.x);
                vec3 p1 = vec3(a0.zw, h.y);
                vec3 p2 = vec3(a1.xy, h.z);
                vec3 p3 = vec3(a1.zw, h.w);

                // Normalise gradients
                vec4 norm = TaylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
                p0 *= norm.x;
                p1 *= norm.y;
                p2 *= norm.z;
                p3 *= norm.w;

                // Mix final noise value
                vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0); // 0.6: falloff start
                m = m * m;
                return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3))); // scale to [-1,1]
            }
        )";
        auroraNoiseAtlasShaderPrecalculation_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (auroraNoiseAtlasShaderPrecalculation_ == nullptr) {
        GE_LOGE("GEAuroraNoiseShader auroraNoiseAtlasShaderPrecalculation_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(auroraNoiseAtlasShaderPrecalculation_);
}
void GEAuroraNoiseShader::Preprocess(Drawing::Canvas& canvas, const Drawing::Rect& rect)
{
    if (cacheAnyPtr_ == nullptr) {
        Drawing::ImageInfo cacheImgInf(rect.GetWidth(), rect.GetHeight(),
            Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_OPAQUE);
        auto cacheImg = MakeAuroraNoiseAtlasPrecalculationShader(canvas, cacheImgInf);
        if (cacheImg) {
            cacheAnyPtr_ = std::make_shared<std::any>(std::make_any<CacheDataType>(cacheImg));
            GE_LOGD(" GEAuroraNoiseShader Preprocess OK");
        } else {
            GE_LOGD(" GEAuroraNoiseShader Preprocess NG");
        }
    }
}
std::shared_ptr<Drawing::Image>
    GEAuroraNoiseShader::MakeAuroraNoiseAtlasPrecalculationShader(Drawing::Canvas& canvas,
    const Drawing::ImageInfo& imageInfo)
{
    float width = imageInfo.GetWidth();
    float height = imageInfo.GetHeight();
    auto builder = GetAuroraNoiseAtlasPrecalculationBuilder();
    builder->SetUniform("iResolution", width, height);
    auto auroraNoiseAtlasShader = builder->MakeImage(canvas.GetGPUContext().get(), nullptr, imageInfo, false);
    if (auroraNoiseAtlasShader == nullptr) {
        GE_LOGE("GEAuroraNoiseShader auroraNoiseAtlasShader is nullptr.");
        return nullptr;
    }
    return auroraNoiseAtlasShader;
}
std::shared_ptr<Drawing::RuntimeShaderBuilder> GEAuroraNoiseShader::GetAuroraNoiseBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> auroraNoiseShaderEffect_ = nullptr;
    if (auroraNoiseShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader atlasImage;
            uniform vec2 iResolution;
            uniform float noise;

            const float TILE_COUNT = 8.0;

            vec4 main(vec2 fragCoord) {
                float TILE_TOTAL = TILE_COUNT * TILE_COUNT;
                float z = clamp(noise, 0.0, 0.999) * TILE_TOTAL;
                float z0 = floor(z);
                float z1 = min(z0 + 1.0, TILE_TOTAL - 1.0);
                float alpha = smoothstep(0.0, 1.0, fract(z));

                float tileX0 = mod(z0, TILE_COUNT);
                float tileY0 = floor(z0 / TILE_COUNT);
                float tileX1 = mod(z1, TILE_COUNT);
                float tileY1 = floor(z1 / TILE_COUNT);

                vec2 tileSize = iResolution.xy / TILE_COUNT;
                vec2 uvInTile = fragCoord / iResolution.xy;
                vec2 pixel = uvInTile * (tileSize - 1.0);

                vec2 texCoord0 = (pixel + vec2(tileX0, tileY0) * tileSize + 0.5);
                vec2 texCoord1 = (pixel + vec2(tileX1, tileY1) * tileSize + 0.5);

                vec4 col0 = atlasImage.eval(texCoord0);
                vec4 col1 = atlasImage.eval(texCoord1);

                return mix(col0, col1, alpha);
            }
        )";
        auroraNoiseShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (auroraNoiseShaderEffect_ == nullptr) {
        GE_LOGE("GEAuroraNoiseShader::GetAuroraNoiseBuilder auroraNoiseShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(auroraNoiseShaderEffect_);
}
std::shared_ptr<Drawing::RuntimeShaderBuilder> GEAuroraNoiseShader::GetAuroraNoiseVerticalBlurBuilder()
{
    thread_local std::shared_ptr<Drawing::RuntimeEffect> auroraNoiseVerticalBlurShaderEffect_ = nullptr;

    if (auroraNoiseVerticalBlurShaderEffect_ == nullptr) {
        static constexpr char prog[] = R"(
            uniform shader auroraNoiseTexture;
            uniform vec2 iResolution;
            const int sampleCount = 20;
            vec2 weights[21]; // precalculated t (i / sampleCount) and weights (1 - t) for the blur
            void initWeights() {
                weights[0] = vec2(0.0, 1.0);
                weights[1] = vec2(0.05, 0.95);
                weights[2] = vec2(0.10, 0.90);
                weights[3] = vec2(0.15, 0.85);
                weights[4] = vec2(0.20, 0.80);
                weights[5] = vec2(0.25, 0.75);
                weights[6] = vec2(0.30, 0.70);
                weights[7] = vec2(0.35, 0.65);
                weights[8] = vec2(0.40, 0.60);
                weights[9] = vec2(0.45, 0.55);
                weights[10] = vec2(0.50, 0.50);
                weights[11] = vec2(0.55, 0.45);
                weights[12] = vec2(0.60, 0.40);
                weights[13] = vec2(0.65, 0.35);
                weights[14] = vec2(0.70, 0.30);
                weights[15] = vec2(0.75, 0.25);
                weights[16] = vec2(0.80, 0.20);
                weights[17] = vec2(0.85, 0.15);
                weights[18] = vec2(0.90, 0.10);
                weights[19] = vec2(0.95, 0.05);
                weights[20] = vec2(1.0, 0.0);
            }

            vec4 main(vec2 fragCoord)
            {
                vec2 uv = fragCoord / iResolution.xy;
                float dist = 1.2 - uv.y; // 1.2: origin height of the vertical blur
                float blurRadius = mix(0.0, 0.3, smoothstep(0.0, 1.2, dist)); // 0.3: blur radius on top, 1.2: origin
                initWeights();

                vec4 col = vec4(0.0);
                float totalWeight = 0.0;
                for (int i = 0; i <= sampleCount; ++i) {
                    vec2 offset = vec2(0.0, weights[i].r * blurRadius);
                    vec2 sampleUV = uv + offset;

                    sampleUV = clamp(sampleUV, vec2(0.0), vec2(1.0));
                    vec2 sampleCoord = sampleUV * iResolution.xy;

                    col += auroraNoiseTexture.eval(sampleCoord) * weights[i].g;
                    totalWeight += weights[i].g;
                }

                return col / totalWeight;
            }
        )";
        auroraNoiseVerticalBlurShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
    }

    if (auroraNoiseVerticalBlurShaderEffect_ == nullptr) {
        GE_LOGE("GEAuroraNoiseShader::GetAuroraNoiseBuilder auroraNoiseVerticalBlurShaderEffect_ is nullptr.");
        return nullptr;
    }
    return std::make_shared<Drawing::RuntimeShaderBuilder>(auroraNoiseVerticalBlurShaderEffect_);
}

std::shared_ptr<Drawing::ShaderEffect> GEAuroraNoiseShader::MakeAuroraNoiseShader(const Drawing::Rect& rect)
{
    if (cacheAnyPtr_ == nullptr) {
        GE_LOGD("GEAuroraNoiseShader MakeAuroraNoiseShader cache is nullptr.");
        return nullptr;
    }
    auto atlasPrecalculationImage = std::any_cast<CacheDataType>(*cacheAnyPtr_);
    auto width = rect.GetWidth();
    auto height = rect.GetHeight();
    builder_ = GetAuroraNoiseBuilder();
    Drawing::Matrix matrix;
    auto atlasShader = Drawing::ShaderEffect::CreateImageShader(*atlasPrecalculationImage, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder_->SetChild("atlasImage", atlasShader);
    builder_->SetUniform("iResolution", width, height);
    builder_->SetUniform("noise", auroraNoiseParams_.noise_);
    auto auroraNoiseShader = builder_->MakeShader(nullptr, false);

    verticalBlurBuilder_ = GetAuroraNoiseVerticalBlurBuilder();
    if (verticalBlurBuilder_ == nullptr) {
        GE_LOGE("GEAuroraNoiseShader::MakeAuroraNoiseShader verticalBlurBuilder_ is nullptr.");
        return nullptr;
    }
    verticalBlurBuilder_->SetChild("auroraNoiseTexture", auroraNoiseShader);
    verticalBlurBuilder_->SetUniform("iResolution", width, height);
    auto auroraNoiseVerticleBlurShader = verticalBlurBuilder_->MakeShader(nullptr, false);
    if (auroraNoiseVerticleBlurShader == nullptr) {
        GE_LOGE("GEAuroraNoiseShader::MakeAuroraNoiseShader auroraNoiseVerticleBlurShader is nullptr.");
        return nullptr;
    }
    return auroraNoiseVerticleBlurShader;
}
} // namespace Rosen
} // namespace OHOS   