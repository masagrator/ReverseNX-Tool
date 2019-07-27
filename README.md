This doc has been made for release: **1.1**

ReverseNX-Tool is an additional tool to make managament of ReverseNX 0.32+ flags easier (titleid mode is not compatible with older releases). 

https://github.com/masagrator/ReverseNX

-------------

Tool has two operation modes: `global` and `titleid`. If you want to change operation mode, press **+** button.

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

If you want to disable forcing performance mode, you need to press **Y**.

-------------

# titleid

titleid mode sets docked or handheld performance mode for each game separately.

![titleid_image](https://github.com/masagrator/ReverseNX-Tool/blob/master/docs/titleid.jpg?raw=true)

**It's not a manager of flags for games!** It only informs ReverseNX to create needed files in correct folders for first game you will boot and it will stay in that way. After booting first game you need to set again new performance mode if you want to change already set performance modes or create a new one. _So changing performance mode works only for first game you will boot after changing performance mode in titleid mode!_

>"No change flag detected. ReverseNX-Tool disabled" means that ReverseNX will not change performance mode for next game you will boot.
>
>"Detected docked titleid flag" means that ReverseNX will change performance mode to **docked** for next game you will boot.
>
>"Detected handheld titleid flag" means that ReverseNX will change performance mode to **handheld** for next game you will boot.

If you want to change it, press

**A** for Docked mode

**B** for Handheld mode

If you want to bring default settings for game, press **Y**.
