# 2023-10-30: Editing and View menu flow done.
Added the editing flow some bugs still remain but it's verry nice (if i do say so myself)
i also reworked the button events so you can have it repeat which is so much nicer for editing the values.

Next thing will be saving and loading the profile from flash storage
after you should probably finish the pcb design, while you wait on the order you can cleanup the menu code.

By the way when designing the pcb please take some measurements of the toaster oven you ordered
it's gonna need to fit in it. 

But as of now even tho the menu code can do with some housekeeping, i'm really happy with how
this is turning out so far, it's still a lot of stuff to do on the code (temp control and reflow etc)
but it's fun getting to play with the breadboard and designing the pcb along side it.

# 2023-10-27: Basic menu flow design and implementation
Added the basic menu system, please see drawio diagrams for flow.
Added basic profiles (no flash yet but thats gonna go after editing and saving)

# TODO:
* ~~Add the editing flow for the profile~~ (2023-10-30: DONE!)
* Add loading the profiles from flash storage.
* Restructure the nf_menu.c functions to reduce the duplicate code.
* Add 'confirm screen' after the 'Run' menu option to make sure it's not being called on accident (use different button maybe?)
* 

# Bugs
The first time can go negative while editing? Do idea why but if you don't do that it's fine to tired to figure it out now.