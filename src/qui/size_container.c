void _qui_update_sized_container(qui_widget* el) {
	
}


void _qui_render_sized_container(qui_widget* el) {
	renderer->render_rectangle(el->x, el->y, el->width, el->height, rgb(0,0,255));
}

qui_widget* qui_create_size_container(qui_widget* qui, u8 dir)
{
	qui_widget* wg = _qui_create_empty_widget(qui);
	wg->data = mem_alloc(sizeof(qui_size_container));
	wg->data->direction = dir;
	wg->type = WIDGET_SIZE_CONTAINER;
	if (qui && qui->type == WIDGET_VERTICAL_LAYOUT) {
		
	}
	else {
		log_assert(0, "Sized container must be placed in vertical or horizontal layout");
	}
	return wg;
}