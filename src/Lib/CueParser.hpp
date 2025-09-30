#include <string>
#include <vector>
#include <optional>
#include <map>

namespace cue {

struct CueIndex {
    int number = 1;      // index number (usually 0 or 1)
    int frames = 0;      // absolute frames from start of referenced file (75 frames/sec)
};

struct CueTrack {
    int number = 0;                 // track number
    std::string type;               // "AUDIO", "MODE1/2352", etc.
    std::optional<std::string> title;
    std::optional<std::string> performer;
    std::optional<std::string> songwriter;
    std::optional<std::string> isrc;
    std::optional<std::string> flags; // e.g. "DCP"
    int pregapFrames = 0;
    int postgapFrames = 0;
    int fileIndex = -1;            // which file in Cue.files this track belongs to
    std::vector<CueIndex> indexes;
};

struct CueFile {
    std::string path;               // the filename quoted or not
    std::string fileType;          // e.g. WAVE, MP3
    std::vector<int> trackNumbers; // tracks that reference this file
};

struct ParseError {
    int line = -1;
    std::string message;
};

struct Cue {
    // global meta
    std::optional<std::string> title;
    std::optional<std::string> performer;
    std::map<std::string, std::string> rem; // other REM comments keyed by first token
    std::vector<CueFile> files;
    std::vector<CueTrack> tracks;

    // Parse functions
    static std::pair<Cue, std::optional<ParseError>> parseFromString(const std::string &text);
    static std::pair<Cue, std::optional<ParseError>> parseFromFile(const std::string &path);
    static void parseREM(Cue &c, const std::vector<std::string> &toks, const std::string &line, int lineNo);
    static void parseTITLE(Cue &c, CueTrack *currentTrack, const std::vector<std::string> &toks, const std::string &line);
    static void parsePERFORMER(Cue &c, CueTrack *currentTrack, const std::vector<std::string> &toks, const std::string &line);
    static bool parseFILE(Cue &c, const std::vector<std::string> &toks, int &currentFileIndex, ParseError &err, int lineNo);
    static bool parseTRACK(Cue &c, const std::vector<std::string> &toks, int currentFileIndex, CueTrack* &currentTrack, ParseError &err, int lineNo);
    static bool parseINDEX(CueTrack* currentTrack, const std::vector<std::string> &toks, ParseError &err, int lineNo);
    static bool parsePREGAP(CueTrack* currentTrack, const std::vector<std::string> &toks, ParseError &err, int lineNo);
    static bool parsePOSTGAP(CueTrack* currentTrack, const std::vector<std::string> &toks, ParseError &err, int lineNo);
    static void parseISRC(CueTrack* currentTrack, const std::vector<std::string> &toks);
    static void parseFLAGS(CueTrack* currentTrack, const std::vector<std::string> &toks);
    static void parseSONGWRITER(Cue &c, CueTrack* currentTrack, const std::vector<std::string> &toks, const std::string &line);

    // Utilities
    std::string toString() const; // human friendly dump
    static int timeStrToFrames(const std::string &timeStr, bool &ok); // mm:ss:ff -> frames (75fps)
};

} // namespace cue
