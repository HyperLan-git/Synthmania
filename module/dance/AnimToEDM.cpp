#include "ChartHandler.hpp"
#include "PlayMode.hpp"

struct UniformBufferObject2 {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec4 color;
    alignas(sizeof(float)) float warp;
};

struct UniformBufferObject3 {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(4) float chroma;
    alignas(4) float blur;
};

struct AnimData {
    uint64_t timing;
    float kick;
    float color;
    float chroma;
    float rotation;
    float blur;
    float x, y;
    float zoom;
    float light;
    float warp;
};

glm::vec3 hsv2rgb(glm::vec3 hsv) {
    float hh, p, q, t, ff;
    int i;
    glm::vec3 rgb;

    hh = hsv.x >= 360.0 ? 0.0 : hsv.x / 60.0;
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

extern "C" class AnimToEDM : public ChartHandler {
   public:
    std::vector<AnimData> datas;
    std::string songFolder;

    AnimToEDM(Gamemode* game)
        : ChartHandler(game),
          songFolder(dynamic_cast<PlayMode*>(game)->getSongFolder()) {
        std::vector<char> file = readFile(songFolder + "/anim.csv");
        file.push_back('\0');

        size_t elem;
        char* cur = NULL;
        char line[2048];
        std::stringstream lines(file.data());
        while (lines.getline(line, 2048)) {
            elem = 0;
            char* token = std::strtok(line, ",");
            while (token != NULL) {
                std::string str = std::string(token);
                bool isNumber =
                    str.find_first_not_of("0123456789. ") == str.npos;
                if (isNumber) {
                    if (datas.size() <= elem) datas.resize(elem + 1);

                    // I hate this
                    if (std::strcmp(cur, "timing") == 0)
                        datas[elem].timing = std::atoll(token);
                    else if (std::strcmp(cur, "kick") == 0)
                        datas[elem].kick = std::atof(token);
                    else if (std::strcmp(cur, "color") == 0)
                        datas[elem].color = std::atof(token);
                    else if (std::strcmp(cur, "x") == 0)
                        datas[elem].x = std::atof(token);
                    else if (std::strcmp(cur, "y") == 0)
                        datas[elem].y = std::atof(token);
                    else if (std::strcmp(cur, "zoom") == 0)
                        datas[elem].zoom = std::atof(token);
                    else if (std::strcmp(cur, "chroma") == 0)
                        datas[elem].chroma = std::atof(token);
                    else if (std::strcmp(cur, "rotation") == 0)
                        datas[elem].rotation = std::atof(token);
                    else if (std::strcmp(cur, "blur") == 0)
                        datas[elem].blur = std::atof(token);
                    else if (std::strcmp(cur, "light") == 0)
                        datas[elem].light = std::atof(token);
                    else if (std::strcmp(cur, "warp") == 0)
                        datas[elem].warp = std::atof(token);
                    elem++;
                } else
                    cur = token;
                token = std::strtok(NULL, ",");
            }
        }
    }

    const std::string getVertShaderCode() override {
        return songFolder + "/v_shader.spv";
    }

    const std::string getGeomShaderCode() override {
        return songFolder + "/g_shader.spv";
    }

    const std::string getFragShaderCode() override {
        return songFolder + "/f_shader.spv";
    }

    const std::string getFinalVertShaderCode() override {
        return songFolder + "/f_v.spv";
    }

    const std::string getFinalGeomShaderCode() override {
        return songFolder + "/f_g.spv";
    }

    const std::string getFinalFragShaderCode() override {
        return songFolder + "/f_f.spv";
    }

    virtual VkDeviceSize getUBOSize() { return sizeof(UniformBufferObject2); }

    virtual VkDeviceSize getFinalUBOSize() {
        return sizeof(UniformBufferObject3);
    }

    void onSpawn(std::shared_ptr<Gui>& g) override {}

    void update(int64_t time) override {}

    AnimData getData(int64_t time) {
        auto data = datas[0], next = *--datas.end();
        if (next.timing < time) return next;
        for (auto iter = datas.rbegin(); iter != datas.rend(); iter++) {
            if (time > iter->timing) {
                data = *iter;
                break;
            }
            next = *(iter);
        }
        if (next.timing == data.timing) return data;

        double progress =
            (time - data.timing) / (long double)(next.timing - data.timing);
        double p = (1. - progress);
        AnimData result;
        result.blur = data.blur * p + next.blur * progress;
        result.chroma = data.chroma * p + next.chroma * progress;
        result.color = data.color * p + next.color * progress;
        result.kick = data.kick * p + next.kick * progress;
        result.light = data.light * p + next.light * progress;
        result.rotation = data.rotation * p + next.rotation * progress;
        result.zoom = data.zoom * p + next.zoom * progress;
        result.warp = data.warp * p + next.warp * progress;
        result.x = data.x * p + next.x * progress;
        result.y = data.y * p + next.y * progress;
        return result;
    }

    size_t updateUBO(void*& ubo, int64_t time) override {
        UniformBufferObject* u = (UniformBufferObject*)ubo;
        UniformBufferObject2* result = new UniformBufferObject2();
        if (time < 0) {
            result->color = glm::vec4(0.3, 0.3, 0.3, 1);
            result->view = u->view;
            result->proj = u->proj;
            ubo = result;
            return sizeof(UniformBufferObject2);
        }
        auto data = getData(time);
        uint64_t MPQ = dynamic_cast<PlayMode*>(game)->getPartition().MPQ;
        double progress = ((time % MPQ) / (double)MPQ);
        int odd = (time % (MPQ * 2)) > MPQ;
        double a = -100 * progress * (progress - 0.2) * data.kick;
        if (a < 0) a = 0;
        double zoom = data.zoom + .02 * a;
        u->view = glm::rotate(
            glm::mat4(1),
            (float)(data.rotation * progress * (progress - 1) * .1f) *
                (odd ? 1 : -1),
            glm::vec3(0, 0, 1));
        u->view[0][0] = u->view[1][1] = zoom;
        result->view = u->view;
        result->proj = u->proj;
        result->color = glm::vec4(
            (data.color) * hsv2rgb(glm::vec3(
                               (time % (MPQ * 4)) / MPQ / 5. * 360, .5, 1)) +
                (data.light) * glm::vec3(1),
            1);
        result->warp = data.warp;
        ubo = result;
        return sizeof(UniformBufferObject2);
    }

    size_t updateFinalUBO(void*& ubo, int64_t time) override {
        UniformBufferObject* u = (UniformBufferObject*)ubo;
        UniformBufferObject3* result = new UniformBufferObject3();
        if (time < 0) {
            result->proj = u->proj;
            result->view = u->view;
            result->chroma = 0;
            result->blur = 0;
            ubo = result;
            return sizeof(UniformBufferObject3);
        }
        auto data = getData(time);
        uint64_t MPQ = dynamic_cast<PlayMode*>(game)->getPartition().MPQ;
        result->proj = u->proj;
        result->view = u->view;
        result->chroma =
            data.chroma * .005f * (1 - (time % (MPQ)) / (double)MPQ);
        result->blur = data.blur * 2.5f * (1 - (time % (MPQ)) / (double)MPQ);
        ubo = result;
        return sizeof(UniformBufferObject3);
    }

    void freeUBO(void*& ubo) { delete (UniformBufferObject2*)ubo; }

    void freeFinalUBO(void*& ubo) { delete (UniformBufferObject3*)ubo; }

    virtual ~AnimToEDM() = default;
};

extern "C" ChartHandler* getChartHandler(Gamemode* game) {
    return new AnimToEDM(game);
}
