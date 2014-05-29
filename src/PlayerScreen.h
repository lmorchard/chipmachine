#include <memory>
#include <coreutils/vec.h>
#include <grappix/grappix.h>

#include "SongInfo.h"

class PlayerScreen {
public:

	struct TextField {
		TextField(const std::string &text, float x, float y, float sc, uint32_t col) : text(text), pos(x, y), scale(sc), color(col), add(0), f {&pos.x, &pos.y, &scale, &color.r, &color.g, &color.b, &color.a, &add} {
		}

		std::string text;
		utils::vec2f pos;

		float scale;
		grappix::Color color;
		float add;

		float& operator[](int i) { return *f[i]; }

		int size() { return 8; }
	private:
		float* f[8];
	};

	void render(uint32_t d) {
		for(auto &f : fields) {
			grappix::screen.text(font, f->text, f->pos.x, f->pos.y, f->color + f->add, f->scale);
		}
	}

	void setFont(const grappix::Font &font) {
		this->font = font;
	}

	grappix::Font& getFont() {
		return font;
	}

	std::shared_ptr<TextField> addField(const std::string &text, float x = 0, float y = 0, float scale = 1.0, uint32_t color = 0xffffffff) {
		fields.push_back(std::make_shared<TextField>(text, x, y, scale, color));
		return fields.back();
	}

	void removeField(const std::shared_ptr<TextField> &field) {
		auto it = fields.begin();
		while(it != fields.end()) {
			if(field.get() == it->get())
				it = fields.erase(it);
			else
				it++;
		}
	}

private:

	grappix::Font font;
	std::vector<std::shared_ptr<TextField>> fields;
};

struct SongInfoField {

		SongInfoField() {
		}

		SongInfoField(int x, int y, float sc = 1.0, uint32_t color = 0xffffffff) {
			fields[0] = std::make_shared<PlayerScreen::TextField>("", x, y, sc, color);
			fields[1] = std::make_shared<PlayerScreen::TextField>("", x, y+50*sc, sc*0.6, color);
			fields[2] = std::make_shared<PlayerScreen::TextField>("", x, y+100*sc, sc*0.4, color);
		}

		SongInfoField(PlayerScreen &ps, int x, int y, float sc = 1.0, uint32_t color = 0xffffffff) : fields { ps.addField("", x, y, sc, color), ps.addField("", x, y+30*sc, sc*0.6, color),  ps.addField("", x, y+50*sc, sc*0.4, color) } {}

		void setInfo(const SongInfo &info) {
			fields[0]->text = info.title;
			fields[1]->text = info.composer;
			fields[2]->text = info.format;
		}

		SongInfo getInfo() {
			SongInfo si;
			si.title = fields[0]->text;
			si.composer = fields[1]->text;
			si.format = fields[2]->text;
			return si;
		}

		std::shared_ptr<PlayerScreen::TextField> fields[3];

		PlayerScreen::TextField& operator[](int i) { return *fields[i]; }
		int size() { return 3; }

};
