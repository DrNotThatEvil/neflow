#include "nf_menu/nf_profiles_screen.h"

void nf_profiles_render(_nf_menu_t *menu_state, ssd1306_t *disp_ptr,
                        void *extra_data)
{
    char str[20];
    _nf_profiles_menu_state_t *profiles_state =
        ((_nf_profiles_menu_state_t *)extra_data);
    _nf_profile_t *selected_profile =
        (&profiles_state->_profile_state
              ->_profiles[profiles_state->selected_profile]);

    if (!profiles_state->selected)
    {

        draw_prev_section(disp_ptr, "Main menu");
        sprintf(str, "Profile %d\0", (profiles_state->selected_profile + 1));

        draw_next_arrow(disp_ptr);
        ssd1306_draw_string(disp_ptr, 5, 25, 2, str);

        if (!selected_profile->initialized)
        {
            sprintf(str, "not configured!\0");
            ssd1306_draw_string(disp_ptr, 5, 40, 1, str);
        }
        return;
    }

    draw_prev_section(disp_ptr, "Profiles");

    if (profiles_state->selected_option == 0)
    {
        sprintf(str, "Run", (profiles_state->selected_profile + 1));
    }
    else
    {
        sprintf(str, "Edit", (profiles_state->selected_profile + 1));
    }

    draw_next_arrow(disp_ptr);
    ssd1306_draw_string(disp_ptr, 5, 25, 2, str);

    if (!selected_profile->initialized)
    {
        sprintf(str, "not configured!");
        ssd1306_draw_string(disp_ptr, 5, 40, 1, str);
    }
}

void nf_profiles_btn_handler(_nf_menu_t *menu_state, uint btn, bool repeat,
                             void *extra_data)
{
    _nf_profiles_menu_state_t *profiles_state =
        ((_nf_profiles_menu_state_t *)extra_data);

    if (!profiles_state->selected)
    {
        if (btn == 0)
        {
            nf_menu_change_screen(menu_state, MAIN_MENU_SCREEN_ID);
            return;
        }

        if (btn == 1)
        {
            profiles_state->selected_profile =
                ((profiles_state->selected_profile) + 1) % 2;
            return;
        }

        if (btn == 2)
        {
            profiles_state->selected = true;
            return;
        }
    }

    if (profiles_state->selected)
    {
        if (btn == 0)
        {
            profiles_state->selected = false;
            return;
        }

        if (btn == 1)
        {
            profiles_state->selected_option =
                ((profiles_state->selected_option) + 1) % 2;
            return;
        }

        if (btn == 2)
        {
            _nf_profile_t *_profile =
                (&profiles_state->_profile_state
                      ->_profiles[profiles_state->selected_profile]);
            if (profiles_state->selected_option == 1)
            {
                tone(menu_state->_tonegen, NOTE_A4, 100);
                nf_menu_change_screen_with_data(
                    menu_state, PROFILE_EDIT_SCREEN_ID, (void *)_profile);
                return;
            }

            if (_profile->initialized)
            {
                tone(menu_state->_tonegen, NOTE_A4, 100);
                nf_set_profile(menu_state, (void *)_profile);
                nf_menu_change_screen_with_data(
                    menu_state, PROFILE_RUN_SCREEN_ID, (void *)_profile);
                // nf_menu_change_screen_with_data(menu_state,
                // PROFILE_EDIT_SCREEN_ID, (void*)
                // (&profiles_state->_profile_state->_profiles[profiles_state->selected_profile]));
            }
            else
            {
                tone(menu_state->_tonegen, NOTE_C3, 100);
            }
            return;
        }
    }
}

void nf_profiles_menu_init(_nf_menu_t *_menu_state,
                           _nf_profile_state_t *_profile_state)
{
    // get start of the screens.
    _nf_menu_screen_t **_menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t *profiles_screen =
        (_nf_menu_screen_t *)malloc(sizeof(_nf_menu_screen_t));

    profiles_screen->id = PROFILES_SCREEN_ID;
    profiles_screen->autoclear = true;
    profiles_screen->extra_data = malloc(sizeof(_nf_profiles_menu_state_t));

    _nf_profiles_menu_state_t *profile_menu_state =
        (_nf_profiles_menu_state_t *)profiles_screen->extra_data;
    profile_menu_state->selected_profile = 0;
    profile_menu_state->selected_option = 0;
    profile_menu_state->selected = false;
    profile_menu_state->_profile_state = _profile_state;

    _nf_menu_screen_fn_ptrs_t profiles_screen_fns = {
        .on_render = nf_profiles_render,
        .on_btn = nf_profiles_btn_handler,
        .on_active = NULL,
    };

    profiles_screen->fnptrs = profiles_screen_fns;
    profiles_screen->next = NULL;

    nf_menu_add_screen(_menu_screens_ptr, profiles_screen);
}
