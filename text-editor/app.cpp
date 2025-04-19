#include <ncurses.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <stack>
namespace fs = std::filesystem;

const int SIDEBAR_WIDTH = 25;

std::stack<std::vector<std::string>> undoStack;
std::stack<std::vector<std::string>> redoStack;

std::string bold(const std::string& text) {
    return "**" + text + "**";
}

std::string italic(const std::string& text) {
    return "*" + text + "*";
}

std::string underline(const std::string& text) {
    return "__" + text + "__";
}

std::string formatText(const std::string& text, const std::string& formatType) {
    if (formatType == "bold") {
        return bold(text);
    } else if (formatType == "italic") {
        return italic(text);
    } else if (formatType == "underline") {
        return underline(text);
    }
    return text;
}

void list_directory_tree(const fs::path& path, std::vector<std::string>& files, int depth = 0) {
    if (!fs::exists(path) || !fs::is_directory(path)) return;

    for (const auto& entry : fs::directory_iterator(path)) {
        std::string indent(depth * 2, ' ');
        std::string name = entry.path().filename().string();

        if (entry.is_directory()) {
            files.push_back(indent + "(D) " + name);
            list_directory_tree(entry.path(), files, depth + 1);
        } else {
            files.push_back(indent + "(F) " + name);
        }
    }
}

void draw_separator() {
    for (int i = 1; i < LINES - 1; i++) {
        mvaddch(i, SIDEBAR_WIDTH-3, ACS_VLINE);
    }
}

void draw_header(bool selecting) {
    attron(A_REVERSE);
    mvhline(0, 0, ' ', COLS + 3);
    mvprintw(0, (COLS - 25) / 2, " My-Text-Editor ");
    std::string mode_status = selecting ? "Selection Mode" : "Write Mode";
    attron(COLOR_PAIR(1));
    mvprintw(0, COLS - mode_status.size() - 2, mode_status.c_str());
    attroff(A_REVERSE);
    attroff(COLOR_PAIR(1));
}

void draw_footer() {
    int y = LINES - 1;
    attron(A_REVERSE);
    mvhline(y-2, 0, ' ', COLS+3);
    mvprintw(y-2, 1, " CTRL+X: Exit ");
    mvprintw(y-2, 15, "| CTRL+Z: Undo ");
    mvprintw(y-2, 30, "| CTRL+Y: Redo ");
    mvhline(y-1, 0, ' ', COLS+3);
    mvprintw(y-1, 1, " CTRL+S: Selection Mode (use arrow to select, selected text can be formatted in md format) ");
    mvhline(y, 0, ' ', COLS+3);
    mvprintw(y, 1, " Available format: CTRL+U (Underline), CTRL+I (Italic), CTRL+B (Bold) ");
    attroff(A_REVERSE);
}

std::vector<std::string> list_files() {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(".")) {
        files.push_back(entry.path().filename().string());
    }
    return files;
}

void draw_sidebar(const std::vector<std::string>& files) {
    for (int i = 0; i < files.size() && i < LINES - 2; ++i) {
        mvprintw(i + 2, 1, "%-*s", SIDEBAR_WIDTH - 1, files[i].c_str());
    }
}

bool is_selected(int x, int y, int cx, int cy, int sx, int sy, bool selecting) {
    if (!selecting) return false;

    if (sy > cy || (sy == cy && sx > cx)) {
        std::swap(sx, cx);
        std::swap(sy, cy);
    }

    if (y < sy || y > cy) return false;
    if (y == sy && y == cy) return x >= sx && x < cx;
    if (y == sy) return x >= sx;
    if (y == cy) return x < cx;
    return true;
}

void draw_editor(const std::vector<std::string>& lines, int cx, int cy, int sel_cx, int sel_cy, bool selecting) {
    for (int y = 0; y < lines.size() && y < LINES - 2; ++y) {
        for (int x = 0; x < lines[y].size(); ++x) {
            if (is_selected(x, y, cx, cy, sel_cx, sel_cy, selecting)) {
                attron(A_REVERSE);
                mvaddch(y + 2, x + SIDEBAR_WIDTH, lines[y][x]);
                attroff(A_REVERSE);
            } else {
                mvaddch(y + 2, x + SIDEBAR_WIDTH, lines[y][x]);
            }
        }
    }
}

