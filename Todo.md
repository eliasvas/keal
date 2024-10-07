### Todo

- Fix Rendering abstraction
	- make it more like [sokol_gfx](https://floooh.github.io/2017/07/29/sokol-gfx-tour.html) (add shader_bindings)
	- see wether we can put all framebuffer info in one [pointer](https://stackoverflow.com/questions/15089703/how-to-get-the-textures-attached-to-a-framebuffer)
	- Find a good way to handle uniforms, currently not very good (we copy the BAD OpenGL API pretty much)

- Look into Rope Physics (needed for guillotine mechanic)
	- how can we implement this / what changes are needed in the physics engine

- Physics Engine
	- we might want to do some spatial partitioning (BSP?)

- For some reason the counter of invincibility takes way longer than specified

- Construct the engine's drawcall API (on top of *ogl*)
	- Probably Command buffer [based](https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1)
	- Material Systems
	- Multithreading