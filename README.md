This doc has been made for releases: ReverseNX-Tool **1.34**, ReverseNX **0.34**

ReverseNX-Tool is an additional tool to make managament of ReverseNX flags easier.

https://github.com/masagrator/ReverseNX

-------------

Tool has three operation modes: `global`, `titleid` and `disabled`.

For `global` and `titleid` modes:

If you want to change modes, press **+** button. 

If you want to disable loading ReverseNX, press **ZR**. It will go to disabled mode.

If you want to disable loading cheats on game boot in Atmosphere, press **ZL** (This option is hidden for other CFWs).

# global

global mode is set by default. Sets **Docked** or **Handheld** performance mode for every game that you will boot (with **docked** as default).

![global image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/global.jpg?raw=true)

>"Detected docked global flag" means that plugin is forcing docked performance mode.
>
>"Detected handheld global flag" means that plugin is forcing handheld performance mode.

If you want to force one of performance modes, you need to press

**A** for Docked mode

**B** for Handheld mode

-------------

# titleid

titleid mode sets docked or handheld performance mode for each game separately.

![titleid_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/titleid2.jpg?raw=true)

**It's not a manager of flags for games!** It only informs ReverseNX to create needed files in correct folders for first game you will boot and it will stay in that way. After booting first game you need to set again new performance mode if you want to change already set performance modes or create a new one. **So changing performance mode works only for first game you will boot after setting performance mode in titleid operation mode of ReverseNX-Tool!**

>"No change flag detected. ReverseNX-Tool disabled" means that ReverseNX will not change performance mode for next game you will boot.
>
>"Detected docked titleid flag" means that ReverseNX will change performance mode to **docked** for next game you will boot.
>
>"Detected handheld titleid flag" means that ReverseNX will change performance mode to **handheld** for next game you will boot.

If you want to change it, press

**A** for Docked mode

**B** for Handheld mode

If you want to bring default settings for game, press **Y**.

When game is running, it will show at first line what flag is set in folder for this game.

---

# disabled

Disabled mode locks loading plugins from SaltySD/plugins folder. 

![titleid_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/disabled2.jpg?raw=true)

To turn off disabled mode, press **A**.

To change cheats state, press **B**.

---

# How to use cheats in Atmosphere

Cheats loaded by Atmosphere when game is booting are crashing OS. To avoid it I added to ReverseNX-Tool function for renaming cheats folders in Atmosphere/titles and in Atmosphere/contents.

If you want to use ReverseNX with cheats, You need to go in this order:

>Disable Cheats in ReverseNX-Tool
>
>Run game and wait for first sign of game loading after Nintendo logo will disappear
>
>Enable Cheats in ReverseNX-Tool
>
>Use Edizon or Checkpoint to apply cheats

---

# Troubleshooting

If SaltyNX was not injected properly or at all, you will see... Red Screen of Death.
![error_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/error.jpg?raw=true)

If you will see red text with first line "SaltyNX was not injected!!", check if files were copied properly. Otherwise it means CFW can't handle it.
