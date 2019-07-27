This docs are made for releases: **1.1**

ReverseNX-Tool is an additional tool to make managamenet of ReverseNX 0.32+ flags easier (titleid mode is not compatible with older releases). 

https://github.com/masagrator/ReverseNX

-------------

Tool has two operation modes: `global` and `titleid`. If you want to change operation mode, press **+** button.

#global

Global mode is set by default. Sets **docked** or **handheld** performance mode for every game that you will boot (with **docked** as default).

![global image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/global.jpg?raw=true)

"No global flag detected. ReverseNX disabled" means that plugin is not forcing docked or handheld mode at this moment.

If you want to force one of performance modes, you need to press

**A** for docked mode

**B** for handheld mode

If you want to disable forcing performance mode, you need to press **Y**.

-------------

#titleid

Titleid mode sets docked or handheld performance mode for each game separately.

![titleid_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/titleid.jpg?raw=true)

**It's not a manager of flags for games!** It only creates flag for ReverseNX that will force it to set chosen performance mode for first game it will boot. After creating flag for performance mode it will delete any sign of changing performance mode. So after booting game you need to again set new performance mode in titleid mode if you want to change flag for the same game or for different game.

"No change flag detected. ReverseNX-Tool disabled" means that ReverseNX will not change performance mode for next game you will boot.

If you want to change it, press

**A** for docked mode

**B** for handheld mode

If you want to bring default settings for game, press **Y**.
