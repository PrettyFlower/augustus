#include "generic_button.h"

#include "graphics/lang_text.h"

static int get_button(const mouse *m, int x, int y, generic_button *buttons, int num_buttons)
{
    for (int i = 0; i < num_buttons; i++) {
        if (x + buttons[i].x <= m->x &&
            x + buttons[i].x + buttons[i].width > m->x &&
            y + buttons[i].y <= m->y &&
            y + buttons[i].y + buttons[i].height > m->y) {
            return i + 1;
        }
    }
    return 0;
}

static int get_button_min(const mouse* m, int x, int y, generic_button* buttons, int num_buttons, int minimum_button)
{
    for (int i = minimum_button; i < num_buttons; i++) {
        if (x + buttons[i].x <= m->x &&
            x + buttons[i].x + buttons[i].width > m->x&&
            y + buttons[i].y <= m->y &&
            y + buttons[i].y + buttons[i].height > m->y) {
            return i + 1;
        }
    }
    return 0;
}

void generic_button_border_draw(const generic_button *button, int has_focus)
{
    button_border_draw(button->x, button->y, button->width, button->height, has_focus);
}

void generic_button_draw_text_centered(int group, int number, const generic_button *button, font_t font)
{
    int y = button->y + button->height / 2 - 5;
    lang_text_draw_centered(group, number, button->x, y, button->width, FONT_NORMAL_BLACK);
}

int generic_buttons_handle_mouse(const mouse *m, int x, int y, generic_button *buttons, int num_buttons, int *focus_button_id)
{
    int button_id = get_button(m, x, y, buttons, num_buttons);
    if (focus_button_id) {
        *focus_button_id = button_id;
    }
    if (!button_id) {
        return 0;
    }
    generic_button *button = &buttons[button_id - 1];
    if (m->left.went_up) {
        button->left_click_handler(button->parameter1, button->parameter2);
        return button->left_click_handler != button_none;
    } else if (m->right.went_up) {
        button->right_click_handler(button->parameter1, button->parameter2);
        return button->right_click_handler != button_none;
    } else {
        return 0;
    }
}

int generic_buttons_min_handle_mouse(const mouse* m, int x, int y, generic_button* buttons, int num_buttons, int* focus_button_id, int minimum_button)
{
    int button_id = get_button_min(m, x, y, buttons, num_buttons,minimum_button);
    if (focus_button_id) {
        *focus_button_id = button_id;
    }
    if (!button_id) {
        return 0;
    }
    generic_button* button = &buttons[button_id - 1];
    if (m->left.went_up) {
        button->left_click_handler(button->parameter1, button->parameter2);
    }
    else if (m->right.went_up) {
        button->right_click_handler(button->parameter1, button->parameter2);
    }
    else {
        return 0;
    }
    return button_id;
}



