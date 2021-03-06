#include <cbpch.h>

#include <bot/localization/localization.h>
#include <bot/commands/pchemCommand.h>
#include <bot/bot.h>

const std::string HOST = "pubchem.ncbi.nlm.nih.gov";
const std::string REST_URI = "/rest/pug";
const std::string VIEW_URI = "/rest/pug_view";

void PchemCommand::call(int verb, const std::vector<std::string>& parameters, CommandContext* ctx) {
    if(parameters.size() < 1) {
        ctx->respond("pubchem_enter_query");
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

    ctx->waitTyping();

    std::string cid;
    try {
        std::stoi(query); // throws if query couldn't be parsed as a number
        cid = query;
    } catch (std::exception& e) {
        // not a CID, try searching for it
        cid = getCID(query);
    }
    
    if(cid == "") {
        ctx->respond("pubchem_comp_not_found");
        return;
    }

    PCResult res = getInfo(cid);

    LocHelper loc(m_Bot->getLocalization(), ctx->getSettings().lang);

    std::string ghsStatements;
    for(std::string message : res.ghsMessages) {
        ghsStatements += message + "\n\n";
    }
    if(res.ghsMessages.size() < 1) ghsStatements = loc.get("pubchem_none");
    if(ghsStatements.length() > 2048) ghsStatements = loc.get("pubchem_too_long");

    std::string warningLabels;
    for(std::string label : res.warningLabels) {
        warningLabels += label + ", ";
    }
    warningLabels = warningLabels.substr(0, warningLabels.length()-2);
    if(res.warningLabels.size() < 1) warningLabels = loc.get("pubchem_none");

    if(res.precautions.length() < 1) res.precautions = loc.get("pubchem_none");
    if(res.molMass.length() < 1) res.molMass = loc.get("pubchem_not_found");

    nlohmann::json embed {
        {"title", loc.get("pubchem_info_for") + std::string(" ") + res.name},
        {"thumbnail", {
            {"url", res.structUrl}
        }},
        {"fields", nlohmann::json::array({
            {
                {"name", loc.get("pubchem_formula")},
                {"value", res.formula}
            },
            {
                {"name", loc.get("pubchem_mol_mass")},
                {"value", res.molMass + " g/mol"}
            },
            {
                {"name", loc.get("pubchem_pictograms")},
                {"value", warningLabels}
            },
            {
                {"name", loc.get("pubchem_pcs")},
                {"value", res.precautions}
            }
        })},
        {"url", "https://pubchem.ncbi.nlm.nih.gov/compound/" + cid},
        {"description", ghsStatements},
        {"footer", {{"text", loc.get("pubchem_ghs_ref") + std::string(": ") + res.ghsReference}}}
    };

    ctx->respondEmbedUnlocalized("", embed);
}

std::string PchemCommand::getCID(const std::string& query) {
    Poco::Net::HTTPSClientSession session(HOST, 443);
    Poco::Net::HTTPRequest req("GET", REST_URI + "/compound/name/" + query + "/cids/JSON");

    session.sendRequest(req);
    Poco::Net::HTTPResponse res;
    std::string bodyString(std::istreambuf_iterator<char>(session.receiveResponse(res)), {});

    if(res.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK) {
        return "";
    }

    nlohmann::json body = nlohmann::json::parse(bodyString);
    int cid = body["IdentifierList"]["CID"][0];
    return std::to_string(cid);
}

PCResult PchemCommand::getInfo(const std::string& cid) {
    PCResult res;
    Poco::Net::HTTPSClientSession session(HOST, 443);
    res.structUrl = "https://" + HOST + REST_URI + "/compound/cid/" + cid + "/PNG";

    nlohmann::json body;
    nlohmann::json record;

    Poco::Net::HTTPRequest req("GET", VIEW_URI + "/data/compound/" + cid + "/JSON?heading=Computed+Properties");

    session.sendRequest(req);
    Poco::Net::HTTPResponse response;
    std::string bodyString(std::istreambuf_iterator<char>(session.receiveResponse(response)), {});

    body = nlohmann::json::parse(bodyString);
    record = body["Record"];
    res.molMass = std::to_string((int)record["Section"][0]["Section"][0]["Section"][0]["Information"][0]["Value"]["Number"][0]);

    res.name = record["RecordTitle"];
    res.cid = std::to_string((int)record["RecordNumber"]);

    req.setURI(VIEW_URI + "/data/compound/" + cid + "/JSON?heading=GHS+Classification");

    session.sendRequest(req);
    bodyString = std::string(std::istreambuf_iterator<char>(session.receiveResponse(response)), {});

    if(response.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK) {
        res.ghsMessages.emplace_back("None");
        res.ghsReference = "None";
        res.precautions = "None";
        res.warningLabels.emplace_back("None");
        return res;
    }

    body = nlohmann::json::parse(bodyString);
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

    req.setURI(VIEW_URI + "/data/compound/" + cid + "/JSON?heading=Molecular+Formula");

    session.sendRequest(req);
    bodyString = std::string(std::istreambuf_iterator<char>(session.receiveResponse(response)), {});

    body = nlohmann::json::parse(bodyString);

    res.formula = body["Record"]["Section"][0]["Section"][0]["Information"][0]["Value"]["StringWithMarkup"][0]["String"];

    return res;
}

CommandInfo PchemCommand::getCommandInfo() {
    return {
        "pubchem",
        "pubchem_cmd",
        {"pubchem_alias"},
        "pubchem_desc",
        {
            "pubchem_option_desc"
        },
        {},
        false
    };
}