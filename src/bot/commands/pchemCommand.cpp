#include "pchemCommand.h"
#include "../bot.h"

#include <restclient-cpp/restclient.h>

const std::string REST_URL = "https://pubchem.ncbi.nlm.nih.gov/rest/pug";
const std::string VIEW_URL = "https://pubchem.ncbi.nlm.nih.gov/rest/pug_view";

void PchemCommand::call(std::vector<std::string> parameters, MessageInfo current) {
    Command::call(parameters, current);

    if(parameters.size() < 1) {
        _aegisCore->create_message(_current.channelId, "You need to enter a query.");
        return;
    }

    std::string query;
    auto param = parameters.begin();
    while (param != parameters.end()) {
        query += *param;
        if(param != --parameters.end()) {
            query += "%20";
        }
        param++;
    }

    _aegisCore->find_channel(_current.channelId)->create_reaction(aegis::create_reaction_t().message_id(_current.messageId).emoji_text("%E2%9C%85"));

    std::string cid;
    try {
        std::stoi(query); // throws if query couldn't be parsed as a number
        cid = query;
    } catch (std::exception& e) {
        // not a CID, try searching for it
        cid = getCID(query);
    }
    
    if(cid == "") {
        _aegisCore->create_message(_current.channelId, "Couldn't find that compound. Try entering a CID or name.");
        return;
    }

    PCResult res = getInfo(cid);

    std::string ghsStatements;
    for(std::string message : res.ghsMessages) {
        ghsStatements += message + "\n\n";
    }
    if(res.ghsMessages.size() < 1) ghsStatements = "None";
    if(ghsStatements.length() > 2048) ghsStatements = "Text is too long for discord.";

    std::string warningLabels;
    for(std::string label : res.warningLabels) {
        warningLabels += label + ",";
    }
    warningLabels = warningLabels.substr(0, warningLabels.length()-1);
    if(res.warningLabels.size() < 1) warningLabels = "None";

    if(res.precautions.length() < 1) res.precautions = "None";
    if(res.molMass.length() < 1) res.molMass = "Not found";

    nlohmann::json embed {
        {"title", "Information for " + res.name},
        {"thumbnail", {
            {"url", res.structUrl}
        }},
        {"fields", json::array({
            {
                {"name", "Precautionary Statement Codes"},
                {"value", res.precautions}
            },
            {
                {"name", "Pictograms"},
                {"value", warningLabels}
            },
            {
                {"name", "Molecular Mass"},
                {"value", res.molMass + " g/mol"}
            }
        })},
        {"url", "https://pubchem.ncbi.nlm.nih.gov/compound/" + cid},
        {"description", ghsStatements},
        {"footer", {{"text", "GHS reference: " + res.ghsReference}}}
    };

    auto response = _aegisCore->create_message_embed(_current.channelId, "", embed);

}

std::string PchemCommand::getCID(std::string query) {
    auto r = RestClient::get(REST_URL + "/compound/name/" + query + "/cids/JSON");
    if(r.code != 200) {
        return "";
    }

    nlohmann::json body = json::parse(r.body);
    int cid = body["IdentifierList"]["CID"][0];
    return std::to_string(cid);
}

PCResult PchemCommand::getInfo(std::string cid) {
    PCResult res;
    res.structUrl = REST_URL + "/compound/cid/" + cid + "/PNG";

    nlohmann::json body;
    nlohmann::json record;

    auto r = RestClient::get(VIEW_URL + "/data/compound/" + cid + "/JSON?heading=Computed+Properties");

    body = json::parse(r.body);
    record = body["Record"];
    res.molMass = std::to_string((int)record["Section"][0]["Section"][0]["Section"][0]["Information"][0]["Value"]["Number"][0]);

    res.name = record["RecordTitle"];
    res.cid = std::to_string((int)record["RecordNumber"]);

    r = RestClient::get(VIEW_URL + "/data/compound/" + cid + "/JSON?heading=GHS+Classification");

    if(r.code != 200) {
        res.ghsMessages.emplace_back("None");
        res.ghsReference = "None";
        res.precautions = "None";
        res.warningLabels.emplace_back("None");
        return res;
    }

    body = json::parse(r.body);
    record = body["Record"];

    int refNumber = record["Reference"][0]["ReferenceNumber"];
    res.ghsReference = record["Reference"][0]["SourceName"];

    nlohmann::json ghsInformation = record["Section"][0]["Section"][0]["Section"][0]["Information"];
    for(auto info : ghsInformation) {
        if(info["ReferenceNumber"] != refNumber) continue;

        std::string infoName = info["Name"];
        nlohmann::json value = info["Value"];
        if(infoName == "Pictogram(s)") {
            for (auto val : value["StringWithMarkup"][0]["Markup"]) {
                res.warningLabels.emplace_back(val["Extra"]);
            }
        } else if (infoName == "GHS Hazard Statements") {
            for (auto val : value["StringWithMarkup"]) {
                res.ghsMessages.emplace_back(val["String"]);
            }
        } else if (infoName == "Precautionary Statement Codes") {
            res.precautions = value["StringWithMarkup"][0]["String"];
        }
    }

    return res;

}

bool PchemCommand::checkPermissions(aegis::permission channelPermissions) {
    if(!channelPermissions.can_embed()) {
        return false;
    }
    return true;
}

CommandInfo PchemCommand::getCommandInfo() {
    return {
        "pubchem",
        {"pchem", "pc"},
        "Look up some basic information about a compound on pubchem",
        {}
    };
}