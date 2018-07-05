
; ---------------------------------------------------------------------------

; enum objects,	mappedto_1, width 8 bytes
space		 = 0
zonk		 = 1
base		 = 2
murphy		 = 3
infotron	 = 4
chip		 = 5
hardware	 = 6
exit_		 = 7
orangedisk	 = 8
portright	 = 9
portdown	 = 10
portleft	 = 11
portup		 = 12
sportright	 = 13
sportdown	 = 14
sportleft	 = 15
sportup		 = 16
sniksnak	 = 17
yellowdisk	 = 18
terminal	 = 19
reddisk		 = 20
portvertical	 = 21
porthorizontal	 = 22
port4way	 = 23
electron	 = 24
bug		 = 25


stBeep = 1
stSample = 2
stAdlib = 3
stBlaster = 4
stRoland = 5


levelWidth = 60         ; 3Ch
levelHeight = 24        ; 18h
levelSize = levelWidth * levelHeight    ; 1440/5A0h
levelcells = levelSize

levelDataLength = 1536 ; 600h
levelbytes = 1536
maxdemolength = 0BE0Ah
