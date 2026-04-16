#include "core/AppConfig.hpp"
#include "core/AppController.hpp"

int main()
{
    AppConfig config;
    // All defaults are set in AppConfig.hpp.
    // To change the starting camera or thresholds, modify AppConfig fields here.

    AppController controller(config);
    controller.run();

    return 0;
}
