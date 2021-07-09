# -*- coding: utf-8 -*-
# Ys Foliage in Ocean Celceta Monster Lib Generator
import os,sys,struct,textwrap

"""
 Monslib description: This file controls what is seen in the bestiary. The original format is a 
 type4 commented Falcom text file:
 - // denotes comments and they normally separate sections with it).
 - Fields are separated with new lines
 - Text can use forced newline literals \n to linebreak ingame.
 - The order of the entries may play a role in the order of the pages in the game - we'll see.
   To be safe, I'll save the order and keep everything in a list in its original order and test
   this theory later on.
 - Each entry has a few fields:
	>  Monster ID (shown in status.csv): (e.g. E4905)
	>  Monster Name - If it's a boss: 「Boss Subtitle」Boss Name. Not all bosses have this (Mini-Bosses)
	   The engine may exclude whats in japanese quotes from the nameplate on the menu (「...」)
	>  0 or 1 - If this is a boss.
	>  Description Text - I think the size is about 48 chars per line -144 max. I'll make this script break on words.
	
 - Monsters are divided by Area. To start a new Area section, use AREA, Newline, "Area Name"
 - Although Falcom specified the map code for clarity before each section (e.g. //mp13xx) - we probably
   don't need to do this.
 
 monslib.tbb file format.
 0      uint16 MAGIC - Normall 4 (maybe plaintext type 4)
 2      uint8  padding
 3      uint32 Size of string table
 7      uint8  padding
 8      uint32 Size of raw string section
 C      var    raw string section
 var    var    string table (each 4 byte entry is an offset to a new line (not counting forced \n).
 
"""
OUTPUT_NAME = "monslib.tbb"
OUTPUT_MAGIC = struct.pack("<H",0x04)
LINE_MAX = 41 # Max number of characters per description line.
NUM_LINES = 3 # Number of lines available for description.




