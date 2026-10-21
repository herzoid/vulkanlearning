#include "application.hpp"
// #include <external/CLI11.hpp>
#include <fstream>
#include <external/json.hpp>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

int main(int argc, char const *argv[]) {

  // CLI::App appCli;
  std::string configPath = "../config.json";
  // appCli.add_option("--config", path, "config filename");
  // appCli.parse(argc, argv);

  std::ifstream f(configPath);
  if (!f.is_open()) {
    throw std::runtime_error("failed to open config file!");
  }
  std::string modelPath;
  try {
    json data = json::parse(f);
    auto modelPathValue = data.at("/model"_json_pointer);
    modelPath = modelPathValue;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  HelloTriangleApplication app(modelPath);
  app.setWindowSize(800, 600);

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
