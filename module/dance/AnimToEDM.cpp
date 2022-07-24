#include "GraphicalEffectHandler.hpp"

// We do a bit of trolling
const char VERT[] =
    "#version 450\n"
    "layout(binding = 0) uniform UniformBufferObject {"
    "mat4 model; mat4 view; mat4 proj; vec4 color;} ubo;"
    "layout(location = 0) in vec3 inPosition;"
    "layout(location = 1) in vec2 inTexCoord;"
    "layout(location = 0) out vec2 fragTexCoord;"
    "layout(location = 1) out vec4 color;"
    "layout(push_constant) uniform PushConstants {"
    "vec3 position;float rotation;vec2 size;vec4 color;} constants;"
    "void main() {"
    "vec4 v, c = constants.color;"
    "c.r *= ubo.color.r;c.g *= ubo.color.g;c.b *= ubo.color.b;"
    "c.a *= ubo.color.a;"
    "color = c;"
    "vec3 pos = constants.position;"
    "vec2 size = constants.size;"
    "float rotation = constants.rotation;"
    "v = vec4(inPosition.x, inPosition.y, 0, 1.0);"
    "float c = cos(rotation), s = sin(rotation);"
    "float temp = v.x;"
    "v.x = v.x * c - v.y * s;"
    "v.y = temp * s + v.y * c;"
    "v.x *= size.x;"
    "v.y *= size.y;"
    "v.xyz += pos;"
    "v = ubo.proj * ubo.view * ubo.model * v;"
    "gl_Position = v;"
    "fragTexCoord = inTexCoord;"
    "}";

struct UniformBufferObject2 {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec4 color;
};

glm::vec3 hsv2rgb(glm::vec3 hsv) {
    float hh, p, q, t, ff;
    int i;
    glm::vec3 rgb;

    hh = hsv.x;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (int)hh;
    ff = hh - i;
    p = hsv.z * (1.0 - hsv.y);
    q = hsv.z * (1.0 - (hsv.y * ff));
    t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

    switch (i) {
        case 0:
            rgb.r = hsv.z;
            rgb.g = t;
            rgb.b = p;
            break;
        case 1:
            rgb.r = q;
            rgb.g = hsv.z;
            rgb.b = p;
            break;
        case 2:
            rgb.r = p;
            rgb.g = hsv.z;
            rgb.b = t;
            break;

        case 3:
            rgb.r = p;
            rgb.g = q;
            rgb.b = hsv.z;
            break;
        case 4:
            rgb.r = t;
            rgb.g = p;
            rgb.b = hsv.z;
            break;
        default:
            rgb.r = hsv.z;
            rgb.g = p;
            rgb.b = q;
            break;
    }
    return rgb;
}

extern "C" class AnimToEDM : public GraphicalEffectHandler {
   public:
    AnimToEDM(Synthmania* game) : GraphicalEffectHandler(game) {}

    const char* getVertShaderCode() override { return VERT; }

    const char* getFragShaderCode() override { return NULL; }

    virtual size_t getUBOSize() { return sizeof(UniformBufferObject2); }

    void onSpawn(Gui* g) override {}

    void update(int64_t time) override {}

    size_t updateUBO(void*& ubo, int64_t time) override {
        UniformBufferObject* u = (UniformBufferObject*)ubo;
        UniformBufferObject2* result = new UniformBufferObject2();
        uint64_t MPQ = game->getPartition().MPQ;
        double progress = ((time % MPQ) / (double)MPQ);
        short odd = (time % (MPQ * 2)) > MPQ;
        if (time < 0) odd = !odd;
        double a = -100 * progress * (progress - 0.2);
        if (a < 0) a = 0;
        double zoom = 1 + .02 * a;
        u->view = glm::rotate(
            glm::mat4(1),
            (float)(progress * (progress - 1) * .1f) * (odd ? 1 : -1),
            glm::vec3(0, 0, 1));
        u->view[0][0] = u->view[1][1] = zoom;
        result->model = u->model;
        result->view = u->view;
        result->view = u->view;
        result->proj = u->proj;
        u->color =
            glm::vec4(hsv2rgb(glm::vec3((time % (MPQ * 4)) * 360, 1, 1)), 1);
        ubo = u;
        return sizeof(UniformBufferObject2);
    }

    ~AnimToEDM() = default;
};

extern "C" GraphicalEffectHandler* getEffectHandler(Synthmania* game) {
    return new AnimToEDM(game);
}
