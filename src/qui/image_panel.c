
void _qui_update_image_panel(qui_state* main_state, qui_widget* el) {
	qui_image_panel* data = (qui_image_panel*)el->data;

	el->x = el->parent->x;
	el->y = el->parent->y;

	// Fit image into parent while ratio resolution.
	if (el->parent->width <= el->parent->height)
	{
		float ratio = data->image->height / data->image->width;
		el->width = el->parent->width;
		el->height = el->width * ratio;
	}
	else
	{
		float ratio = data->image->width / data->image->height;
		el->height = el->parent->height;
		el->width = el->height * ratio;
	}
}

void _qui_render_image_panel(qui_state* main_state, qui_widget* el) {
	qui_image_panel* data = (qui_image_panel*)el->data;
	renderer->render_image(data->image, el->x, el->y, el->width, el->height);
}

qui_widget* qui_create_image_panel(qui_widget* qui, image* img) {
	log_assert(img, "Image cannot be 0");

	qui_widget* el = _qui_create_empty_widget(qui);
	el->type = WIDGET_IMAGE_PANEL;
	el->width = 0;
	el->height = 0;

	qui_image_panel* data = (qui_image_panel*)_qui_allocate(qui, sizeof(qui_image_panel));
	data->image = img;
	el->data = (u8*)data;

	return el;
}