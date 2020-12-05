#ifndef _INPUT_H_
#define _INPUT_H_

enum key_map {
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_OK,
	KEY_BACK,
	KEY_PAUSE,
	KEY_COUNT
};

struct input {
	int map[KEY_COUNT];
	int key[KEY_COUNT];
};

int input_init(void);
void input_poll(struct input *input);
void input_set_default_map(struct input *input);

#endif /* _INPUT_H_ */
