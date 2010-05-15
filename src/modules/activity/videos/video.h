
extern Evas_Object *o_video;
extern Evas_Object *o_video_bg;

extern void video_spu_audio_info_print(int show);

extern void video_init(char *module, char *file, char *swallow);
extern void video_shutdown(void);
extern Eina_Bool video_event_cb(void* data, rage_input in);

