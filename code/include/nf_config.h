#ifndef nf_config_h
#define nf_config_h

#define NF_BTN0_PIN 4
#define NF_BTN1_PIN 5
#define NF_BTN2_PIN 6
#define NF_BTN3_PIN 7

#define NF_MENU_UP_BTN NF_BTN0_PIN
#define NF_MENU_DOWN_BTN NF_BTN1_PIN
#define NF_MENU_SELECT_BTN NF_BTN2_PIN
#define NF_MENU_BACK_BTN NF_BTN3_PIN

// State of the reflow oven.
enum NF_STATE
{
    NORMAL,
    CALIBATION,
    REFLOW
};

enum NF_REFLOW_STATE
{
    PREHEAT,
    SOAK,
    REFLOW,
    COOLDOWN
};


#endif