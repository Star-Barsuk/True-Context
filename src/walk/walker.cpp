#include <truecontext/walk/walker.hpp>

#include <truecontext/ignore/matcher.hpp>
#include <truecontext/util/path.hpp>

#include <cstdio>
#include <cstring>

#include <dirent.h>
#include <sys/stat.h>

namespace truecontext::walk {

namespace {

bool is_directory(const char* abs_path) {
    struct stat st {};
    return stat(abs_path, &st) == 0 && S_ISDIR(st.st_mode);
}

void walk_recursive(
    const char* root_abs,
    const char* rel,
    const ignore::Matcher& matcher,
    PathList& out
) {
    char dir_abs[4096];
    if (rel[0] == '\0') {
        std::strncpy(dir_abs, root_abs, sizeof(dir_abs) - 1U);
        dir_abs[sizeof(dir_abs) - 1U] = '\0';
    } else if (!util::path_join(dir_abs, sizeof(dir_abs), root_abs, rel)) {
        return;
    }

    DIR* dir = ::opendir(dir_abs);
    if (dir == nullptr) {
        return;
    }

    while (const dirent* entry = ::readdir(dir)) {
        const char* name = entry->d_name;
        if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0'))) {
            continue;
        }

        char child_rel[4096];
        if (rel[0] == '\0') {
            std::strncpy(child_rel, name, sizeof(child_rel) - 1U);
        } else if (!util::path_join(child_rel, sizeof(child_rel), rel, name)) {
            continue;
        }
        child_rel[sizeof(child_rel) - 1U] = '\0';

        char child_abs[4096];
        if (!util::path_join(child_abs, sizeof(child_abs), root_abs, child_rel)) {
            continue;
        }

        if (is_directory(child_abs)) {
            if (!matcher.should_prune_dir(child_rel)) {
                walk_recursive(root_abs, child_rel, matcher, out);
            }
            continue;
        }

        if (!matcher.should_ignore(child_rel)) {
            out.push_copy(child_rel);
        }
    }

    ::closedir(dir);
}

} // namespace

Walker::Walker(const char* root_abs) noexcept : root_abs_{root_abs} {}

PathList Walker::collect() const {
    PathList files;
    if (root_abs_ == nullptr || root_abs_[0] == '\0') {
        return files;
    }

    ignore::Matcher matcher;
    matcher.load_from_project(root_abs_);

    walk_recursive(root_abs_, "", matcher, files);
    return files;
}

} // namespace truecontext::walk
