/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void _camera_apply_transformations(platform_window *window, camera *camera)
{
	s32 x = (window->width/2)+(camera->x);
	s32 y =  (window->height/2)+(camera->y);
	IMP_glTranslatef(x, y, 0.0f);
	IMP_glRotatef(camera->rotation, 0.0f, 0.0f, 1.0f);
	IMP_glTranslatef(-x, -y, 0.0f);
	
	IMP_glMatrixMode(GL_PROJECTION);
	IMP_glLoadIdentity();
    
    IMP_glOrtho(camera->x, window->width+camera->x, 
            window->height+camera->y, camera->y, -100, 100);
    
	IMP_glMatrixMode(GL_MODELVIEW);
}

vec4 camera_get_target_rectangle(platform_window *window)
{
	int w = window->width;
	int h = window->height;

	double targetRatio = 16.0 / 9.0;
	double ratio = (double)w / h;
	if (ratio < targetRatio) // Too much height
	{
		int newHeight = (int)(w / targetRatio);
		int newY = (h - newHeight) / 2;
		return (vec4){0, newY, w, newHeight};
	}
	else if (ratio > targetRatio) // Too much width
	{
		int newWidth = (int)(h * targetRatio);
		int newX = (w - newWidth) / 2;
		return (vec4){newX, 0, newWidth, h};
	}

	return (vec4){0,0,w,h};
}