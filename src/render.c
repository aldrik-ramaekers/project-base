/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

render_driver_id current_render_driver()
{
	if (renderer == &render_gl_driver) return DRIVER_GL;
	if (renderer == &render_cpu_driver) return DRIVER_CPU;
	return DRIVER_NONE;
}

void set_render_driver(render_driver_id driver)
{
    if (driver == DRIVER_GL) renderer = &render_gl_driver;
    if (driver == DRIVER_CPU) renderer = &render_cpu_driver;
}