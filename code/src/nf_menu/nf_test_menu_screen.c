#include "nf_menu/nf_test_menu_screen.h"

bool testing_countdown(struct repeating_timer *t_ptr)
{
    _nf_test_menu_state_t* test_state = ((_nf_test_menu_state_t*) t_ptr->user_data);
    test_state->countdown_time = test_state->countdown_time - 1;

    bool t_cancel = test_state->cancel;

    if(test_state->countdown_time == 0 || t_cancel) {
        //tone(test_state->_tonegen, NOTE_E4, 100);

        if(!test_state->cancel) {
            note_t CONFIRM_DONE[] = {
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {NOTE_A7, 64},
                {NOTE_A6, 64},
                {MELODY_END, 0},
            };

            melody(test_state->_tonegen, CONFIRM_DONE, 5);
        } else {
            note_t REJECT[] = {
                {NOTE_CM1, 128},
                {REST, 128},
                {NOTE_CM1, 128},
                {REST, 128},
                {NOTE_CM1, 128},
                {REST, 128},
                {NOTE_CM1, 128},
                {REST, 128},
                {REST, 8},
                {MELODY_END, 0},
            };

            melody(test_state->_tonegen, REJECT, 5);
        }
        
        const uint INDEX_TO_PIN[] = { NF_SSR0_PIN, NF_SSR1_PIN, NF_FAN0_PIN, NF_FAN1_PIN };
        gpio_put(INDEX_TO_PIN[test_state->selected_option], 0);

        test_state->testing = false;
        test_state->cancel = false;
    } else {

        tone(test_state->_tonegen, NOTE_C4, 100);
    }

    return (test_state->countdown_time > 0 && !t_cancel);
}

void nf_test_render(_nf_menu_t* menu_state, ssd1306_t* disp_ptr, void* extra_data)
{
    char str[20];
    char str2[20];
    _nf_test_menu_state_t* test_state = ((_nf_test_menu_state_t*) extra_data);

    if(test_state->testing) {
        sprintf(str2, "Countdown: %d", test_state->countdown_time);
        ssd1306_draw_string(disp_ptr, 5, 15, 1, str2);
    }

    const char* options[] = { "SSR0", "SSR1", "FAN0", "FAN1" };
    sprintf(str, "%s: %s", (test_state->testing ? "Testing: " : "Test: "), options[test_state->selected_option]);
    ssd1306_draw_string(disp_ptr, 5, 35 - (test_state->testing ? 0 : 10), 1, str);

    draw_next_arrow(disp_ptr);
}

void nf_test_btn_handler(_nf_menu_t* menu_state, uint btn, bool repeat, void* extra_data)
{
    _nf_test_menu_state_t* test_state = ((_nf_test_menu_state_t*) extra_data);

    if(!test_state->testing) {
        if(btn == 0) {
            nf_menu_change_screen(menu_state, 0);
            return;
        }

        if(btn == 1) {
            test_state->selected_option = ((test_state->selected_option) + 1) % 4;
            return;
        } 

        if(btn == 2) {
            test_state->testing = true;
            test_state->cancel = false;
            test_state->countdown_time = 60;

            const uint INDEX_TO_PIN[] = { NF_SSR0_PIN, NF_SSR1_PIN, NF_FAN0_PIN, NF_FAN1_PIN };
            gpio_put(INDEX_TO_PIN[test_state->selected_option], 1);
            
            add_repeating_timer_ms(-900, testing_countdown, test_state, test_state->timer);
            return;
        }
    }

    if(test_state->testing) {
        if(btn == 0) {
            test_state->cancel = true;
            return;
        }

        if(btn == 1) {
            test_state->selected_option = ((test_state->selected_option) + 1) % 3;
            return;
        } 

        if(btn == 2) {
            //nf_menu_change_screen(menu_state, 2);
            return;
        }
    }
}

void nf_test_menu_init(_nf_menu_t* _menu_state)
{
    // get start of the screens.
    _nf_menu_screen_t** _menu_screens_ptr = &_menu_state->menu_screens;
    _nf_menu_screen_t* test_screen = (_nf_menu_screen_t*) malloc(sizeof(_nf_menu_screen_t));


    test_screen->id = TEST_SCREEN_ID;
    test_screen->extra_data = malloc(sizeof(_nf_test_menu_state_t));

    _nf_test_menu_state_t* test_menu_state = (_nf_test_menu_state_t*) test_screen->extra_data;
    test_menu_state->timer = (struct repeating_timer*) malloc(sizeof(struct repeating_timer));
    test_menu_state->selected_option = 0;
    test_menu_state->testing = false;
    test_menu_state->cancel = false;
    test_menu_state->_tonegen = _menu_state->_tonegen;
    test_menu_state->countdown_time = 0;

    _nf_menu_screen_fn_ptrs_t test_screen_fns = {
        .on_render = nf_test_render,
        .on_btn = nf_test_btn_handler
    };
    
    test_screen->fnptrs = test_screen_fns;
    test_screen->next = NULL;

    nf_menu_add_screen(_menu_screens_ptr, test_screen);
}

