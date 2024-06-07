#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOTES 99
#define MAX_LENGTH 99

// Define a structure to hold each note and its checkbox state
typedef struct {
    char text[MAX_LENGTH];
    int checked;
} Note;

// Function prototypes
WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void display_list(Note notes[], int size, int selected);
void add_note(Note notes[], int *qnotes);
void delete_note(Note notes[], int *qnotes, int *selected);
void toggle_checkbox(Note notes[], int selected);
void handle_input(Note notes[], int *qnotes, int *selected);

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    int selected = 0;
    int qnotes = 0;
    Note notes[MAX_NOTES];

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    // Main loop
    while (1) {
        erase();
        display_list(notes, qnotes, selected);
        mvprintw(LINES - 1, (COLS - 42) / 2, "[A] Add note [D] Delete note [ENTER] Toggle [F1] Quit");
        refresh();
        handle_input(notes, &qnotes, &selected);
    }

    // Cleanup and end ncurses mode
    endwin();
    return 0;
}

void display_list(Note notes[], int size, int selected) {
    for (int i = 0; i < size; ++i) {
        if (i == selected) {
            attron(A_REVERSE);
        }
        mvprintw(i, 0, "[%c] %s", notes[i].checked ? 'x' : ' ', notes[i].text);
        if (i == selected) {
            attroff(A_REVERSE);
        }
    }
    refresh();
}

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wrefresh(local_win);
    return local_win;
}

void destroy_win(WINDOW *local_win) {
    werase(local_win);
    wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(local_win);
    delwin(local_win);
}

void add_note(Note notes[], int *qnotes) {
    if (*qnotes < MAX_NOTES) {
        int height = 3, width = 80, starty = (LINES - height) / 2, startx = (COLS - width) / 2;
        WINDOW *my_win = create_newwin(height, width, starty, startx);
        echo();
        wmove(my_win, 1, 4);
        wgetnstr(my_win, notes[*qnotes].text, MAX_LENGTH - 1);
        notes[*qnotes].checked = 0; // Initialize checkbox as unchecked
        (*qnotes)++;
        destroy_win(my_win);
        curs_set(0);
        noecho();
    }
}

void delete_note(Note notes[], int *qnotes, int *selected) {
    if (*qnotes > 0) {
        for (int i = *selected; i < *qnotes - 1; i++) {
            notes[i] = notes[i + 1];
        }
        (*qnotes)--;
        if (*selected >= *qnotes && *selected > 0) {
            (*selected)--;
        }
    }
}

void toggle_checkbox(Note notes[], int selected) {
    notes[selected].checked = !notes[selected].checked;
}

void handle_input(Note notes[], int *qnotes, int *selected) {
    int ch = getch();
    switch (ch) {
        case 'A':
        case 'a':
            add_note(notes, qnotes);
            break;
        case 'D':
        case 'd':
            delete_note(notes, qnotes, selected);
            break;
        case 10: // ENTER key
            toggle_checkbox(notes, *selected);
            break;
        case KEY_UP:
            if (*selected > 0) {
                (*selected)--;
            }
            break;
        case KEY_DOWN:
            if (*selected < *qnotes - 1) {
                (*selected)++;
            }
            break;
        case KEY_F(1):
            endwin();
            exit(0);
        default:
            break;
    }
}
