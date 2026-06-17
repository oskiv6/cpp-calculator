#define NOB_IMPLEMENTATION
#include "nob.h"

#define BUILD_DIR   "build"
#define OBJ_DIR     BUILD_DIR "/obj"

const char* sources[] = {
    "src/Main.cpp",
    "src/core/Lexer.cpp",
    "src/core/Parser.cpp",
};

const char* flags[] = {
    "-std=c++17",
    "-ggdb",
    "-g",
    "-O0",
    "-I", "src",
};

// ====

const char *get_compiler(void) {
    if (system("clang++ --version > /dev/null 2>&1") == 0) return "clang++";
    if (system("g++ --version > /dev/null 2>&1") == 0) return "g++";
    if (system("c++ --version > /dev/null 2>&1") == 0) return "c++";
    return NULL;
}

const char* get_obj_path(const char* source) {
    const char *src_name = nob_path_name(source);
    return nob_temp_sprintf(OBJ_DIR "/%s.o", src_name);
}

void clean_build() {
    nob_log(NOB_INFO, "cleaning build directory");
    for (size_t i = 0; i < NOB_ARRAY_LEN(sources); ++i) {
        nob_delete_file(get_obj_path(sources[i]));
    }

    nob_delete_file("build/calc");
}

bool ensure_build_dirs(void) {
    if (!nob_mkdir_if_not_exists(BUILD_DIR)) return false;
    if (!nob_mkdir_if_not_exists(OBJ_DIR)) return false;
    return true;
}

bool build_project(int jobs) {
    if (!ensure_build_dirs()) return false;

    Nob_Procs procs = {0};
    bool needs_linking = false;
    const char* binary_path = BUILD_DIR"/calc";

    for (size_t i = 0; i < NOB_ARRAY_LEN(sources); ++i) {
        const char *src_path = sources[i];
        const char *obj_path = get_obj_path(src_path);

        if (!nob_needs_rebuild1(obj_path, src_path)) continue;

        needs_linking = true;
        nob_log(NOB_INFO, "Compiling %s", src_path);

        const char *compiler = get_compiler();
        if (!compiler) {
            nob_log(NOB_ERROR, "No C++ compiler found (tried clang++, g++, c++)");
            return false;
        }

        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, compiler);
        for (size_t j = 0; j < NOB_ARRAY_LEN(flags); ++j) {
            nob_cmd_append(&cmd, flags[j]);
        }

        nob_cmd_append(&cmd, "-c", src_path, "-o", obj_path);

        // run async and append to proc pool
        nob_da_append(&procs, nob_cmd_run_async(cmd));

        // flush batch if we hit the thread limit
        if ((int)procs.count >= jobs) {
            if (!nob_procs_wait(procs)) return false;
            procs.count = 0; // Reset pool for the next batch
        }
    }

    // wait for any remaining processes in the final batch
    if (procs.count > 0) {
        if (!nob_procs_wait(procs)) return false;
    }

    // check if output binary is missing even if objects are up to date
    for (size_t i = 0; i < NOB_ARRAY_LEN(sources); ++i) {
        if (nob_needs_rebuild1(binary_path, get_obj_path(sources[i]))) {
            needs_linking = true;
            break;
        }
    }

    if (!needs_linking) {
        nob_log(NOB_INFO, "Up to date.");
        return true;
    }

    nob_log(NOB_INFO, "Linking...");
    Nob_Cmd link_cmd = {0};

    const char *compiler = get_compiler();
    if (!compiler) {
        nob_log(NOB_ERROR, "No C++ compiler found for linking");
        return false;
    }

    nob_cmd_append(&link_cmd, compiler, "-o", binary_path);

    for (size_t i = 0; i < NOB_ARRAY_LEN(sources); ++i) {
        nob_cmd_append(&link_cmd, get_obj_path(sources[i]));
    }

    bool success = nob_cmd_run_sync(link_cmd);
    nob_cmd_free(link_cmd);
    return success;
}

int main(int argc, char* argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    int jobs = 1;
    bool is_clean = false;
    bool is_run = false;
    int run_args_start = 0;

    // Parse CLI arguments
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-j", 2) == 0) {
            if (strlen(argv[i]) > 2) {
                jobs = atoi(argv[i] + 2);
            } else if (i + 1 < argc) {
                jobs = atoi(argv[++i]);
            }
        } else if (strcmp(argv[i], "clean") == 0) {
            is_clean = true;
        } else if (strcmp(argv[i], "run") == 0) {
            is_run = true;
            run_args_start = i + 1;
            break;
        }
    }

    if (jobs < 1) jobs = 1;

    if (is_clean) {
        clean_build();
        return 0;
    }

    if (is_run) {
        if (!build_project(jobs)) return 1;

        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "./build/calc");
        for (int i = run_args_start; i < argc; i++) nob_cmd_append(&cmd, argv[i]);
        bool success = nob_cmd_run_sync(cmd);
        nob_cmd_free(cmd);
        return success ? 0 : 1;
    }

    bool proj_ok = build_project(jobs);

    return (proj_ok) ? 0 : 1;
}