LOCATION_TABLE = {
0:"Casnia Mine",                   # mp13xx
1:"Beast Plains",                  # mp110x
2:"Forest of Dawn",                # mp21xx
3:"Jade Cavern",                   # sd1100
4:"Waterfall Cave",                # sd2100
5:"Comodo",                        # mp13xx
6:"Gidona Crater",                 # sd23xx
7:"Ancient Burrow",                # sd23xx
8:"Algon River Basin",             # sd31xx
9:"Sacred Beasts Nest",            # mp33xx
10:"Underground Ruins",            # sd53xx
11:"Plateau Path",                 # mp610x
12:"Sanctuary Approach",           # mp611x
13:"Tower Of Providence",          # mp43xx
14:"Highland",                     # mp6201
15:"Table Mountain",               # mp612
16:"Subterranean Forest",          # mp51xx
17:"Ashen Forest",                 # mp613x
18:"The Frog's Inn",               # sd3110
19:"Forest of Spores",             # mp616x
20:"Colonia Battlefield",          # mp614x
21:"Outskirts of Elduke",          # mp614x
22:"Mt. Vesuvio",                  # mp634x
23:"Lake Tolmes Ruins",            # mp636x
24:"Colonia Artifact Laboratory",  # mp632x
25:"Iris",                         # mp731x
26:"Mt. Vesuvio - Volcano",        # mp739x
27:"BOSS"                          
}
"""
MONSTER TABLE TEMPLATE:
{
"id":"",
"name":"",
"boss":0,
"desc":"",
"area":LOCATION_TABLE[0]
},
"""
MONSTER_TABLE = [
#--- Casnia Mine ---
{
"id":"E4905",
"name":"Crustaceous Mole",
"boss":0,
"desc":"Mole-like monster that awoke during the commotion in the mine. While its vision is lacking, its sense of smell is exceptional.",
"area":LOCATION_TABLE[0]
},
{
"id":"E4900",
"name":"Sticky Worm",
"boss":0,
"desc":"Monster shaped like an earthworm. Slides toward its foes and spits a powerful acid.",
"area":LOCATION_TABLE[0]
},
{
"id":"E4901",
"name":"Ore Insect",
"boss":0,
"desc":"Insect monster disguised as ore. Discharges electricty with its feelers to paralyze prey.",
"area":LOCATION_TABLE[0]
},
{
"id":"E4904",
"name":"Molmoth",
"boss":1,
"desc":"A ferocious giant mole monster. Digs into the ground and attacks with its sharp claws.",
"area":LOCATION_TABLE[0]
},
{
"id":"E4903",
"name":"Olrabadi",
"boss":1,
"desc":"A bat with an extraordinarily large body and wings. Uses ultrasonic waves to stop its foes' movements and tears them apart from within.",
"area":LOCATION_TABLE[0]
},
#--- Beast Plains ---
{
"id":"E9902",
"name":"Ratani",
"boss":0,
"desc":"An adept beast that uses long branches as spears - tends to hunt in packs.",
"area":LOCATION_TABLE[1]
},
{
"id":"E0002",
"name":"Arenjo",
"boss":0,
"desc":"Arenjo has skin tough as armor. Curls into a ball to perform a rolling attack.",
"area":LOCATION_TABLE[1]
},
{
"id":"E0004",
"name":"Fabros",
"boss":0,
"desc":"Fabros is a normally docile herbivore which, when threatened, attack with its powerful horns.",
"area":LOCATION_TABLE[1]
},
{
"id":"E0060",
"name":"Langritz",
"boss":1,
"desc":"Commonly found in the Great Forest, these giant bear move surprisingly fast given their size.",
"area":LOCATION_TABLE[1]
},

# --- Forest of Dawn ---
{
"id":"E0113",
"name":"Cobonga",
"boss":0,
"desc":"A primate that lives in the Great Forest. Has high intelligence, attacking its foes from a distance by throwing rocks.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0102",
"name":"Asarkos",
"boss":0,
"desc":"Scorpion that usually lives in tropical areas, but has become enlarged in the Great Forest environment. Possesses an extremely lethal poison.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0154",
"name":"Vargin",
"boss":0,
"desc":"One of the terrifying mermaids that control the wetlands. Capable of moving swiftly in water, impaling its foes with a spear from close range.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0150",
"name":"Ramelebolero",
"boss":0,
"desc":"An insect which walks on water using the suction pads on its feet. Shoots large bubbles of water from its mouth and uses them to prey on small animals.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0152",
"name":"Orogilan",
"boss":0,
"desc":"Giant fish said to have lived in the wetlands of the Great Forest since ancient times. Possesses a powerful jaw that can crush the bones of its prey.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0101",
"name":"Bozowalla",
"boss":0,
"desc":"An insectivorous plant which has continued to evolve in the Great Forest. Spits seeds at distant foes and poison at those nearby.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0207",
"name":"Canolen",
"boss":0,
"desc":"A plant that scatters seeds as if firing them from a slingshot to attack all who get close. Demands extreme caution.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0208",
"name":"Shard",
"boss":0,
"desc":"A large bird that lives in seaside areas. Stocks water and pressurizes it within its body, firing it in jets at high speeds to attack its prey.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0202",
"name":"Gazapo",
"boss":0,
"desc":"A crab that has adapted to life on dry land. Its shell is incredibly tough, making it nearly invulnerable to damage while standing upright.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0100",
"name":"Floura",
"boss":0,
"desc":"Ancient plant with legs like an insect. Stores the toxic substances it extracts from the ground as sustenance.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0153",
"name":"Rushiarbo",
"boss":0,
"desc":"A sinister-looking beetle. Draws enemies near with its long tongue before piercing them repeatedly with its thorn-like legs.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0162",
"name":"Barazowalla",
"boss":0,
"desc":"Plant unique to the Great Forest that grows in dark places. Shoots a spray of pollen capable of causing paralysis.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0114",
"name":"Bozuonga",
"boss":0,
"desc":"Leader of a group of primates. Swings its axe with incredible force and can summon minions with its mighty roar.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0108",
"name":"Rangoa",
"boss":0,
"desc":"Extremely powerful wild boar. Its face is covered by an armor-like exoskeleton, allowing it to reduce the damage to itself when colliding with foes.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0160",
"name":"Golwonga",
"boss":1,
"desc":"King of the monkeys. Sleeps deep within the forest. Wields its sword with power beyond all other beasts, slaughtering all who stand in its path.",
"area":LOCATION_TABLE[2]
},
{
"id":"E0950",
"name":"Golden Pikkard",
"boss":0,
"desc":"An extremely rare, shining pikkard. Not often seen, and drops a large amount of gold if defeated.",
"area":LOCATION_TABLE[2]
},
{
"id":"B058",
"name":"Foria-Daros",
"boss":1,
"desc":"A holy beast which has lived deep within the Great Forest since ancient times as its ruler. Cultivates countless life forms with its immense power.",
"area":LOCATION_TABLE[2]
},
# --- Jade Cavern ---
{
"id":"E4902",
"name":"Labadi",
"boss":0,
"desc":"A small bat that moves in swarms. Restrains its foes with eerie ultrasonic waves before attacking from above.",
"area":LOCATION_TABLE[3]
},
{
"id":"E5060",
"name":"Vesbiwar",
"boss":1,
"desc":"A giant spider that has multiplied deep within the cave. As a group, they spit sticky silk to capture prey, and then devour it until only bone remains.",
"area":LOCATION_TABLE[3]
},
{
"id":"E5061",
"name":"Vesbilizon",
"boss":1,
"desc":"A giant deep crimson spider that reproduces at an alarming rate. Captures prey using an acidic, sticky web and devours it as a group.",
"area":LOCATION_TABLE[3]
},
# --- Waterfall Cave ---
{
"id":"E0211",
"name":"Ribbi-Magus",
"boss":1,
"desc":"Once a high ranking Ribbi, but exiled after it was discovered to be plotting an uprising. Currently seeking a place to rest with its subordinates.",
"area":LOCATION_TABLE[4]
},
# --- Comodo ---
{
"id":"B950",
"name":"Duran",
"boss":1,
"desc":"A burly man exploring the Great Forest with Adol. An information dealer, he possesses a wide variety of knowledge, as well as skill in martial arts.",
"area":LOCATION_TABLE[5]
},
# --- Gidona Crater ---
{
"id":"E0103",
"name":"Blaybye",
"boss":0,
"desc":"A ferocious bee which hails from the Ancient Burrow. Deals lethal damage to its foes using its large knife-like stinger.",
"area":LOCATION_TABLE[6]
},
{
"id":"E0112",
"name":"Selbanther",
"boss":0,
"desc":"A beast which hunts on the plains of the Great Forest. Uses its incredibly powerful legs to leap at its prey, then tears at it with its giant fangs.",
"area":LOCATION_TABLE[6]
},
# --- Ancient Burrow ---
{
"id":"B051b",
"name":"Masked Warrior",
"boss":1,
"desc":"One of the hunters of Comodo. The mask he wears draws out his latent potential, allowing him to fight with remarkable strength.",
"area":LOCATION_TABLE[7]
},
{
"id":"E5000",
"name":"Jasraja",
"boss":0,
"desc":"A monstrous mollusk. Releases powerful electricity from its charged feelers, causing its foes to burst apart at the seams.",
"area":LOCATION_TABLE[7]
},
{
"id":"E5003",
"name":"Damrahm",
"boss":0,
"desc":"A larva which crawls across the ground. Appears harmless, but extends a horn length of its body to pierce enemies if threatened.",
"area":LOCATION_TABLE[7]
},
{
"id":"E5002",
"name":"Pashringa",
"boss":0,
"desc":"A small beetle which inhabits narrow burrows. The evolved form of a damrahm. It protects itself by discharging an acidic liquid from its tail.",
"area":LOCATION_TABLE[7]
},
{
"id":"E5004",
"name":"Egrege",
"boss":0,
"desc":"Pashringa's adult form. Carnivorous and highly aggressive, it will swiftly attack any human that enters its line of sight.",
"area":LOCATION_TABLE[7]
},
{
"id":"E5001",
"name":"Vesbinake",
"boss":0,
"desc":"A large spider that occupies the Ancient Burrow. Creates sticky silk inside its body, which it sprays at foes to restrain them.",
"area":LOCATION_TABLE[7]
},

# --- Algon River Basin ---
{
"id":"E0203",
"name":"Hourun",
"boss":0,
"desc":"Lives near water and appears to wear a helmet. Has eyes on its back and when it sees an enemy, it aims for the neck and fires countless needles.",
"area":LOCATION_TABLE[8]
},
{
"id":"E0210",
"name":"Gelog",
"boss":0,
"desc":"A frog which lives alongside the river. Toys with its foes through unpredictable movements, and draws them near using its long tongue.",
"area":LOCATION_TABLE[8]
},
{
"id":"E0209",
"name":"Crocle",
"boss":0,
"desc":"A large reptile that has inhabited this area since ancient times. Its jaw is strong enough to break iron and even the toughest armor cannon repel it.",
"area":LOCATION_TABLE[8]
},
# --- Sacred Beasts Nest ---
{
"id":"E5102",
"name":"Bibarmul",
"boss":0,
"desc":"A strongly territorial aquatic creature. While it is agile and has sharp claws, it is also cowardly and quickly plays dead.",
"area":LOCATION_TABLE[9]
},
{
"id":"E5101",
"name":"Rapshel",
"boss":0,
"desc":"A relatively docile bivalve. Usually stays in one place and waits for prey to approach.",
"area":LOCATION_TABLE[9]
},
{
"id":"E5103",
"name":"Amnagan",
"boss":0,
"desc":"A crustacean with powerful, sharp claws. One of the biggest reasons fishermen have to watch where they step.",
"area":LOCATION_TABLE[9]
},
{
"id":"E5100",
"name":"Reppi",
"boss":0,
"desc":"A carnivorous fish that lives in the river. Pierces prey with its sharp fangs. Particularly dangerous in water due to its prey's restricted movements.",
"area":LOCATION_TABLE[9]
},
{
"id":"E5104",
"name":"Pigbulb",
"boss":0,
"desc":"A bizarre mollusk that developed near water. uses powerful electricity from its feelers to paralyze its prey for later devouring.",
"area":LOCATION_TABLE[9]
},
{
"id":"E0205",
"name":"Varchin",
"boss":0,
"desc":"An aquatic creature that explodes when a threat approaches as an instinctive (and somewhat ironic) act of 'self-preservation.'",
"area":LOCATION_TABLE[9]
},
{
"id":"E5105",
"name":"Zogrom",
"boss":0,
"desc":"A large frog, distinctive for its multiple eyes, that lives in areas of high humidity. Crushes foes with its body press and powerful acidic spit.",
"area":LOCATION_TABLE[9]
},
# --- Underground Ruins ---
{
"id":"E5251",
"name":"Ka-Bulb",
"boss":0,
"desc":"A strange soft-bodied life form with little combat ability or intelligence.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5250",
"name":"Hal-Bulb",
"boss":0,
"desc":"A strange soft-bodied life form with two feelers. Splits into two Ka-Bulbs when defeated.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5202",
"name":"Bulb",
"boss":0,
"desc":"An aggregation of strange, soft-bodied life forms. Attacks using its four feelers, and splits into two Hal-Bulbs when defeated.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5206",
"name":"Cobira",
"boss":0,
"desc":"Beast that has lived in the dark Underground Ruins since antiquity, causing its eyes to deteriorate. Protects itself by attacking with its sharp claws.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5200",
"name":"Complerra",
"boss":0,
"desc":"Part of a tribe of monsters with their own culture. Fights using different weapons, such as fire arrows or knives, depending on the situation.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5203",
"name":"Zorats",
"boss":0,
"desc":"A mouse with an unnaturally swollen head. Loves darkness, and attacks anything that draws near with its teeth.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5201",
"name":"Guspoun",
"boss":0,
"desc":"A mutated plant found deep underground. Spits acid at its surroundings and preys upon those paralyzed by it.",
"area":LOCATION_TABLE[10]
},
{
"id":"E5260",
"name":"Compragov",
"boss":1,
"desc":"Patriarch of a group of monsters. Attacks using its large axe and powerful explosions. Plotted to invade the Primeval Lands with its hordes of subordinates.",
"area":LOCATION_TABLE[10]
},
# --- Plateau Path ---
{
"id":"E0000",
"name":"Wonma",
"boss":0,
"desc":"A rare species of bird which lives on the plateau. Jumps great heights, smashing its enlarged beak upon enemies from above like a hammer.",
"area":LOCATION_TABLE[11]
},
{
"id":"E0051",
"name":"Robutani",
"boss":0,
"desc":"A fox which wields a roughly-short stick. Hunts its prey with great agility.",
"area":LOCATION_TABLE[11]
},
{
"id":"E0050",
"name":"Boltani",
"boss":0,
"desc":"A shockingly dextrous fox. Uses a hand-made wooden bow to hunt as if it were a human.",
"area":LOCATION_TABLE[11]
},
{
"id":"E0005",
"name":"Egriz",
"boss":0,
"desc":"A ferocious giant bear that lives in the Primeval Lands. Tenaciously pursues its prey, impaling them alive with its sharp fangs.",
"area":LOCATION_TABLE[11]
},
# --- Sanctuary Approach ---
{
"id":"E5360",
"name":"Ruzaslape",
"boss":1,
"desc":"A guardian that's wandered outside the Tower of Providence, forgetting its original purpose. Prowls the Sanctuary of Storms, attacking blindly.",
"area":LOCATION_TABLE[12]
},

# --- Tower Of Providence ---
{
"id":"E5301",
"name":"Rogiong",
"boss":0,
"desc":"One of the guardians of the Tower of Providence. Curls into a ball and squashes its enemies by rolling over them at high speeds.",
"area":LOCATION_TABLE[13]
},
{
"id":"E5304",
"name":"Inslape",
"boss":0,
"desc":"A horse-like guardian with a body of iron. Out of control, it uses its hooves to kill enemies instead of capturing them as originally designed.",
"area":LOCATION_TABLE[13]
},
{
"id":"E5300",
"name":"Zelsitu",
"boss":0,
"desc":"An armored guardian carrying a giant axe. out of control, it treats all those who enter its line of sight as intruders and attempts to eliminate them.",
"area":LOCATION_TABLE[13]
},
{
"id":"E5306",
"name":"Vivuig",
"boss":0,
"desc":"A guardian that disguises itself as a treasure chest. Slices up those whose desire drives them to seek its contents.",
"area":LOCATION_TABLE[13]
},
{
"id":"E5350",
"name":"Le-Vivuig",
"boss":0,
"desc":"A guardian that disguises itself as a red treasure chest. Restraint is required to safely ascertain its true nature.",
"area":LOCATION_TABLE[13]
},
{
"id":"E5351",
"name":"Vivuigol",
"boss":0,
"desc":"An enchanting guardian that disguises itself as a shining gold treasure chest. Said to seduce those filled with greed and test their true worth.",
"area":LOCATION_TABLE[13]
},
{
"id":"E5302",
"name":"Gelgrarga",
"boss":0,
"desc":"Originally a stone statue that guarded the tower. Warps behind intruders, summoning lightning to burn them to a crisp.",
"area":LOCATION_TABLE[13]
},

# --- Highland ---
{
"id":"B051",
"name":"Masked Soldier",
"boss":1,
"desc":"A soldier from the Romun Army who has obtained inhuman power by wearing a mysterious mask.",
"area":LOCATION_TABLE[14]
},

# --- Table Mountain ---
{
"id":"E0309",
"name":"Rabee",
"boss":0,
"desc":"An extremely dangerous rabbit with a tail shaped like a claw. Its fur is tufted.",
"area":LOCATION_TABLE[15]
},
{
"id":"E0303",
"name":"Brancha",
"boss":0,
"desc":"A spider with an enlarged spinneret. Sprays acidic spider silk, and can cause its spinneret to expand and explode when angered.",
"area":LOCATION_TABLE[15]
},
{
"id":"E0308",
"name":"Gemelon",
"boss":0,
"desc":"Large reptile with a sharp axe-like horn. Moves slowly, but when it grabs enemies with its tongue and swings them around, its power is immense.",
"area":LOCATION_TABLE[15]
},



# --- Subterranean Forest ---
{
"id":"E0400",
"name":"Pitol",
"boss":0,
"desc":"An insect adept at moving in dark environments. While its movements are swift and its jumping attacks are powerful, it often winds up tripping.",
"area":LOCATION_TABLE[16]
},
{
"id":"E0402",
"name":"Bonpeet",
"boss":0,
"desc":"Monster which stores noxious gases form the deepest parts of the Ashen Forest inside itself. Its eyes are weak, so it uses ultrasonic waves to 'see'.",
"area":LOCATION_TABLE[16]
},
{
"id":"E0403",
"name":"Rudorian",
"boss":0,
"desc":"A grotesque monster with a mouth that covers its entire body. Bites and chews its prey as if embracing it with its entire body.",
"area":LOCATION_TABLE[16]
},
{
"id":"E0161",
"name":"Anderaula",
"boss":0,
"desc":"A strange plant created as a result of magic. Wanders entirely through instinct, expelling or injecting a cursed poison into its targets.",
"area":LOCATION_TABLE[16]
},
# --- Ashen Forest ---
{
"id":"E0500",
"name":"Grahm",
"boss":0,
"desc":"A giant-bodied monster that lives in polluted earth. Wriggles across the surface, targeting other living creatures.",
"area":LOCATION_TABLE[17]
},
{
"id":"E0551",
"name":"Benolid",
"boss":0,
"desc":"A strange creature born in areas where the toxic gases are particularly dense. Explodes when touched, spreading large amounts of toxins nearby.",
"area":LOCATION_TABLE[17]
},
{
"id":"E0550",
"name":"Dizwalla",
"boss":0,
"desc":"Plant which came about as a sudden mutation in poor-quality soil. Fires a barrage of seeds upon sensing prey, and can spit toxic gases at nearby foes.",
"area":LOCATION_TABLE[17]
},
{
"id":"E0501",
"name":"Vilma",
"boss":0,
"desc":"A monster which floats in stagnant air. Spreads toxic gases when it senses other living creatures, causing significant bodily harm.",
"area":LOCATION_TABLE[17]
},
{
"id":"E0506",
"name":"Rotoruta",
"boss":0,
"desc":"A turtle which has evolved to be able to survive in the toxic marshland. Sprays toxic gases from its carapace and charges its foes at high speeds.",
"area":LOCATION_TABLE[17]
},
{
"id":"E0509",
"name":"Madogra-Slef",
"boss":0,
"desc":"A large plant born in a corrupted ecosystem. uses seeds and poisonous gases to steal the energy of other creatures and extend its own life.",
"area":LOCATION_TABLE[17]
},
{
"id":"E0900",
"name":"Vandaroper",
"boss":1,
"desc":"The ghost of a nameless soldier who died hundreds of years ago. Seeks eternal slumber after one last duel with a strong warrior.",
"area":LOCATION_TABLE[17]
},

# --- The Frog's Inn ---
{
"id":"E9901",
"name":"Frogman",
"boss":0,
"desc":"A humanoid frog that lives in an area of the Ashen Forest where the concentration of noxious gases is low. Most are remarkably lazy.",
"area":LOCATION_TABLE[18]
},
# --- Forest of Spores ---
{
"id":"E0650",
"name":"Zakaruko",
"boss":0,
"desc":"A large scorpion born in a distorted environment. Its scissor-like tail is strong enough to cut through solid bone.",
"area":LOCATION_TABLE[19]
},
{
"id":"E0602",
"name":"Ravaeel",
"boss":0,
"desc":"Infant monster shelled in a thin egg-like membrane that it can enter and exit at will. Spends most of its time hopping, but occasionally headbutts foes.",
"area":LOCATION_TABLE[19]
},
{
"id":"E0601",
"name":"Demileel",
"boss":0,
"desc":"Ravaeel's adult form. While it has no eyes, it can sense the presence of prey and bite at them with its strong jaw.",
"area":LOCATION_TABLE[19]
},
{
"id":"E0600",
"name":"Supofanga",
"boss":0,
"desc":"A monster that has developed to have arms like scythes. Moves comically while taking aim at its foes, and fights with acidic mucus.",
"area":LOCATION_TABLE[19]
},
# --- Colonia Battlefield ---
{
"id":"E0752",
"name":"Compleras",
"boss":0,
"desc":"A humanoid monster residing in Colonia Battlefield. Shoots fire arrows coated with a special oil that chars the point of impact.",
"area":LOCATION_TABLE[20]
},
{
"id":"E9903",
"name":"Pogarov",
"boss":0,
"desc":"A man-beast hybrid that lives in the wastelands. The smell of its breath is so bad, one whiff will render any poor soul unable to move.",
"area":LOCATION_TABLE[20]
},
{
"id":"E5603",
"name":"Galba-Deule",
"boss":1,
"desc":"A living weapon created during an ancient battle. Has been bound to a heavy door since, blindly attacking all who approach it.",
"area":LOCATION_TABLE[20]
},
# --- Outskirts of Elduke ---
{
"id":"E0750",
"name":"Wibbye",
"boss":0,
"desc":"A giant bee which is said to have resided in this area since the time of the Celcetan Kingdom. Takes down its prey by shooting contracted needles.",
"area":LOCATION_TABLE[21]
},
{
"id":"E0751",
"name":"Madagora",
"boss":0,
"desc":"A plant which stores acidic mucus inside its body. The impact created when it hits the ground causes a geyser effect, rendering its prey helpless.",
"area":LOCATION_TABLE[21]
},
# --- Mt. Vesuvio ---
{
"id":"E5450",
"name":"Avalatch",
"boss":0,
"desc":"A primate known for its white fur. Roams around on the snowy peak, attacking all who attempt to climb the mountain.",
"area":LOCATION_TABLE[22]
},
{
"id":"E5400",
"name":"Grunpe",
"boss":0,
"desc":"A bird with wings that don't actually enable it to fly. Slides across the frozen ground at great speeds, using its sharp beak to kill its prey.",
"area":LOCATION_TABLE[22]
},
{
"id":"E5452",
"name":"Domlits",
"boss":0,
"desc":"A beetle with a large body and sharp feelers that help it survive in the extreme cold. Digs into the ground at high speed and attacks from below.",
"area":LOCATION_TABLE[22]
},
{
"id":"E5453",
"name":"Avabish",
"boss":0,
"desc":"A large primate that lives deep within the mountains. Attacks by throwing snowballs from high places, but flees when an enemy draws near.",
"area":LOCATION_TABLE[22]
},
{
"id":"E5451",
"name":"Nogriz",
"boss":0,
"desc":"A large polar bear that dwells on Mt. Vesuvio. Said to use its icy breath to freeze its prey for transport back to its nest.",
"area":LOCATION_TABLE[22]
},
# --- Lake Tolmes Ruins ---
{
"id":"E5550",
"name":"Merahva",
"boss":0,
"desc":"Demi-human monster which has taken to living in the deserted ruins. Skilled at fighting from any range thanks to its homemade steel boomerang.",
"area":LOCATION_TABLE[23]
},
{
"id":"E5500",
"name":"Tolmeus",
"boss":0,
"desc":"Large beast which has lived around Lake Tolmes since ancinet times. Spits large quantities of water at any who attempt to enter its territory.",
"area":LOCATION_TABLE[23]
},
{
"id":"E5551",
"name":"Ibolzu",
"boss":0,
"desc":"A giant frog that creates acid within its body. Hard as a rock, and strengthens its defenses when it senses danger nearby.",
"area":LOCATION_TABLE[23]
},
{
"id":"E5501",
"name":"Sodwar",
"boss":0,
"desc":"A winged fish said to have once lived in water. Sprials toward enemies and pierces them with its sharp beak.",
"area":LOCATION_TABLE[23]
},
# --- Colonia Artifact Laboratory ---
{
"id":"E5610",
"name":"Galba-Zera",
"boss":0,
"desc":"Living weapon which was abandoned during an experiment in ancient times. Its instinctive desire to eliminate enemies on sight remains.",
"area":LOCATION_TABLE[24]
},
{
"id":"E5601",
"name":"Pulgart",
"boss":0,
"desc":"Used in an ancient war. Employs advanced technology that allows it to focus and fire light. Likely a byproduct of the creation of living weapons.",
"area":LOCATION_TABLE[24]
},
{
"id":"E5600",
"name":"Goras",
"boss":0,
"desc":"Monster which floats around the laboratory. It's said that the souls of those who died in battle became gorases and wander the area.",
"area":LOCATION_TABLE[24]
},
{
"id":"E5604",
"name":"Denas",
"boss":0,
"desc":"Monster which resembles the grim reaper. Brandishes a scythe and seeks souls to consume.",
"area":LOCATION_TABLE[24]
},
{
"id":"E5605",
"name":"Godenas",
"boss":0,
"desc":"A monster that's acquired its true form by consuming wandering souls. Curses any form of life it finds, then takes that life with its scythe.",
"area":LOCATION_TABLE[24]
},
{
"id":"E5650",
"name":"Pit-Bulb",
"boss":0,
"desc":"Mollusk-type monster with four feelers on its head, which it uses to spray a powerful solvent. Can merge with others to form a more powerful creature.",
"area":LOCATION_TABLE[24]
},
{
"id":"E5602",
"name":"Uno-Bulb",
"boss":0,
"desc":"The result of several Pit-Bulbs fusing into one. Spreads a viscous solvent throughout the surrounding area, vaporizing all living tissue on contact.",
"area":LOCATION_TABLE[24]
},
# --- Iris ---
{
"id":"E5700",
"name":"Togone",
"boss":0,
"desc":"Has the face of a male demon and represents the repulsive side of humanity. Pulverizes the heads of its foes with its powerful jaw.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5701",
"name":"Sonarm",
"boss":0,
"desc":"A strange creature with the face of a female demon. Spits a rainbow-colored gas which steals the energy of those it touches.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5750",
"name":"Sidgodde",
"boss":0,
"desc":"A bronze pedestal modeled on a woman with wings. Shoots light arrows from the eye on its lower half to repel intruders.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5702",
"name":"Totoankh",
"boss":0,
"desc":"Object that warps through space and patrols the area. Fires controllable magic bullets to crush its targets.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5800",
"name":"Shehazask",
"boss":0,
"desc":"An armored knight with claws in both hands. Can cut enemies in half in addition to firing magical spheres that compress space.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5801",
"name":"Vezfes",
"boss":0,
"desc":"An eerie statue with three faces. Each of the faces uses different kinds of magic based on the weather in the surrounding area.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5851",
"name":"Galba-Muva",
"boss":0,
"desc":"Virtual image of a living weapon, created from records of an ancient battle. Fires small bombs to cover the surrounding area in blue flames.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5900",
"name":"Rudogodde",
"boss":1,
"desc":"A golden pedestal modeled on a woman with wings. Fires orbs of light to repel intruders.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5950",
"name":"Nel-Vesbye",
"boss":1,
"desc":"A giant spider created from records of the past. Captures its prey with strong, sticky silk.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5951",
"name":"Nel-Situ",
"boss":1,
"desc":"An aquatic monster created from records of the past. Possesses sharp claws that make it particularly dangerous when it attacks as part of a group.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5952",
"name":"Nel-Plera",
"boss":1,
"desc":"An armored knight created from records of the past. Suppresses its enemies using beams of light from its eyes and its powerful axe.",
"area":LOCATION_TABLE[25]
},
{
"id":"E5953",
"name":"Nel-Beever",
"boss":1,
"desc":"A demi-human created from records of the past. Attacks in groups using a combination of arrows and knives.",
"area":LOCATION_TABLE[25]
},
{
"id":"E6000",
"name":"Zenoranpa",
"boss":0,
"desc":"Armored soldier that patrols Iris' Central Area. Spins a spear at high speeds around its feet to repel its enemies.",
"area":LOCATION_TABLE[25]
},
{
"id":"E6001",
"name":"Jadelos",
"boss":0,
"desc":"An armored cavalier created by the Akashic Records for protection. Guards the Central Area, and is regulated by the Mask of the Sun.",
"area":LOCATION_TABLE[25]
},
# --- Mt. Vesuvio - Volcano ---
{
"id":"E7007",
"name":"Hadugora",
"boss":0,
"desc":"A 'shadow' created when distortions in the Akashic Records began interfering with the real world. Its existence indicates how much damage was done.",
"area":LOCATION_TABLE[26]
},
{
"id":"B099",
"name":"Shadow Gruda",
"boss":0,
"desc":"Gruda's will and tenacity given form as a shadow. Blindly seeks the Mask of the Sun, endlessly spawning more shadows.",
"area":LOCATION_TABLE[26]
},
#--- Bosses ---
{
"id":"B9002",
"name":"[Awakened Armored Beast]Aldovoss",
"boss":1,
"desc":"A monster with a sturdy shell that appeared from deep within the mine. Pulverizes those who draw near with its powerful arms.",
"area":LOCATION_TABLE[27]
},
{
"id":"B901",
"name":"[Purgatorial Hornet]Melldrabie",
"boss":1,
"desc":"A sadistic queen bee. Brandishes needles with such size that they can gouge the earth, and uses them to harass and torment the weak.",
"area":LOCATION_TABLE[27]
},
{
"id":"B000",
"name":"[Great Squilla of Extermination]Glucarius",
"boss":1,
"desc":"King of beetles and master of the Ancient Burrow, it has slept in the depths since antiquity. Its spin attacks are so powerful, they damage even the walls.",
"area":LOCATION_TABLE[27]
},
{
"id":"B050",
"name":"[Serpent of the Rapids]Volnake",
"boss":1,
"desc":"A giant serpent that inhabits the bottom of the lake. Stores large amounts of water in its body and releases it like arrows to overwhelm its foes.",
"area":LOCATION_TABLE[27]
},
{
"id":"B001",
"name":"[Gluttonous Tyrant]Angoraboras",
"boss":1,
"desc":"A large fish that serves as the master of the river bottom. Uses its many cannons to exert control over its foes, and engorges prey with its large mouth.",
"area":LOCATION_TABLE[27]
},
{
"id":"B002",
"name":"[Blue Cavalier]Elvaron",
"boss":1,
"desc":"A mechanical knight created in ancient times. Awoken from its long slumber, it regards anything that moves as a target to be eliminated.",
"area":LOCATION_TABLE[27]
},
{
"id":"E5321",
"name":"Zandabal",
"boss":1,
"desc":"An autonomous sphere-shaped artifact. Surrounds itself with lightning and pursues invaders.",
"area":LOCATION_TABLE[27]
},
{
"id":"E5320",
"name":"Litonileah",
"boss":1,
"desc":"An artifact that supplies electrical energy. Has no combat abilities, but protects itself using a magnetic field.",
"area":LOCATION_TABLE[27]
},
{
"id":"B003",
"name":"[Guardian of the Storms]Krell-Emecia",
"boss":1,
"desc":"A magical machine in charge of protecting the tower. Uses the electrical energy within it to eliminate intruders.",
"area":LOCATION_TABLE[27]
},
{
"id":"B0033",
"name":"[Guardian of the Storms]Krell-Emecia",
"boss":1,
"desc":"The final form of the tower's chief guardian. Supplied with vast amounts of electrical energy by the artifact at its core.",
"area":LOCATION_TABLE[27]
},
{
"id":"B0042",
"name":"[White Galbalan]Sol-Galba",
"boss":1,
"desc":"A Galbalan passed down through Leeza's family line. Possesses such immense power that it can cut through space, and cannot be harmed by conventional weapons.",
"area":LOCATION_TABLE[27]
},
{
"id":"B052",
"name":"[Master of the Grotto]Ribbi-Roi",
"boss":1,
"desc":"A giant frog that's made its home in the grotto. Despite its arrogance, impudence and gluttony, it has earned the unwavering trust from the other Ribbi.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7002",
"name":"Polestol",
"boss":1,
"desc":"A giant mushroom which has grown at an alarming rate due to the spores in the air. Creates a large explosion when impacted.",
"area":LOCATION_TABLE[27]
},
{
"id":"B005",
"name":"[Deformed Nightmare]Mustrum",
"boss":1,
"desc":"Formed by numerous other monsters fused together, and so repulsive that one could faint just by looking at it. Uses defeated foes as a seedbed.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7000",
"name":"Mesarabadi",
"boss":1,
"desc":"A bat familiar created through Bami's magic. Restrains enemies using supersonic waves.",
"area":LOCATION_TABLE[27]
},
{
"id":"B053",
"name":"[Enchanting Magician]Bami",
"boss":1,
"desc":"A magician in service to Gruda, who's harnessed the power of a mask artifact to gain immense magical ability. Uses her magic to immolate the opposition.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7001",
"name":"Dagbanther",
"boss":1,
"desc":"A ferocious beast summoned by Gadis. Follows orders without question, and tears the throats of its enemies.",
"area":LOCATION_TABLE[27]
},
{
"id":"B054",
"name":"[Sadistic Beast Tamer]Gadis",
"boss":1,
"desc":"A beast tamer who manipulates creatures into servitude. Used the mask's power to become cruelty incarnate, slaughtering the weak with his raw power.",
"area":LOCATION_TABLE[27]
},
{
"id":"B004",
"name":"[Muddled Galbalan]Sol-Galba",
"boss":1,
"desc":"A living weapon passed down through Leeza's family line. Out of control, it seeks to burn everything to ashes.",
"area":LOCATION_TABLE[27]
},
{
"id":"B006",
"name":"[Black-Winged God]Eldeel",
"boss":1,
"desc":"The other 'persona' dwelling within Eldeel. Brandishes a long, jet-black spear and seeks to give humanity numerous trials to overcome.",
"area":LOCATION_TABLE[27]
},
{
"id":"B055",
"name":"[Bronze Colossus]Girguzalm",
"boss":1,
"desc":"Ancient knight that guards the Bronze Area. Its giant sword can cut through anything, and it carves up those who approach until nothing remains.",
"area":LOCATION_TABLE[27]
},
{
"id":"B056",
"name":"[Silver Colossus]Zerivutz",
"boss":1,
"desc":"Ancient knight that guards the Silver Area. Its core is made of highly condensed magic, allowing it to cast extremely powerful spells.",
"area":LOCATION_TABLE[27]
},
{
"id":"B057",
"name":"[Golden Colossus]Bulaghoul",
"boss":1,
"desc":"Ancient knight that guards the Gold Area. Its fists have power beyond human recognition, allowing it to pulverize all intruders.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7003",
"name":"Kut-Shell",
"boss":1,
"desc":"One of the three artifacts which form the force field around the Akashic Records. Has a crimson gem embedded within it.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7004",
"name":"Blo-Shell",
"boss":1,
"desc":"One of the three artifacts which form the force field around the Akashic Records. Has a amber gem embedded within it.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7005",
"name":"Shu-Shell",
"boss":1,
"desc":"One of the three artifacts which form the force field around the Akashic Records. Has a emerald green gem embedded within it.",
"area":LOCATION_TABLE[27]
},
{
"id":"E7009",
"name":"Forberis",
"boss":1,
"desc":"A monument containing a portion of the Akashic Records. Paralyzes the body and mind of a foe by transferring more information than they can handle.",
"area":LOCATION_TABLE[27]
},
{
"id":"B007",
"name":"[False God of Causality]Akasha-Glyph",
"boss":1,
"desc":"The result of Gruda absorbing the contents of the Akashic Records through his extreme tenacity. Has the ability to rewrite the world at will.",
"area":LOCATION_TABLE[27]
},
{
"id":"B008",
"name":"[Hollow Specter]Phantom Gruda",
"boss":1,
"desc":"An aggregation of multiple Gruda shadows. His insane tenacity amplifies to the limit, he is controlled solely by his own hatred.",
"area":LOCATION_TABLE[27]
},
]


