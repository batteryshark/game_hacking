
WK_DB = {
	0:'WK_GOLD',              # money
	1:'WK_BGM',               # BGM
	2:'WK_LASTITEM',          # item number that was used in the last
	3:'WK_ITEMSTATE',         # item state
	4:'WK_KEY_CIRCLE',        # Circle button is pressed
	5:'WK_KEY_CROSS',         # X button has been pressed
	6:'WK_KEY_TRIANGLE',      # Triangle button is pressed
	7:'WK_KEY_SQUARE',        # Square whether the button is pressed
	8:'WK_KEY_L',             # L1 or L2 button is pressed
	9:'WK_KEY_R',             # R1 or R2 button is pressed
	10:'WK_GAMERANK',         # GAMERANK_EASY ~ GAMERANK_NIGHTMARE
	11:'WK_BGMLIMIT',         # When you put the 2, processing on the system side ignores keep a low volume. If you put a -1 to ignore the processing of the system side maintain a normal volume to. It is automatically reset in the map switching.
	12:'WK_MAPTRAVEL_RATIO',  # map Disrupt rate
	13:'WK_ACTITEM',          # action items
	14:'WK_ISNIGHT',          # or noon or night (day = 0 nights = 1)
	15:'WK_DAYTIME',          # time (0 to 23.99)
	16:'WK_EVENT', 		      # whether or not in event.
	17:'WK_MAPRATIO',         # map Disrupt rate
	18:'WK_ADOL_BTLMB',       # Adol, Battle members
	19:'WK_DUREN_BTLMB',      # Duren, Battle members
	20:'WK_CARNA_BTLMB',      # Carna, Battle members
	21:'WK_OZMA_BTLMB',       # Ozma, Battle members
	22:'WK_CANLILICA_BTLMB',  # Kanririka, Battle members
	23:'WK_FRIDA_BTLMB',      # Frida, Battle members
	24:'WK_RANDBTLMB',        # elected by random from the battle members
	25:'WK_RANDJOINMB',       # elected from members join in a random
	26:'WK_RANDBTLMB_NOADOL', # random other than the elected Adol from Battle members
	27:'WK_RANDJOINMB_NOADOL',# random other than the elected Adol from join members
	28:'WK_ISEXIST_PLAYER1',  # whether player 1 is present
	29:'WK_ISEXIST_PLAYER2',  # whether player 2 is present
	30:'WK_ISEXIST_PLAYER3',  # whether player 3 is present
	31:'WK_ISEXIST_PLAYER4',  # whether player 4 is present
	32:'WK_ITEMSTATELV',      # strengthen status of magic equipment (1-Not indoctrination 2-strengthening already)
	33:'WK_ISVITATV',         # VitaTV whether patch 1.02_2: added in the "Search word: PATCH0102_2
}

INFO_FLAG_DB = {
	1:'INFOFLAG_FIXED',                # fixed (rotation can be. can not move)
	2:'INFOFLAG_NOGRAVITY',            # gravity (height fixed: floor setting)
	4:'INFOFLAG_INVISIBLE',            # invisible
	8:'INFOFLAG_MUTEKI',               # completely invincible
	0x10:'INFOFLAG_GUARD',             # guard other than the event
	0x20:'INFOFLAG_NOSHADOW',          # Kagenashi
	0x40:'INFOFLAG_NOATARI',           # not check per (effect also map also not be seen characters)
	0x80:'INFOFLAG_FLING',             # flying (height fixed: non-floor setting)
	0x100:'INFOFLAG_PLDASH',           # in the player dash (the movement is hard to bend becomes a factor of 1.5)
	0x200:'INFOFLAG_NORESURRECT',      # not resurrected
	0x400:'INFOFLAG_ANYTIMECALC',      # any time
	0x8000:'INFOFLAG_NOPUSH',          # not pushed to the other characters
	0x1000:'INFOFLAG_NOCHKINVCOLL',    # per invisible removed from the decision per
	0x2000:'INFOFLAG_NODAMAGE',        # There is a decision I hit not received damage
	0x4000:'INFOFLAG_NOMAPATARICHECK', # Not determined per the map
	0x8000:'INFOFLAG_NOTALKTURN',      # Not to change direction at the time of conversation
	0x10000:'INFOFLAG_NOCHRATARI',     # I do not see around the characters
	0x20000:'INFOFLAG_NOTALKMOTION',   # Not to change the motion at the time of conversation
	0x40000:'INFOFLAG_NODEAD',         # not death (not turn also to .CalcDead to hang on in the HP1. primarily for turtle boss)
	0x80000:'INFOFLAG_SWIMMING',       # swim in
	0x100000:'INFOFLAG_LAVAON',        # Riding a lava (per CE)
	0x01000000:'INFOFLAG_WATER',       # water (face is out)
	0x02000000:'INFOFLAG_INWATER',     # water (sinking are in)
	0x04000000:'INFOFLAG_INZUBUZUBU',  # Zubuzubu sink location
	0x10000000:'INFOFLAG_ICEON',       # ice
	0x20000000:'INFOFLAG_DUMMY',       # substitute characters (used in TALKPT)
	0x40000000:'INFOFLAG_NETERU'       # for sleeping character (put in a low position Emotion)
}

ENV_SE_DB = {
	0:'ENVSE_NONE',		    # no
	1:'ENVSE_2DSOUND',		# 2D environmental sound
	2:'ENVSE_POINTSOUND',	# point sound source
	3:'ENVSE_RAILSOUND',	# rail sound source
	4:'ENVSE_SURFACESOUND',	# surface sound source
	5:'ENVSE_EVBOXSOUND',	# to play the sound in the event box
}
SE_DB = {
# TODO - Copy from se.h
}
MUSIC_DB = {
# TODO - Copy from music.h
}

FLAGS_DB = {
413:"SF_ADOL_JOINOK"
}