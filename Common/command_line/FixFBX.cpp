#include "DzFbxUtils.h"
#include "DTUConfig.h"
#include "debug.h"

#include "CLI/App.hpp"
#include "CLI/Formatter.hpp"
#include "CLI/Config.hpp"

#include <fmt/core.h>
#include <fmt/color.h>

#include "simdjson.h"
// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

#define VERSION 1.1

int main(int argc, char **argv)
{
  std::string dtuPath = "default.dtu";

  CLI::App app{fmt::format(fg(fmt::color::green_yellow), "DAZ FBX Fix {}", VERSION)};
  app.require_subcommand();

  CLI::App *fix = app.add_subcommand("fix", "Fix DAZ issues from a DTU file (Subdivision, Joint orientation, Material names)");

  fix->add_option("-i,--input", dtuPath, "The basemesh obj to diff from")->required(true)->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  if (fix->parsed())
  {
    DTUConfig config;

    // std::ifstream ifs(dtuPath);
    // json jf = json::parse(ifs);
    // config.from_json(jf);
    // DzFbxUtils::PostExport(config);
    DEBUG("Done");
  }
}
