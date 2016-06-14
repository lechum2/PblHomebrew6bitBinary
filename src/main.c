// This is my first working modification of MyNameIsKir's Binary Watch watchface for the Pebble Smart Watch
// (https://github.com/MyNameIsKir/Pebble-Binary-Watch-C). Most of the code is hers; I just fumbled around
// with some modifications--namely, I changed it from an 8-bit-per-line display to a 6-bit used a different,
// larger font added the old-school green-on-black "homebrew" color scheme and removed the date window.
// Many thanks to MyNameIsKir for her help and her work.

#include <pebble.h>
#include <stdio.h>

Window *s_main_window;
TextLayer *text_layer;
//The current binary time
char binary_time[36], binary_hours[9], binary_minutes[9], binary_days[9], binary_months[9];

//Convert integer into a 8 digit binary number
void getBin(int num, char *str)
{
  *(str+8) = '\0';
  int mask = 0x20 << 1;
  while(mask >>= 1)
    *str++ = !!(mask & num) + '0';
}

//Update the time and print it to the screen
static void update_time() {
  //Get the time and split it into decimal variables (hour, minute, time)
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  int hours = tick_time->tm_hour, minutes = tick_time->tm_min;
  int days = tick_time->tm_mday, months = tick_time->tm_mon;

  //If needed, convert the hours to reflect a 12 hour clock. AM/PM is not displayed
  //  on this watchface at this time
  if(clock_is_24h_style() == false && hours > 12)
      hours = hours - 12;
  
  //Use the decimal times to reassign the global binary time variables
  getBin(hours, binary_hours);
  getBin(minutes, binary_minutes);
  getBin(days, binary_days);
  getBin(1 + months, binary_months);
  
  //Combine the binary time variables to create the full time string.
  //  strcpy also serves to clear the previous string assigned to binary_time
  //  and reassign it to the beginning of what will be the new string.
  strcpy(binary_time, binary_hours);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_minutes);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_days);
  strcat(binary_time, "\n");
  strcat(binary_time, binary_months);
  
  //Set the text layer to display the binary time, thereby printing it to the string.
  text_layer_set_text(text_layer, binary_time);
}

//Generate the main window
static void main_window_load(Window *window) {
  //Generate the text layer that will display the binary time
  text_layer = text_layer_create(GRect(0, -10, 144, 200));
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorGreen);
  text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Share_Tech_Mono_44)));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  //Add the text layer to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
   
  update_time(); 
}

//Destroy the main window
static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

//Calls the actions to occur every minute 
static void minute_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

//Initialize objects and processes
static void handle_init(void) {
  //Generate the main window
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  //Run the time updater every minute 
  tick_timer_service_subscribe(MINUTE_UNIT, minute_tick_handler);
}

//Deinitialize everything
static void handle_deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
