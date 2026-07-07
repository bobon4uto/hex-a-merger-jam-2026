// #define RESOURCES_IMPLEMENTATION
#ifndef    _RESOURCES_H_
#define    _RESOURCES_H_
// resources interface

typedef struct sMetaMusic {
  float       volume;
  const char* filename;
  const char* title;
  const char* author;
  const char* link;
  const char* date;
  const char* license;
  const char* license_link;
} MetaMusic;


typedef struct sMetaImage {
  const char* filename;
  const char* title;
  const char* author;
  const char* link;
  const char* date;
  const char* license;
  const char* license_link;
} MetaImage;


#ifdef      RESOURCES_IMPLEMENTATION
// resources implementation

static MetaMusic music_dark_shrine_loop_meta = (MetaMusic){
  .volume=1.0f,
  .filename="resources/music/qubodup-yd-DarkShrineLoop-OpenGameArt.ogg",
  .title="Dark shrine loop",
  .author="qubodup",
  .link="https://opengameart.org/content/dark-shrine-loop",
  .date="Wednesday, October 10, 2012 - 15:50",
  .license="CC0 1.0",
  .license_link="https://creativecommons.org/publicdomain/zero/1.0/",
};
static MetaMusic music_going_deep_meta = (MetaMusic){
  .volume=1.0f,
  .filename="resources/music/S31-Going Deep.ogg",
  .title="Going deep",
  .author="section31",
  .link="https://opengameart.org/content/going-deep",
  .date="Sunday, November 1, 2015 - 17:46",
  .license="CC0 1.0",
  .license_link="https://creativecommons.org/publicdomain/zero/1.0/",
};


#endif   // RESOURCES_IMPLEMENTATION
#endif   //_RESOURCES_H_


