### Todo

- Physics Engine
	- also layers would be great (any layer only interactible with >= layers), better yet layer + layer_flags (static is 0?)
	- Look into Rope Physics (needed for guillotine mechanic)

- For some reason the counter of invincibility takes way longer than specified

- Some steps to 'make' the game playable
	- the GUI is fine, just add a boink sound when buttons are pressed
	- instead of complex dungeons, each levels should be a square dungeon and have 0..4 enemies inside + closed door
	- when enemies are defeated open the door and make a new level
	- only weapon will be the guillotine (so ref. Rope Physics), it should bounce through walls
	- more enemies, bats will be fast and always follow, skeletons slow
	- there will be dash (maybe it will work through physics engine layers) that makes player invincible, reset via timer