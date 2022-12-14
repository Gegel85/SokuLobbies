from unidecode import unidecode
import json

chrs = [
    "Hakurei Reimu",
    "Kirisame Marisa",
    "Izayoi Sakuya",
    "Margatroid Alice",
    "Knowledge Patchouli",
    "Konpaku Youmu",
    "Scarlet Remilia",
    "Saigyouji Yuyuko",
    "Yakumo Yukari",
    "Ibuki Suika",
    "Reisen Udongein Inaba",
    "Shameimaru Aya",
    "Onozuka Komachi",
    "Nagae Iku",
    "Hinanawi Tenshi",
    "Kochiya Sanae",
    "Cirno",
    "Meiling Hong",
    "Reiuji Utsuho",
    "Moriya Suwako"
]
names = [
    "Reimu",
    "Marisa",
    "Sakuya",
    "Alice",
    "Patchouli",
    "Youmu",
    "Remilia",
    "Yuyuko",
    "Yukari",
    "Suika",
    "Reisen",
    "Aya",
    "Komachi",
    "Iku",
    "Tenshi",
    "Sanae",
    "Cirno",
    "Meiling",
    "Utsuho",
    "Suwako"
]
requs = [
    {"type": "win",    "count": 10},
    {"type": "lose",   "count": 10},
    {"type": "play",   "count": 10},
    {"type": "cards"},
    {"type": "play",   "count": 100},
    {"type": "play",   "count": 1000}
]
rewards = [
    {"type": "avatar"},
    {"type": "emote"},
    {"type": "prop"},
    None,
    {"type": "accessory"},
    {"type": "background"},
]
emotes = [
    "reimu2",
    "marisa2",
    "sakuya2",
    "alice2",
    "patchy4",
    "youmu4",
    "remilia4",
    "yuyuko6",
    "yukari6",
    "suika2",
    "reisen4",
    "aya6",
    "komachi4",
    "iku6",
    "tenshi4",
    "sanae6",
    "cirno6",
    "meiling4",
    "utsuho2",
    "suwako2"
]

elems = []
with open("data.txt", encoding="utf-8") as fd:
    lines = [i.strip() for i in fd.read().split("\n")]
    for i in range(len(lines) // 10):
        name = lines[i * 10][1:-1]
        index = chrs.index(name)
        for rid, requ in enumerate(requs):
            name = unidecode(lines[i * 10 + rid + 1]).split(" / ")
            if len(elems):
                result = {
                    "requirement": requ.copy(),
                    "rewards": [
                        {
                            "type": "title",
                            "name": None if len(name) == 1 else name[0]
                        }
                    ],
                    "name": name[-1],
                    "description": (
                        "Use each " + names[index] + "'s skill and spell at<br>least once" if requ["type"] == "cards"
                        else requ["type"].capitalize() + " " + str(requ["count"]) + " games as " + names[index]
                    ),
                    "hidden": False
                }
            else:
                result = {
                    "comment1": "THIS FILE CONTAINS SPOILERS!!",
                    "comment2": "IF YOU WISH TO FIND ACHIEVEMENTS BY YOURSELF CLOSE IT!",
                    "requirement": requ.copy(),
                    "rewards": [
                        {
                            "type": "title",
                            "name": None if len(name) == 1 else name[0]
                        }
                    ],
                    "name": name[-1],
                    "description": (
                        "Use each " + names[index] + "'s skill and spell<br>at least once" if requ["type"] == "cards"
                        else requ["type"].capitalize() + " " + str(requ["count"]) + " games as " + names[index]
                    ),
                    "hidden": False
                }
            rval = None
            if not rewards[rid]:
                pass
            elif rewards[rid]["type"] == "emote":
                rval = {"type": "emote", "name": emotes[index]}
            elif rewards[rid]["type"] == "avatar":
                rval = {"type": "avatar", "id": index + 3}
            else:
                rval = {"type": rewards[rid]["type"], "id": index}
            if rval and rid >= 4:
                rval["comment"] = unidecode(lines[i * 10 + rid + 3])
            result["requirement"]["chr"] = index
            if rval:
                result["rewards"].insert(0, rval)
            elems.append(result)

with open("achievements.json", "w") as fd:
    json.dump(elems, fd, indent=4)
