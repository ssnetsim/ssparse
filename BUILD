licenses(["notice"])

exports_files([
    "LICENSE",
    "NOTICE",
])

COPTS = [
    "-UNDEBUG",
]

LIBS = [
    "@libprim//:prim",
    "@libex//:ex",
    "@libfio//:fio",
    "@libstrop//:strop",
    "@libmut//:mut",
    "@tclap//:tclap",
    "@zlib//:zlib",
]

cc_library(
    name = "lib",
    srcs = glob(
        ["src/**/*.cc"],
        exclude = [
            "src/main.cc",
            "src/**/*_TEST*",
        ],
    ),
    hdrs = glob(
        [
            "src/**/*.h",
            "src/**/*.tcc",
        ],
        exclude = ["src/**/*_TEST*"],
    ),
    copts = COPTS,
    includes = [
        "src",
    ],
    visibility = ["//visibility:private"],
    deps = LIBS,
    alwayslink = 1,
)

cc_binary(
    name = "ssparse",
    srcs = ["src/main.cc"],
    copts = COPTS,
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":lib",
    ] + LIBS,
)

cc_library(
    name = "test_lib",
    testonly = 1,
    srcs = glob([
        "src/**/*_TEST*.cc",
    ]),
    hdrs = glob([
        "src/**/*_TEST*.h",
        "src/**/*_TEST*.tcc",
    ]),
    copts = COPTS,
    visibility = ["//visibility:private"],
    deps = [
        ":lib",
        "@googletest//:gtest_main",
    ] + LIBS,
    alwayslink = 1,
)

cc_test(
    name = "ssparse_test",
    args = [
        "--gtest_color=yes",
    ],
    copts = COPTS,
    visibility = ["//visibility:public"],
    deps = [
        ":test_lib",
    ] + LIBS,
)

genrule(
    name = "lint",
    srcs = glob([
        "src/**/*.cc",
    ]) + glob([
        "src/**/*.h",
        "src/**/*.tcc",
    ]),
    outs = ["linted"],
    cmd = """
    python $(location @cpplint//:cpplint) \
      --root=$$(pwd)/src \
      --headers=h,tcc \
      --extensions=cc,h,tcc \
      --quiet $(SRCS) > $@
    echo // $$(date) > $@
  """,
    tools = [
        "@cpplint",
    ],
    visibility = ["//visibility:public"],
)

genrule(
    name = "format_check",
    srcs = glob([
        "src/**/*.cc",
        "src/**/*.h",
        "src/**/*.tcc",
    ]),
    outs = ["format_checked"],
    cmd = """
    cp $(location @clang_format//file) .clang-format
    clang-format --style=file --dry-run --Werror $(SRCS)
    echo // $$(date) > $@
    """,
    tools = [
        "@clang_format//file",
    ],
    visibility = ["//visibility:public"],
)

py_binary(
    name = "transient",
    srcs = ["scripts/transient.py"],
    main = "scripts/transient.py",
    python_version = "PY3",
    visibility = ["//visibility:public"],
)

sh_test(
    name = "transient_check",
    srcs = ["test/transient.sh"],
    data = [
        ":transient",
        ":ssparse",
        "test/fattree_iq_blast.mpf.gz",
    ],
    visibility = ["//visibility:public"],
)
