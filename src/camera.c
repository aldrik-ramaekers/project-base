/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

void camera_apply_transformations(platform_window *window, camera *camera)
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