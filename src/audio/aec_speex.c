/**
 * @file aec_speex.c
 * @brief Acoustic echo cancellation entry points.
 *
 * No echo-cancellation backend is built in: every call is a passthrough. A NULL context means
 * "passthrough" to the callers. (The file keeps its old name so the build lists stay unchanged.)
 */

#include "ethervox/aec.h"
#include "ethervox/logging.h"

ethervox_aec_config_t ethervox_aec_default_config(void) {
    ethervox_aec_config_t config = {
        .sample_rate = 16000,
        .frame_size = 160,           // 10ms @ 16kHz
        .filter_length = 1024,       // ~64ms echo tail
        .backend = ETHERVOX_AEC_SPEEX,
        .suppression_level = 0.5f,   // Moderate suppression
    };
    return config;
}

ethervox_aec_t* ethervox_aec_create(const ethervox_aec_config_t* config) {
    ETHERVOX_LOG_WARN("No AEC backend built in - echo cancellation disabled (passthrough mode)");
    return NULL;  // Null AEC context means passthrough in the calling code
}

void ethervox_aec_set_reference(ethervox_aec_t* aec, const float* reference, size_t count) {
    // No-op
}

ethervox_result_t ethervox_aec_process(ethervox_aec_t* aec, float* mic_input, size_t count) {
    // Passthrough - no processing
    return ETHERVOX_SUCCESS;
}

void ethervox_aec_reset(ethervox_aec_t* aec) {
    // No-op
}

void ethervox_aec_destroy(ethervox_aec_t* aec) {
    // No-op
}

bool ethervox_aec_is_active(const ethervox_aec_t* aec) {
    return false;
}
