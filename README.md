This doc has been made for releases: ReverseNX-Tool **1.2**, ReverseNX **0.33**

ReverseNX-Tool is an additional tool to make managament of ReverseNX flags easier (titleid mode is not compatible with older releases of ReverseNX). 

https://github.com/masagrator/ReverseNX

-------------

Tool has three operation modes: `global`, `titleid` and `disabled`. If you want to change modes between `global` and `titleid`, press **+** button. If you want to disable loading ReverseNX, press **ZR**.

# global

global mode is set by default. Sets **Docked** or **Handheld** performance mode for every game that you will boot (with **docked** as default).

![global image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/global.jpg?raw=true)

>"No global flag detected. ReverseNX disabled" means that plugin is not forcing docked or handheld mode at this moment.
>
>"Detected docked global flag" means that plugin is forcing docked performance mode.
>
>"Detected handheld global flag" means that plugin is forcing handheld performance mode.

If you want to force one of performance modes, you need to press

**A** for Docked mode

**B** for Handheld mode

-------------

# titleid

titleid mode sets docked or handheld performance mode for each game separately.

![titleid_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/titleid.jpg?raw=true)

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

---

# disabled

Disabled mode locks loading plugins from SaltySD/plugins folder. 

![titleid_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/disabled.jpg?raw=true)

With my fork of SaltyNX in ReverseNX 0.33+ release we can avoid crashing games with cheats in this mode.

To turn off disabled mode, press **A**.
