#include "ScoreScreen.hpp"

ScoreScreen::ScoreScreen(Game& game) : Menu(game) {
    TextHandler& text = game.getTextHandler();
    Texture button("button"), button_pressed("button-pressed");
    std::shared_ptr<Button> back =
        std::make_shared<Button>(button, button_pressed, "quit",
                                 glm::vec2({-1.5, .85}), glm::vec2({.6, .3}));
    this->buttons.push_back(back);
    for (Text t : text.createText("Quit", "Stupid", 12, glm::vec2({-.1, 0}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "Back_arrow", back);
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }

    std::shared_ptr<Button> save =
        std::make_shared<Button>(button, button_pressed, "save replay",
                                 glm::vec2({1.5, .85}), glm::vec2({.6, .3}));
    this->buttons.push_back(save);
    for (Text t : text.createText("Save", "Stupid", 12, glm::vec2({-.1, 0}))) {
        std::shared_ptr<ParentedGui> g = std::make_shared<ParentedGui>(
            t.character.texture, "Back_arrow", save);
        g->setPosition(t.pos);
        g->setSize(t.size);
        guis.push_back(g);
    }
    guisKept = guis.size();
}

void ScoreScreen::show() {
    guis.resize(guisKept);
    Synthmania& s = dynamic_cast<Synthmania&>(game);
    std::shared_ptr<Gui> bg =
        std::make_shared<Gui>(Texture("score_bg"), "scoreBg");
    bg->setSize({3.65, 2});
    game.addGui(bg);
    score = s.getLastScore();
    replay = s.getReplay();
    replayStart = s.getReplayStart();
    std::string rating;
    if (score.misses == 0) {
        if (score.fars == score.nears && score.fars == 0)
            rating = "SS";
        else
            rating = "FC";
    } else if (score.score > 950000)
        rating = "S";
    else if (score.score > 900000)
        rating = "A";
    else if (score.score > 850000)
        rating = "B";
    else if (score.score > 800000)
        rating = "C";
    else
        rating = "D";

    TextHandler& text = game.getTextHandler();

    SPAWN_SHADOWED_TEXT(text, guis, rating, "rank", "Stupid", 70,
                        glm::vec2({1, -.3}), glm::vec4(.2, 1, .2, 1));

    SPAWN_TEXT(text, guis, std::to_string(score.score), "score", "Stupid", 20,
               glm::vec2({.75, .65}));

    SPAWN_TEXT(text, guis, "Perfects : " + std::to_string(score.perfects),
               "perfects", "Stupid", 15, glm::vec2({-1.75, .05}), glm::vec4(1));
    SPAWN_TEXT(text, guis, "Near : " + std::to_string(score.nears), "near",
               "Stupid", 15, glm::vec2({-1.75, .25}), glm::vec4(1));
    SPAWN_TEXT(text, guis, "Far : " + std::to_string(score.fars), "far",
               "Stupid", 15, glm::vec2({-1.75, .45}), glm::vec4(1));
    SPAWN_TEXT(text, guis, "Missed : " + std::to_string(score.misses), "misses",
               "Stupid", 15, glm::vec2({-1.75, .65}), glm::vec4(1));

    Menu::show();
}

bool ScoreScreen::saveReplay(std::string path) {
    if (!std::filesystem::create_directories(path)) return false;

    try {
        std::ofstream data(path + "/data.json");
        tree ptree;
        ptree.add("start", this->replayStart);
        ptree.add("diff", this->score.diff);
        ptree.add("chart", this->score.chart);
        ptree.add("charter", this->score.charter);
        boost::property_tree::write_json(data, ptree, false);
        data.close();

        libremidi::writer writer;
        // 1 tick per ms and 120 bpm
        writer.ticksPerQuarterNote = 500;
        writer.add_event(0, 1, libremidi::meta_events::tempo(500000));
        int32_t lastTick = replayStart, tick = lastTick;
        for (auto& e : this->replay) {
            tick = e.timestamp - lastTick;
            lastTick = e.timestamp;
            writer.add_event(
                tick / 1000, 0,
                libremidi::message{
                    libremidi::message::make_command(e.type, e.channel),
                    e.data1, e.data2});
        }
        std::ofstream file(path + "/replay.mid", std::ios::binary);
        writer.write(file);
        file.close();
    } catch (boost::property_tree::json_parser_error err) {
        return false;
    }
    return true;
}

void ScoreScreen::onPressed(const std::shared_ptr<Button>& b) {
    Synthmania& s = dynamic_cast<Synthmania&>(game);
    s.playSound("click");
    s.resetScene();
    if (b->getName().compare("quit") == 0)
        s.loadMenu("main");
    else if (b->getName().compare("save replay") == 0) {
        saveReplay("replays/test");
        s.loadMenu("main");
    }
}

ScoreScreen::~ScoreScreen() {}