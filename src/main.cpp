#include <truecontext/app/application.hpp>

int main(int argc, char* argv[]) {
    truecontext::app::Application app{argc, argv};
    return app.run();
}
