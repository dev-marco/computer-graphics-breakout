#ifndef SRC_BREAKOUT_STAGE_H_
#define SRC_BREAKOUT_STAGE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <unordered_map>
#include <random>
#include <chrono>
#include "brick.h"
#include "ball.h"
#include "../engine/window.h"

namespace Breakout {

    class Stage {

        Window &window;
        std::unordered_set<Brick *> can_destroy, cannot_destroy;

        static inline bool not_space (int c) {
        	return !isspace(c);
        }

        static inline std::string &ltrim (std::string &s) {
        	s.erase(s.begin(), find_if(s.begin(), s.end(), not_space));
        	return s;
        }

        static inline std::string &rtrim (std::string &s) {
        	s.erase(find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
        	return s;
        }

        static inline std::string &trim (std::string &s) {
        	return ltrim(rtrim(s));
        }

        static std::string nextLine (
        	std::istream &in,
            bool &ok
        ) {
        	std::string line = "";
        	do {
        		std::getline(in, line);
        		line = line.substr(0, line.find_first_of('#'));
        	} while (!trim(line).size() && in.good());

            ok = line.size() > 0;

        	return line;
        }

        static Brick *createBrickByID (Window &window, unsigned id, double x, double y) {

            std::default_random_engine gen(glfwGetTime() * 1000000);
            std::uniform_int_distribution<int> rgb(0, 255);
            std::uniform_real_distribution<double> alpha(0.3, 1.0);
            BackgroundColor *bg;

            switch (id & 15) {
                case 0x1: bg = new BackgroundColor(Color::rgba(205, 0, 0, 1.0)); break;
                case 0x2: bg = new BackgroundColor(Color::rgba(34, 139, 34, 1.0)); break;
                case 0x3: bg = new BackgroundColor(Color::rgba(30, 144, 255, 1.0)); break;
                case 0x4: bg = new BackgroundColor(Color::rgba(255, 215, 0, 1.0)); break;
                case 0x5: bg = new BackgroundColor(Color::rgba(240, 240, 240, 1.0)); break;
                case 0x6: bg = new BackgroundColor(Color::rgba(148, 0, 211, 1.0)); break;
                case 0x7: bg = new BackgroundColor(Color::rgba(0, 250, 154, 1.0)); break;
                case 0x8: bg = new BackgroundColor(Color::rgba(255, 69, 0, 1.0)); break;
                case 0x9: bg = new BackgroundColor(Color::rgba(238, 197, 145, 1.0)); break;
                default: bg = new BackgroundColor(Color::rgba(rgb(gen), rgb(gen), rgb(gen), alpha(gen)));
            }

            id >>= 4;

            return new Brick(window, { x, y, 4.0 }, bg);
        }

    public:

        static constexpr double DefaultVerticalSpace = 0.01, DefaultHorizontalSpace = 0.01;

        Stage (Window &_window, const std::string &file) : window(_window) {

            bool ok;
            std::ifstream input(file, std::ios::in);
            std::string block;
            std::stringstream ss(Stage::nextLine(input, ok));
            double max_speed, min_speed, x, y = 0.9 - (Stage::DefaultVerticalSpace / 2.0) - Brick::DefaultHeight;

            ss >> max_speed;

            ss.str(Stage::nextLine(input, ok));

            ss.seekg(0) >> min_speed;

            for (std::string line = Stage::nextLine(input, ok); ok; line = Stage::nextLine(input, ok)) {

                x = -1.0 + (Stage::DefaultHorizontalSpace / 2.0);

                ss.str(line);
                ss.seekg(0);

                while (ss.good()) {

                    ss >> block;

                    if (block[0] != '-') {
                        this->addBrick(std::stoul(block, nullptr, 16), x, y);
                    }
                    x += Brick::DefaultWidth + Stage::DefaultHorizontalSpace;
                }
                y -= Brick::DefaultHeight + Stage::DefaultVerticalSpace;
            }

            window.addObject(new Ball(max_speed, min_speed));
        }

        void addBrick (unsigned id, double x, double y) {

            Brick *brick = Stage::createBrickByID(this->window, id, x, y);

            if (brick) {
                if (brick->isDestructible()) {
                    this->can_destroy.insert(brick);
                } else {
                    this->cannot_destroy.insert(brick);
                }
                this->window.addObject(brick);
            }
        }

        inline Window &getWindow (void) const { return this->window; }

    };

}

#endif