# Note - mons_id is added for error handling to easily spot which lines are too long.
def insert_linebreaks(instr,mons_id):
	if(len(instr) > LINE_MAX * NUM_LINES):
		print("WARN: ID %s text is longer than allowed. (%d / %d)" % (mons_id,len(instr),LINE_MAX*NUM_LINES))
	if(len(instr) < LINE_MAX):
		return instr # We don't need linebreaks if it's shorter than one line.

	return "\\n".join(textwrap.wrap(instr,width=LINE_MAX))




def get_monsters_in_area(location):
	monsters_in_area = []
	for i in range(0,len(MONSTER_TABLE)):
		if(MONSTER_TABLE[i]["area"] == location):
			monsters_in_area.append(MONSTER_TABLE[i])
	return monsters_in_area

if(__name__=="__main__"):
	str_lines = []
	for i in range(0,len(LOCATION_TABLE)):
		str_lines.append("AREA")
		str_lines.append(LOCATION_TABLE[i])
		str_lines.append("\x00") # First linebreak after area name - normally blank.
		str_lines.append("\x00") # Second linebreak after area name - normally //mapfile_name
		
		monsters_in_area = get_monsters_in_area(LOCATION_TABLE[i])
		for monster in monsters_in_area:
			str_lines.append(monster["id"])
			str_lines.append(monster["name"].replace("[","\xa1\xb8").replace("]","\xa1\xb9"))
			str_lines.append(str(monster["boss"]))
			str_lines.append(insert_linebreaks(monster["desc"],monster['id']))
			
	
	str_data = ""
	offset_data = ""
	strtab_offsets = []
	with open('test.txt','wb') as e:
		for line in str_lines:
			e.write(line+"\n")
			
	for line in str_lines:
		strtab_offsets.append(len(str_data))
		str_data+=line+"\x00"
	#Fix the rounding shit for the strtab size.

	num_to_pad = 16 - ((len(strtab_offsets)*4) % 16)
	for i in range(0,num_to_pad):
		strtab_offsets.append(0)
	
	header_data = "\x04\x00\x00\x00"+struct.pack("<I",(len(strtab_offsets)*4) >> 4)+struct.pack("<I",len(str_data))	
	for offset in strtab_offsets:
		offset_data += struct.pack("<I",offset)
	print("Done!")
	
	with open("monslib.tbb",'wb') as f:
		f.write(header_data+str_data+offset_data)
