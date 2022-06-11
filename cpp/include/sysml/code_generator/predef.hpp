#pragma once

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) ||           \
    defined(__x86_64)
#    define SYSML_CODE_GENERATOR_ARCHITECTURE_AMD64
#elif defined(__aarch64__)
#    define SYSML_CODE_GENERATOR_ARCHITECTURE_AARCH64
#else
#    error "Target architecture not supported by sysml::code_generator library"
#endif
