#include "Judgement.hpp"

Judgement::Judgement(const char* name, std::vector<ImageView*> textures,
                     TrackPartition notes)
    : Gui(getTextureByName(textures, "judgement_line"), name) {
    this->partition = notes;
}

bool Judgement::update(float time) {
    if (this->partition.notes.empty()) return false;
    auto notes = this->partition.notes;
    MidiNote prev = (notes[0]), next = (notes[notes.size() - 1]);
    for (MidiNote note : notes) {
        if (note.timestamp > prev.timestamp && note.timestamp / 1000000. < time)
            prev = note;
        else if (note.timestamp < next.timestamp &&
                 note.timestamp / 1000000. > time)
            next = note;
    }
    double progress = (next.timestamp - prev.timestamp);
    if (progress != 0) progress = (time * 1000000 - prev.timestamp) / progress;
    this->position.y =
        0.25 + progress * -0.083f * getDifferenceFromC4(next.note) +
        (1 - progress) * -0.083f * getDifferenceFromC4(prev.note);

    return false;
}

ShaderData* Judgement::getShaderData() const {
    ShaderData* data = new ShaderData();
    GuiData* edata = (GuiData*)malloc(sizeof(GuiData));
    edata->pos = position;
    edata->rot = rotation;
    edata->size = size;
    data->data = edata;
    data->size = sizeof(GuiData);
    return data;
}