int main() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(1);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    std::vector<std::string> files = list_files();
    std::vector<std::string> lines(1, "");
    auto save_state = [&]() {
        undoStack.push(lines);
        while (!redoStack.empty()) redoStack.pop();
    };

    int cx = 0, cy = 0;
    bool running = true;
    bool selecting = false;
    int sel_cx = -1, sel_cy = -1;

    while (running) {
        clear();
        draw_header(selecting);
        std::vector<std::string> files;
        list_directory_tree(".", files);
        draw_sidebar(files);
        draw_separator();
        draw_editor(lines, cx, cy, sel_cx, sel_cy, selecting);
        draw_footer();
        move(cy + 2, cx + SIDEBAR_WIDTH);
        refresh();

        int ch = getch();
        if (ch == 24) {
            running = false;
        } else if (ch == 26) {
            selecting = false;
            sel_cx = sel_cy = -1;

            if (!undoStack.empty()) {
                redoStack.push(lines);
                lines = undoStack.top();
                undoStack.pop();
                cy = std::min(cy, (int)lines.size() - 1);
                cx = std::min(cx, (int)lines[cy].size());
            }
        } else if (ch == 25) {
            selecting = false;
            sel_cx = sel_cy = -1;

            if (!redoStack.empty()) {
                undoStack.push(lines);
                lines = redoStack.top();
                redoStack.pop();
                cy = std::min(cy, (int)lines.size() - 1);
                cx = std::min(cx, (int)lines[cy].size());
            }
        } else if (ch == 10) {
            save_state();
            lines.insert(lines.begin() + cy + 1, "");
            cy++;
            cx = 0;
        } else if (ch == 19) {
            if (!selecting) {
                selecting = true;
                sel_cx = cx;
                sel_cy = cy;
            } else {
                selecting = false;
                sel_cx = sel_cy = -1;
            }
        } else if (ch == 2 || ch == 9 || ch == 21) {
            if (!selecting) continue;

            int start_x = sel_cx, start_y = sel_cy;
            int end_x = cx, end_y = cy;
            if (end_y < start_y || (end_y == start_y && end_x < start_x)) {
                std::swap(start_x, end_x);
                std::swap(start_y, end_y);
            }

            std::string selected_text;
            for (int y = start_y; y <= end_y && y < lines.size(); ++y) {
                int from = (y == start_y ? start_x : 0);
                int to = (y == end_y ? end_x : lines[y].size());
                if (from >= lines[y].size()) continue;
                to = std::min(to, (int)lines[y].size());
                selected_text += lines[y].substr(from, to - from);
                if (y != end_y) selected_text += "\n";
            }

            if (ch == 2) {
                selected_text = formatText(selected_text, "bold");
            } else if (ch == 9) {
                selected_text = formatText(selected_text, "italic");
            } else if (ch == 21) {
                selected_text = formatText(selected_text, "underline");
            }

            for (int y = start_y; y <= end_y && y < lines.size(); ++y) {
                int from = (y == start_y ? start_x : 0);
                int to = (y == end_y ? end_x : lines[y].size());
                if (from >= lines[y].size()) continue;
                lines[y].replace(from, to - from, selected_text);
            }

            selecting = false;
            sel_cx = sel_cy = -1;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (cx > 0) {
                save_state();
                lines[cy].erase(cx - 1, 1);
                cx--;
            } else if (cy > 0) {
                save_state();
                cx = lines[cy - 1].size();
                lines[cy - 1] += lines[cy];
                lines.erase(lines.begin() + cy);
                cy--;
            }
        } else if (ch == KEY_LEFT && cx > 0) {
            cx--;
        } else if (ch == KEY_RIGHT && cx < lines[cy].size()) {
            cx++;
        } else if (ch == KEY_UP && cy > 0) {
            cy--;
            if (cx > lines[cy].size()) cx = lines[cy].size();
        } else if (ch == KEY_DOWN && cy + 1 < lines.size()) {
            cy++;
            if (cx > lines[cy].size()) cx = lines[cy].size();
        } else if (isprint(ch)) {
            selecting = false;
            sel_cx = sel_cy = -1;
            save_state();
            lines[cy].insert(cx, 1, (char)ch);
            cx++;
        }
    }

    endwin();
    return 0;
}
