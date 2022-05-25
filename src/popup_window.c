
qui_widget* popup_window_create_ui(popup_window_option options, popup_window_type type) {
	qui_widget* popup_ui = qui_setup();

	qui_widget* layout = qui_create_vertical_layout(popup_ui);
	{
		qui_widget* top_container = qui_create_flex_container(layout, 1);
		{
			top_container->margin_x = 10;
			top_container->margin_y = 10;

			qui_widget* text_layout = qui_create_horizontal_layout(top_container);
			{
				qui_widget* image_container = qui_create_fixed_container(text_layout, 50);
				{
					// Add image here
					(void)image_container;
				}

				qui_create_label(text_layout, "Test label Test label Test label Test label Test label Test label Test label", true);
				qui_create_fixed_container(text_layout, 10);
				qui_create_label(text_layout, "Poop Fortnite Poop Fortnite Poop Fortnite Poop Fortnite Poop Fortnite ", true);
			}
		}
		qui_widget* bottom_container = qui_create_fixed_container(layout, ITEMBAR_H);
		{
			qui_widget* button_layout = qui_create_horizontal_layout(bottom_container);
			{
				if (options & POPUP_BTN_OK) qui_create_button(button_layout, "Accept");
				if (options & POPUP_BTN_CANCEL) qui_create_button(button_layout, "Cancel");
			}		
		}
	}

	return popup_ui;
}

void popup_window_close(platform_window* window)
{
	// Enable parent window.
	if (window_registry.length > 1) 
	{
		platform_window* w = *(platform_window**)array_at(&window_registry, window_registry.length-2);
		w->enabled = true;
	}
}

void popup_window_show(char* title, char* text, popup_window_option options, popup_window_type type)
{
	log_assert(options != 0, "Popup window should have atleast 1 button");
	
	platform_window* parent = 0;
	
	// Disable parent window.
	if (window_registry.length) 
	{
		platform_window* w = *(platform_window**)array_at(&window_registry, window_registry.length-1);
		w->enabled = false;
		parent = w;
	}

	platform_open_window_ex(title, 400, 200, 400, 200, 400, 200, FLAGS_POPUP, 
		0, 0, popup_window_close, parent, popup_window_create_ui(options, type));
}