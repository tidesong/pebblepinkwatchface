#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_bluetooth_layer;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_bt_status_bitmap;
static BitmapLayer *s_bt_status_layer;
static GBitmap *s_bt_status_dc_bitmap;
static BitmapLayer *s_bt_status_dc_layer;

static void bt_handler(bool connected) {
  // Show current connection state
  if (connected) {
    text_layer_set_text(s_bluetooth_layer, "");
    layer_set_hidden((Layer *)s_bt_status_layer, false);
    layer_set_hidden((Layer *)s_bt_status_dc_layer, true);
  } else {
    text_layer_set_text(s_bluetooth_layer, "");
    layer_set_hidden((Layer *)s_bt_status_layer, true);
    layer_set_hidden((Layer *)s_bt_status_dc_layer, false);
  }
}
static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", new_state.charge_percent);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void update_date() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[80];
  strftime(s_buffer, sizeof(s_buffer), "%a %Y %b %d", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_date_layer, s_buffer);
}

static void main_window_load(Window *window) {
 // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //!!!!!!!!!!!!!!!!!!!!!!!Time layer!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(38, 32), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorFashionMagenta);
  //text_layer_set_text(s_time_layer, "00:00"); //tmp
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  //!!!!!!!!!!!!!!!!!!!!!!!!Date layer1!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  
    // Create the TextLayer with specific bounds
  s_date_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(88, 82), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorFashionMagenta);
  //text_layer_set_text(s_time_layer, "00:00"); //tmp
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Battery info!!!!!!!!!!!!!!!!!!!!!!!!!
  
   // Create output TextLayer
  s_battery_layer = text_layer_create(GRect(0, 2, bounds.size.w, 20));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorRoseVale);
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));

  // Get the current battery level
  battery_handler(battery_state_service_peek());
  
    
  //!!!!!!!!!!!!!!!!!!!!!!!!!Bluetooth icon !!!!!!!!!!!!!!!!!!!!!
  s_bt_status_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT_CON);
  s_bt_status_dc_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT_DISCON);

  s_bt_status_layer = bitmap_layer_create(GRect(0, 0, 20, 30));
  s_bt_status_dc_layer = bitmap_layer_create(GRect(0, 0, 20, 30));
  bitmap_layer_set_bitmap(s_bt_status_layer, s_bt_status_bitmap);
  bitmap_layer_set_bitmap(s_bt_status_dc_layer, s_bt_status_dc_bitmap);
#if defined(PBL_BW)
  bitmap_layer_set_compositing_mode(s_bt_status_layer, GCompOpAssign);
  bitmap_layer_set_compositing_mode(s_bt_status_dc_layer, GCompOpAssign);
#elif defined(PBL_COLOR)
  bitmap_layer_set_compositing_mode(s_bt_status_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(s_bt_status_dc_layer, GCompOpSet);
#endif
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bt_status_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bt_status_dc_layer));
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!!Bluetooth info!!!!!!!!!!!!!!!!!!!!!!
  // Create output TextLayer
  s_bluetooth_layer = text_layer_create(GRect(0, 0, bounds.size.w, 20));
  text_layer_set_text_alignment(s_bluetooth_layer, GTextAlignmentLeft);
  text_layer_set_background_color(s_bluetooth_layer, GColorClear);
  text_layer_set_text_color(s_bluetooth_layer, GColorWhite);
  text_layer_set_font(s_bluetooth_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(s_bluetooth_layer));

  // Show current connection state
#ifdef PBL_SDK_2
  bt_handler(bluetooth_connection_service_peek());
#elif PBL_SDK_3
  bt_handler(connection_service_peek_pebble_app_connection());
#endif
  
  //!!!!!!!!!!!!!!!!!!!!!!!!!Graphics Family Photo !!!!!!!!!!!!!!!!!!!!!
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FAMILYPIC);

  s_bitmap_layer = bitmap_layer_create(GRect(0, 100, bounds.size.w, 60));
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
#if defined(PBL_BW)
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpAssign);
#elif defined(PBL_COLOR)
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
#endif
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_bluetooth_layer);
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_bitmap);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Subscribe to the Battery State Service
  battery_state_service_subscribe(battery_handler);
  
    // Subscribe to Bluetooth updates
#ifdef PBL_SDK_2
  bluetooth_connection_service_subscribe(bt_handler);
#elif PBL_SDK_3
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bt_handler
  });
#endif
  
  // Make sure the time is displayed from the start
  update_time();
  update_date();
  
  
  
  window_set_background_color(s_main_window, GColorBlack);
}


static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}





int main(void) {
  init();
  app_event_loop();
  deinit();
}

