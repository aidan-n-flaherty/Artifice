#ifdef __cplusplus
extern "C" {
#endif
  typedef void* cGame;
  typedef void* cGameSettings;
  cGameSettings initGameSettings(void);
  void freeGameSettings(cGameSettings);
  void addGameSetting(cGameSettings, char*, void*);
  cGame initGame(cGameSettings, int, double, int[], int, int);
  void freeGame(cGame);
  void addOrder(cGame, char*, int, int, double, int, int[], int);
  int runGame(cGame, int[]);
  int runGameWithNotifications(cGame, int[], double, int[]);
#ifdef __cplusplus
}
#endif
