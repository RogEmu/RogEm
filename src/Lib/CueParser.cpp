#include "CueParser.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

namespace cue {

namespace {

static inline std::string trim(std::string s) {
    auto iswhitespace = [](int c) { return std::isspace(c); };
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), iswhitespace));
    s.erase(std::find_if_not(s.rbegin(), s.rend(), iswhitespace).base(), s.end());
    return s;
}

static inline std::string toUpper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

static std::vector<std::string> splitTokens(const std::string &line) {
    std::vector<std::string> out;
    std::string cur;
    bool inQuote = false;
    char quoteChar = 0;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (!inQuote) {
            if (c == '"' || c == '\'') {
                inQuote = true; quoteChar = c;
                cur.clear();
                continue;
            }
            if (std::isspace((unsigned char)c)) {
                if (!cur.empty()) { out.push_back(cur); cur.clear(); }
                continue;
            }
            cur.push_back(c);
        } else {
            if (c == quoteChar) {
                out.push_back(cur);
                cur.clear();
                inQuote = false;
                quoteChar = 0;
            } else {
                cur.push_back(c);
            }
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

}

int Cue::timeStrToFrames(const std::string &timeStr, bool &ok) {
    ok = false;
    int mm=0, ss=0, ff=0;
    char sep1=0, sep2=0;
    std::istringstream iss(timeStr);
    if ((iss >> mm >> sep1 >> ss) && (sep1 == ':' || sep1 == '.')) {
        if (iss.peek() == ':' || iss.peek() == '.') {
            iss >> sep2;
        }
        if (!(iss >> ff)) {
            ff = 0;
        }
    } else {
        return 0;
    }
    if (mm < 0 || ss < 0 || ss >= 60 || ff < 0) return 0;
    if (ff > 74 && sep1 == '.') {
        ff = static_cast<int>((ff * 75 + 50) / 100);
    }
    ok = true;
    return (mm * 60 + ss) * 75 + ff;
}

void Cue::parseREM(Cue &c, const std::vector<std::string> &toks, const std::string &line, int lineNo) {
    if (toks.size() >= 3) {
        size_t pos = line.find(toks[1]);
        c.rem[toks[1]] = (pos != std::string::npos) ? trim(line.substr(pos + toks[1].size())) : "";
    } else if (toks.size() == 2) c.rem[toks[1]] = "";
    else c.rem["comment" + std::to_string(lineNo)] = line.substr(4);
}

void Cue::parseTITLE(Cue &c, CueTrack *currentTrack, const std::vector<std::string> &toks, const std::string &line) {
    if (toks.size() < 2) {
        if (!currentTrack) c.title = std::string();
        else currentTrack->title = std::string();
        return;
    }
    size_t pos = line.find_first_not_of(" \t", line.find("TITLE") + 5);
    if (pos != std::string::npos) {
        std::string value = trim(line.substr(pos));
        if ((value.front() == '"' && value.back() == '"') ||
            (value.front() == '\'' && value.back() == '\'')) {
            value = value.substr(1, value.size() - 2);
        }
        if (!currentTrack) c.title = value;
        else currentTrack->title = value;
    }
}

void Cue::parsePERFORMER(Cue &c, CueTrack *currentTrack, const std::vector<std::string> &toks, const std::string &line) {
    if (toks.size() < 2) {
        if (!currentTrack) c.performer = std::string();
        else currentTrack->performer = std::string();
        return;
    }
    size_t pos = line.find_first_not_of(" \t", line.find("PERFORMER") + 9);
    if (pos != std::string::npos) {
        std::string value = trim(line.substr(pos));
        if ((value.front() == '"' && value.back() == '"') ||
            (value.front() == '\'' && value.back() == '\'')) {
            value = value.substr(1, value.size() - 2);
        }
        if (!currentTrack) c.performer = value;
        else currentTrack->performer = value;
    }
}

bool Cue::parseFILE(Cue &c, const std::vector<std::string> &toks, int &currentFileIndex, ParseError &err, int lineNo) {
    if (toks.size() < 3) { err = {lineNo, "Malformed FILE directive"}; return false; }
    CueFile cf{toks[1], toks[2], {}};
    c.files.push_back(cf);
    currentFileIndex = static_cast<int>(c.files.size()) - 1;
    return true;
}

bool Cue::parseTRACK(Cue &c, const std::vector<std::string> &toks, int currentFileIndex, CueTrack *&currentTrack, ParseError &err, int lineNo) {
    if (toks.size() < 3) { err = {lineNo, "Malformed TRACK directive"}; return false; }
    CueTrack t;
    t.number = std::stoi(toks[1]);
    t.type = toks[2];
    t.fileIndex = currentFileIndex;
    c.tracks.push_back(t);
    currentTrack = &c.tracks.back();
    if (currentFileIndex >= 0) c.files[currentFileIndex].trackNumbers.push_back(t.number);
    return true;
}

bool Cue::parseINDEX(CueTrack *currentTrack, const std::vector<std::string> &toks, ParseError &err, int lineNo) {
    if (!currentTrack) { err = {lineNo, "INDEX without active TRACK"}; return false; }
    if (toks.size() < 3) { err = {lineNo, "Malformed INDEX directive"}; return false; }
    CueIndex ix;
    ix.number = std::stoi(toks[1]);
    bool ok;
    ix.frames = timeStrToFrames(toks[2], ok);
    if (!ok) { err = {lineNo, "Malformed time in INDEX"}; return false; }
    currentTrack->indexes.push_back(ix);
    return true;
}

bool Cue::parsePREGAP(CueTrack *currentTrack, const std::vector<std::string> &toks, ParseError &err, int lineNo) {
    if (!currentTrack) { err = {lineNo, "PREGAP without active TRACK"}; return false; }
    if (toks.size() < 2) { err = {lineNo, "Malformed PREGAP"}; return false; }
    bool ok;
    int frames = timeStrToFrames(toks[1], ok);
    if (!ok) { err = {lineNo, "Malformed PREGAP time"}; return false; }
    currentTrack->pregapFrames = frames;
    return true;
}

bool Cue::parsePOSTGAP(CueTrack *currentTrack, const std::vector<std::string> &toks, ParseError &err, int lineNo) {
    if (!currentTrack) { err = {lineNo, "POSTGAP without active TRACK"}; return false; }
    if (toks.size() < 2) { err = {lineNo, "Malformed POSTGAP"}; return false; }
    bool ok;
    int frames = timeStrToFrames(toks[1], ok);
    if (!ok) { err = {lineNo, "Malformed POSTGAP time"}; return false; }
    currentTrack->postgapFrames = frames;
    return true;
}

void Cue::parseISRC(CueTrack *currentTrack, const std::vector<std::string> &toks) {
    if (currentTrack && toks.size() >= 2) currentTrack->isrc = toks[1];
}

void Cue::parseFLAGS(CueTrack *currentTrack, const std::vector<std::string> &toks) {
    if (currentTrack && toks.size() >= 2) currentTrack->flags = toks[1];
}

void Cue::parseSONGWRITER(Cue &c, CueTrack *currentTrack, const std::vector<std::string> &toks, const std::string &line) {
    size_t pos;
    if (!currentTrack) {
        if (!toks.empty() && (pos = line.find_first_not_of(" \t", line.find("SONGWRITER") + 10)) != std::string::npos)
            c.rem["SONGWRITER"] = trim(line.substr(pos));
    } else {
        if (toks.size() >= 2 && (pos = line.find_first_not_of(" \t", line.find("SONGWRITER") + 10)) != std::string::npos)
            currentTrack->songwriter = trim(line.substr(pos));
    }
}


std::pair<Cue, std::optional<ParseError>> Cue::parseFromString(const std::string &text) {
    Cue c;
    ParseError err;
    std::istringstream in(text);
    std::string raw;
    int lineNo = 0;
    int currentFileIndex = -1;
    CueTrack* currentTrack = nullptr;

    while (std::getline(in, raw)) {
        ++lineNo;
        std::string line = trim(raw);
        if (line.empty()) continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto toks = splitTokens(line);
        if (toks.empty()) continue;

        std::string cmd = toUpper(toks[0]);
        try {
            if (cmd == "REM")
                parseREM(c, toks, line, lineNo);
            else if (cmd == "TITLE")
                parseTITLE(c, currentTrack, toks, line);
            else if (cmd == "PERFORMER")
                parsePERFORMER(c, currentTrack, toks, line);
            else if (cmd == "FILE" && !parseFILE(c, toks, currentFileIndex, err, lineNo))
                return {c, err};
            else if (cmd == "TRACK" && !parseTRACK(c, toks, currentFileIndex, currentTrack, err, lineNo))
                return {c, err};
            else if (cmd == "INDEX" && !parseINDEX(currentTrack, toks, err, lineNo))
                return {c, err};
            else if (cmd == "PREGAP" && !parsePREGAP(currentTrack, toks, err, lineNo))
                return {c, err};
            else if (cmd == "POSTGAP" && !parsePOSTGAP(currentTrack, toks, err, lineNo))
                return {c, err};
            else if (cmd == "ISRC")
                parseISRC(currentTrack, toks);
            else if (cmd == "FLAGS")
                parseFLAGS(currentTrack, toks);
            else if (cmd == "SONGWRITER")
                parseSONGWRITER(c, currentTrack, toks, line);
            else {
                if (toks.size() >= 2) {
                    size_t pos = line.find_first_not_of(" \t", line.find(toks[0]) + toks[0].size());
                    c.rem[toks[0]] = (pos != std::string::npos) ? trim(line.substr(pos)) : "";
                } else {
                    c.rem["UNHANDLED_" + std::to_string(lineNo)] = line;
                }
            }
        } catch (const std::exception &ex) {
            err.line = lineNo;
            err.message = std::string("Parse exception: ") + ex.what();
            return {c, err};
        }
    }
    return {c, std::nullopt};
}

std::pair<Cue, std::optional<ParseError>> Cue::parseFromFile(const std::string &path) {
    std::ifstream ifs(path, std::ios::in);
    if (!ifs) {
        ParseError err; err.line = -1; err.message = "Failed to open file: " + path;
        return {Cue(), err};
    }
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return parseFromString(ss.str());
}

std::string Cue::toString() const {
    std::ostringstream ss;
    ss << "CUE Sheet Dump\n";
    if (title) ss << "TITLE: " << *title << "\n";
    if (performer) ss << "PERFORMER: " << *performer << "\n";
    for (const auto &r : rem) {
        ss << "REM " << r.first << " : " << r.second << "\n";
    }
    ss << "Files: " << files.size() << "\n";
    for (size_t i = 0; i < files.size(); ++i) {
        ss << " [" << i << "] PATH: " << files[i].path << " TYPE: " << files[i].fileType << "\n";
    }
    ss << "Tracks: " << tracks.size() << "\n";
    for (const auto &t : tracks) {
        ss << " Track " << std::setw(2) << std::setfill('0') << t.number << " (" << t.type << ")\n";
        if (t.title) ss << "   TITLE: " << *t.title << "\n";
        if (t.performer) ss << "   PERFORMER: " << *t.performer << "\n";
        if (t.songwriter) ss << "   SONGWRITER: " << *t.songwriter << "\n";
        if (t.isrc) ss << "   ISRC: " << *t.isrc << "\n";
        if (t.flags) ss << "   FLAGS: " << *t.flags << "\n";
        ss << "   file_index: " << t.fileIndex << "\n";
        ss << "   PREGAP: " << t.pregapFrames << " frames\n";
        ss << "   POSTGAP: " << t.postgapFrames << " frames\n";
        ss << "   Indexes: ";
        for (const auto &ix : t.indexes) {
            ss << "(" << ix.number << ":" << ix.frames << ") ";
        }
        ss << "\n";
    }
    return ss.str();
}

}
