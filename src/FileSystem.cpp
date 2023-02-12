#include <FileSystem.h>

void FSBegin()
{
    LittleFS.begin();
}

void GetWifiSettings(String *data)
{
    File file = LittleFS.open(WIFI_SETTINGS_PATH, "r");

    data[0] = file.readStringUntil('\n');
    data[1] = file.readString();

    data[0].trim();
    data[1].trim();

    file.close();
}

void SavePreferences(String preferences_json)
{
    File file = LittleFS.open(PREFRENCES_PATH, "w");
    file.write(preferences_json.c_str());
    file.close();
}

String GetPreferences()
{
    File file = LittleFS.open(PREFRENCES_PATH, "r");
    String data = file.readString();
    file.close();
    return data;
}

String GetModeArgs(int id)
{
    File file = LittleFS.open("/mode" + String(id) + ".json", "r");
    String data = file.readString();
    file.close();
    return data;
}

void SaveModeArgs(int id, String json)
{
    File file = LittleFS.open("/mode" + String(id) + ".json", "w");
    file.write(json.c_str());
    file.close();
}