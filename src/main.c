#include <pebble.h>

#define ACCEL_MS     (500)
#define BUTTON_MS    (1000)
  
#define CMD_DENGEKI  0x0
#define CMD_SOUND    0x1
#define CMD_WALK     0x2

enum {
STATUS_KEY = 0,
MESSAGE_KEY = 1
};

Window *my_window;
TextLayer *text_layer;
TextLayer *button_layer;
TextLayer* up_layer;
TextLayer* down_layer;

char pszDisp[20];

static int count;

static void button_on_timer_callback(void* data);

void sendMessage(uint8_t cmd)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "sendMessage(c)");  
  DictionaryIterator* iter = NULL;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, STATUS_KEY, cmd);
  dict_write_end(iter);
  app_message_outbox_send();
    
}
static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "inbox_receive_callback");
  static const uint32_t const segments[] = { 200, 100, 200, 100, 200 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);
  
  Tuple* t = dict_read_first(iterator);
  while(t != NULL)
  {
    switch(t->key)
    {
      case CMD_DENGEKI:
        snprintf(pszDisp, 20, "DG :%d", (int)t->value->int32);
        text_layer_set_text(button_layer, pszDisp);
        APP_LOG(APP_LOG_LEVEL_INFO, pszDisp);
        break;
      case CMD_SOUND:
        snprintf(pszDisp, 20, "AG :%d", (int)t->value->int32);
        text_layer_set_text(button_layer, pszDisp);
        APP_LOG(APP_LOG_LEVEL_INFO, pszDisp);
    }
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void timer_callback(void* data)
{
  AccelData accel = (AccelData){.x = 0, .y = 0, .z = 0};
  accel_service_peek(&accel);
  
  snprintf(pszDisp, 20, "%d, %d, %d", accel.x, accel.y, accel.z);
  text_layer_set_text(text_layer, pszDisp);
  
  app_timer_register(ACCEL_MS, timer_callback, NULL);  
}

static void button_timer_callback(void* data)
{
  text_layer_set_text(button_layer, "");
  text_layer_set_text(up_layer, "");
  text_layer_set_text(down_layer, "");
  count++;
  if (count < 8)
  {
    app_timer_register(170, button_on_timer_callback, NULL);      
  }
  else
  {
    
  }
}

static void button_on_timer_callback(void* data)
{
  text_layer_set_text(button_layer, "DENGEKI!");  
  text_layer_set_text(up_layer, "***********");
  text_layer_set_text(down_layer, "***********");
  app_timer_register(330, button_timer_callback, NULL);  
}
static void select_click_handler(ClickRecognizerRef recognizer, void* context)
{
  text_layer_set_text(button_layer, "DENGEKI!");
  text_layer_set_text(up_layer, "***********");
  text_layer_set_text(down_layer, "***********");
  sendMessage(CMD_DENGEKI);
  count = 0;
  app_timer_register(330, button_timer_callback, NULL);  
}

static void up_click_handler(ClickRecognizerRef recognizer, void* context)
{
  text_layer_set_text(button_layer, "Up");
  sendMessage(CMD_WALK);
  app_timer_register(ACCEL_MS, button_timer_callback, NULL);  
}

static void down_click_handler(ClickRecognizerRef recognizer, void* context)
{
  text_layer_set_text(button_layer, "Down");
  sendMessage(CMD_SOUND);
  app_timer_register(ACCEL_MS, button_timer_callback, NULL);  
}

static void click_config_provider(void* context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window* window)
{
/*  
  text_layer = text_layer_create(GRect(0, 0, 144, 50));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
//  text_layer_set_text(text_layer, "0, 0, 0");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));
*/
  
  button_layer = text_layer_create(GRect(0, 50, 144, 30));
  text_layer_set_background_color(button_layer, GColorClear);
  text_layer_set_text_color(button_layer, GColorBlack);
  text_layer_set_text(button_layer, "");
  text_layer_set_font(button_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));  
  text_layer_set_text_alignment(button_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(button_layer));
  
  up_layer = text_layer_create(GRect(0, 20, 144, 30));
  text_layer_set_background_color(up_layer, GColorClear);
  text_layer_set_text_color(up_layer, GColorBlack);
  text_layer_set_text(up_layer, "");
  text_layer_set_font(up_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));  
  text_layer_set_text_alignment(up_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(up_layer));

  down_layer = text_layer_create(GRect(0, 85, 144, 30));
  text_layer_set_background_color(down_layer, GColorClear);
  text_layer_set_text_color(down_layer, GColorBlack);
  text_layer_set_text(down_layer, "");
  text_layer_set_font(down_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));  
  text_layer_set_text_alignment(down_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(down_layer));

  
}

static void main_window_unload(Window* window)
{
  text_layer_destroy(text_layer);
  text_layer_destroy(button_layer);
}


void handle_init(void) {
  my_window = window_create();
  window_set_click_config_provider(my_window, click_config_provider);
  
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(my_window, true);
//  accel_data_service_subscribe(0, NULL);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);  

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
//  app_timer_register(ACCEL_MS, timer_callback, NULL);
  
}

void handle_deinit(void) {
  app_message_deregister_callbacks();
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
