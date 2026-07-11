// #define RESOURCES_IMPLEMENTATION
#ifndef    _RESOURCES_H_
#define    _RESOURCES_H_
// resources interface

typedef struct sMetaMusic {
  float       volume;
  Music       music;
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


static MetaMusic music_unforgiving_himalayas_meta = (MetaMusic){
  .volume=1.0f,
  .filename="resources/music/unforgiving_himalayas_looping_0.ogg",
  .title="\"UNFORGIVING HIMALAYAS\"",
  .author="Eric Matyas\n www.soundimage.org",
  .link="https://opengameart.org/content/unforgiving-himalayas-looping",
  .date="Thursday, July 9, 2026 - 07:27",
  .license="CC BY 3.0",
  .license_link="https://creativecommons.org/licenses/by/3.0/",
};

static MetaMusic music_forest_meta = (MetaMusic){
  .volume=1.0f,
  .filename="resources/music/forest.ogg",
  .title="Creepy Forest",
  .author="Augmentality\n (Brandon Morris)",
  .link="https://opengameart.org/content/creepy-forest-f",
  .date="Tuesday, October 26, 2010 - 22:40",
  .license="CC0 1.0",
  .license_link="https://creativecommons.org/publicdomain/zero/1.0/",
};


#endif   // RESOURCES_IMPLEMENTATION
#endif   //_RESOURCES_H_


